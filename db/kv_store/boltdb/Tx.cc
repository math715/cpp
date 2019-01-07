//
// Created by ruoshui on 12/19/18.
//

#include "Tx.h"
#include "db.h"
#include "page.h"
#include "port.h"
#include <algorithm>
#include <cassert>
#include "freelist.h"

namespace  boltdb {
    Tx::Tx() {

    }
    void Tx::init(boltdb::DB *db) {
        db_ = db;
        pages.clear();
        meta_ = new meta();
        *meta_ = *(db->Meta());
//        root = newBucket(this);
        root = Bucket(this);
        root.root = meta_->root.root;
        root.sequence = meta_->root.sequence;
        if (writable) {
            meta_->txid += txid(1);
        }
    }

    page* Tx::Page(boltdb::pgid id) {
        if (!pages.empty()) {
             if (pages.find(id) != pages.end()) {
                     return pages[id];
             }
        }
        return db_->Page(id);
    }

    std::pair<page *, Status> Tx::allocate(int count) {
        auto pe= db_->allocate(count);
        if (!pe.second.ok()) {
            return std::make_pair(nullptr, pe.second);
        }

        // Save to our page cache.
        auto p = pe.first;
        pages[p->id] = p;

        // Update statistics.
        stats.PageCount++;
        stats.PageAlloc += count * db_->pageSize;

        return std::make_pair(p, Status::Ok());
    }

    Status Tx::write() {
        std::vector<page*> pgs;
        for (auto p : pages) {
            pgs.push_back(p.second);
        }
        sort(pgs.begin(), pgs.end());
        for (auto p : pgs) {
            auto size = (int(p->overflow) + 1) * db_->pageSize;
            auto offset = int64_t(p->id) * int64_t(db_->pageSize);

            // Write out page in "max allocation" sized chunks.
//            ptr := (*[maxAllocSize]byte)(unsafe.Pointer(p))
            char *ptr = reinterpret_cast<char *>(p);
            while (true) {
                // Limit our write to our max allocation size.
                auto sz = size;
                if (sz > maxAllocSize-1 ){
                    sz = maxAllocSize - 1;
                }

                // Write chunk to disk.
                char *buf = ptr;
                Status status = db_->file->writeAt(buf, sz, offset);
                if (!status.ok()){
                    return status;
                }

                // Update statistics.
                stats.Write++;

                // Exit inner for loop if we've written all the chunks.
                size -= sz;
                if (size == 0) {
                    break;
                }

                // Otherwise move offset forward and move pointer to next chunk.
                offset += int64_t(sz);
                ptr = ptr + sz;
            }
        }

        // Ignore file sync if flag is set on DB.
        if (db_->NoSync || IgnoreNoSync) {
//            auto err = fdatasync(db_);
            auto err = db_->file->Fdatasync();
            if (!err.ok()) {
                return err;
            }
        }

        // Put small pages back to page pool.
        for (auto p : pages) {
                // Ignore page sizes over 1 page.
                // These are allocated using make() instead of the page pool.
                if (int(p.second->overflow) != 0 ) {
                    continue;
                }
                char *buf = reinterpret_cast<char *>(p.second);
//                buf := (*[maxAllocSize]byte)(unsafe.Pointer(p))[:tx.db.pageSize]

                // See https://go.googlesource.com/go/+/f03c9202c43e0abb130669852082117ca50aa9b1
                for (int i = 0; i < db_->pageSize; ++i){
                    buf[i] = 0;
                }
                db_->pagePool->put(buf);
        }

        return Status::Ok();
    }


    Status Tx::writeMeta() {
//        buf := make([]byte, tx.db.pageSize)
        char *buf = new char[db_->pageSize];
        auto p = db_->pageInBuffer(buf, 0);
        meta_->write(p);

        // Write the meta page to file.
        auto err = db_->file->writeAt(buf, db_->pageSize, p->id * int64_t(db_->pageSize));
        if (!err.ok()){
            return err;
        }
//        if _, err := tx.db.ops.writeAt(buf, int64(p.id)*int64(tx.db.pageSize)); err != nil {
//                return err
//        }
        if (!db_->NoSync || IgnoreNoSync) {
            auto err = db_->file->Fdatasync();
            if (!err.ok()){
                return err;
            }
//                    if err := fdatasync(tx.db); err != nil {
//                        return err
//                    }
        }

        // Update statistics.
        stats.Write++;

        return Status::Ok();
    }

    Status Tx::Commit() {
        assert(!managed);
        if (db_ == nullptr) {
            return Status::IOError("tx closed");
        } else if (!writable) {
            return Status::IOError("tx not writable");
        }

        // TODO(benbjohnson): Use vectorized I/O to write out dirty pages.

        // Rebalance nodes which have had deletions.
//        var startTime =
        root.rebalance();
        if (stats.Rebalance > 0) {
//            tx.stats.RebalanceTime += time.Since(startTime)
        }

        // spill data onto dirty pages.
//        startTime = time.Now()
        auto err = root.spill();
        if (!err.ok()) {
                rollback();
                return err;
        }
//        tx.stats.SpillTime += time.Since(startTime)

        // Free the old root bucket.
        meta_->root.root = root.root;

        auto opgid = meta_->pgcnt;

        // Free the freelist and allocate new pages for it. This will overestimate
        // the size of the freelist but not underestimate the size (which would be bad).
        db_->freelist_->free(meta_->txid, db_->Page(meta_->freelist));
        auto perr = allocate((db_->freelist_->size() / db_->pageSize) + 1);
        if (!perr.second.ok())  {
            rollback();
            return perr.second;
        }
        err = db_->freelist_->write(perr.first);
        if (!err.ok()) {
                rollback();
                return err;
        }
        meta_->freelist = perr.first->id;

        // If the high water mark has moved up then attempt to grow the database.
        if (meta_->pgcnt > opgid) {
            auto err = db_->grow(int(meta_->pgcnt + 1) * db_->pageSize);
            if (!err.ok()) {
                rollback();
                return err;
            }
        }

        // Write dirty pages to disk.
//        startTime = time.Now()
         err = write();
        if (!err.ok()) {
                rollback();
                return err;
        }
        //FIXME delete strict mode
        // If strict mode is enabled then perform a consistency check.
        // Only the first consistency error is reported in the panic.
        /*
        if (db_.StrictMode ) {
            ch := Check();
//            var errs []string
            while (true) {
                err, ok := <-ch
                if !ok {
                            break
                    }
                errs = append(errs, err.Error())
            }
            if len(errs) > 0 {
                panic("check fail: " + strings.Join(errs, "\n"))
            }
        }
         */
        err = writeMeta();

        // Write meta to disk.
        if (!err.ok()) {
                rollback();
                return err;
        }
//        tx.stats.WriteTime += time.Since(startTime)

        // Finalize the transaction.
        close();

        // Execute commit handlers now that the locks have been removed.
        for ( auto &fn : commitHandlers) {
            fn();
        }

        return Status::Ok();
    }


    void Tx::forEachPage(boltdb::pgid id, int depth, std::function<void(page *, int)> fn){
            auto p = Page(id);

            // Execute function.
            fn(p, depth);

            // Recursively loop over children.
            if ((p->flags & branchPageFlag) != 0 ){
                for (int i = 0; i < int(p->count); i++ ) {
                    auto elem = p->BranchPageElement(uint16_t(i));
                    forEachPage(elem->pgid_, depth+1, fn);
                }
            }
    }

    void Tx::rollback() {
        if (db_ == nullptr ){
                    return;
        }
        if (writable) {
            db_->freelist_->rollback(meta_->txid);
            db_->freelist_->reload(db_->Page(db_->Meta()->freelist));
        }
        close();
    }

    Status Tx::Rollback() {
        if (db_ == nullptr) {
            return Status::TxError("tx close");
        }
        rollback();
        return Status::Ok();
    }


    void Tx::close() {
        if (db_ == nullptr) {
                    return;
        }
        if (writable) {
            // Grab freelist stats.
            auto  freelistFreeN = db_->freelist_->free_count();
            auto  freelistPendingN = db_->freelist_->pending_count();
            auto freelistAlloc = db_->freelist_->size();

            // Remove transaction ref & writer lock.
            db_->rwtx = nullptr;
            db_->rwlock.Unlock();

            // Merge statistics.
            db_-> statlock.Lock();
            // TODO
            /*
            db.stats.FreePageN = freelistFreeN
            tx.db.stats.PendingPageN = freelistPendingN
            tx.db.stats.FreeAlloc = (freelistFreeN + freelistPendingN) * tx.db.pageSize
            tx.db.stats.FreelistInuse = freelistAlloc
            tx.db.stats.TxStats.add(&tx.stats)
             */
            db_-> statlock.Unlock();;
        } else {
            db_->removeTx(nullptr);
        }

        // Clear all references.
        db_ = nullptr;
        meta_ = nullptr;
        root = Bucket(this);
        pages.clear();
    }


    XXStatus<Bucket *, Status> Tx::CreateBucket(boltdb::boltdb_key_t &name) {
        return root.CreateBucket(name);
    }

    Bucket* Tx::GetBucket(boltdb::boltdb_key_t &key) {
        return root.GetBucket(key);
    }
}