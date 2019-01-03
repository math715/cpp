#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include "db.h"
#include "error.h"
#include "port.h"
#include "page.h"
#include "bucket.h"
#include "Tx.h"
#include "freelist.h"

#include <cstdio>
#include <cassert>

namespace boltdb {
    Status DB::open(std::string path, boltdb::Options *ops, boltdb::DB **pDB) {
        *pDB = nullptr;
        DB *db= new DB();
        (*pDB)->opened = true;
        const Options *pops = ops;
        if (ops == nullptr) {
            pops = &DefautlOptions;
        }
        db->NoGrowSync = pops->NoGrowSync;
        db->MmapFlags = pops->MmapFlags;

        db->MaxBatchSize = DefaultMaxBatchSize;
//        db->MaxBatchDelay = DefaultMaxBatchDelay;
        db->AllocSize = DefaultAllocSize;

        int flag  = O_RDWR;
        if (pops->ReadOnly) {
            flag = O_RDONLY;
            db->readOnly = true;

        }
//        int fd = ::open(path.c_str(), flag|O_CREAT, 0666);
        db->file = new File();
        auto status = db->file->Open(path.c_str(),  flag|O_CREAT, 0666);
//                fopen(path.c_str(), "r+");
//        int fd = fileno(db->file);
        if (!status.ok()) {
            return status;
        }

        int64_t sz = db->file->fileSize();
//        struct stat sb;
//        if (fstat(fd, &sb) == -1) {
//            Status status = Status::NotFound(path);
//            return status;
//        }
        if (sz == 0) {
            Status status = db->init();
            if (!status.ok()){
                return status;
            }
        } else {
            char *buf = new char[0x1000];
//            size_t sz = read(fd, buf, 0x1000);
            size_t sz = db->file->Read(buf, 1, 0x1000);
            if (sz == -1) {
                Status status = Status::IOError(path);
                return status;
            }
            auto m = db->pageInBuffer(buf, 0)->Meta();
            Status status = m->Validate();
            if (!status.ok()) {
                return status;
            }
            db->pageSize = 0x1000;
        }

//        mmaplock.lock();
        auto size = sb.st_size < pops->InitialMmapSize? pops->InitialMmapSize: sb.st_size;

        for (uint32_t i = 15; i <= 30; ++i) {
            if (size <= (1 << i)) {
                size = 1 << i;
                break;
            }
        }
        db->dataref = reinterpret_cast<char *>(mmap(NULL, size, PROT_WRITE, MAP_PRIVATE, fd, 0));
        if (db->dataref == MAP_FAILED) {
            Status status = Status::IOError("mmap");
            return status;
        }
        db->data = db->dataref;
        db->meta0 = db->Page(0)->Meta();
        db->meta1 = db->Page(1)->Meta();


        db->freelist_ = new freelist();
        db->freelist_->read(db->Page(db->Meta()->freelist));
        return Status::Ok();
    }

    page * DB::pageInBuffer(char *byte, boltdb::pgid id) {
        page *p =  reinterpret_cast<page *>(&byte[pageSize * id]);
        return p;
    }


    Status DB::init() {
        pageSize = SysCall::Getpagesize();
        char *buf = new char[4 * pageSize];

        for (int i = 0; i < 2; ++i) {
            page * p = pageInBuffer(buf, i);
            p->id = pgid(i);
            p->flags = metaPageFlag;
            meta *m = p->Meta();
            m->magic = magic;
            m->version = version;
            m->pageSize = uint32_t (pageSize);
            m->freelist = 2;
            m->root.root = 3;
            m->root.sequence = 0;
            m->pgcnt = 4;
            m->txid = txid(i);
            m->checksum = m->sum64();
        }

        page *p = pageInBuffer(buf, 2);
        p->id = pgid(2);
        p->flags = freelistPageFlag;
        p->count = 0;

        p = pageInBuffer(buf, 3);
        p->id = pgid(3);
        p->flags = leafPageFlag;
        p->count = 0;



        auto result = file->Write(buf, sizeof(char), 4 * pageSize);
        if (result != 4 * pageSize) {
            Status status = Status::IOError("write file ");
            return status;
        }
        Status status = file->Fdatasync();
        if (!status.ok()){
            return status;
        }
//        int fd = fileno(file);
//        if (fdatasync(fd) != 0) {
//            Status status = Status::IOError("data sync file ");
//
//            return status;
//        }
        status = Status::Ok();
        return status;
    }


    page* DB::Page(boltdb::pgid id) {
        auto pos = id * pageSize;
        return reinterpret_cast<page *>(&data[pos]);
    }

    meta* DB::Meta() {
        auto metaA = meta0;
        auto metaB = meta1;

        if (meta1->txid > meta0->txid) {
            metaA = meta1;
            metaB = meta0;
        }
        if (metaA->Validate().ok()) {
            return metaA;
        }
        if (metaB->Validate().ok()) {
            return metaB;
        }
        assert(true);

    }


     Status meta::Validate() {
        if (magic != boltdb::magic) {
                    Status stat = Status::InvalidArgument("magic ");
                    return stat;
        } else if (version != boltdb::version) {
                    Status stat = Status::InvalidArgument("version ");
                    return stat;
         } else if (checksum != 0 && checksum != sum64()) {
                Status stat = Status::InvalidArgument("checksum ");
                return stat;
        }
         Status stat = Status::Ok();
         return stat;
    }

    void meta::write(boltdb::page *p) {
        if (root.root >= pgcnt) {
//            panic(fmt.Sprintf("root bucket pgid (%d) above high water mark (%d)", m.root.root, m.pgid))
            assert(true);
        } else if (freelist >= pgcnt) {
            assert(true);
//            panic(fmt.Sprintf("freelist pgid (%d) above high water mark (%d)", m.freelist, m.pgid))
        }

        // Page id is either going to be 0 or 1 which we can determine by the transaction ID.
        p->id = pgid(txid % 2);
        p->flags |= metaPageFlag;

        // Calculate the checksum.
        checksum = sum64();

        *(p->Meta()) = *this;
    }

    uint64_t meta::sum64() {
        return FNV::FNV_64a(reinterpret_cast<char*>(this),sizeof (meta));
    }

    std::pair<page *, Status> DB::allocate(int count) {
        char *buf;
        if (count == 1) {
            buf = new char[pageSize];
        } else {
            buf = new char[count*pageSize];
        }
        page *p = reinterpret_cast<page *>(buf);
        p->overflow = uint32_t(count - 1);

        // Use pages from the freelist if they are available.
        p->id = freelist_->allocate(count);
        if ( p->id != 0) {
            return std::make_pair(p, Status::Ok());
        }

        // Resize mmap() if we're at the end.
        p->id = rwtx->meta_->pgcnt;
        auto minsz = int((p->id+pgid(count))+1) * pageSize;
        if (minsz >= datasz) {
            Status status = Mmap(minsz);;
            if ( !status.ok()) {
                    return std::make_pair(nullptr, status);
            }
        }

        // Move the page id high water mark.
        rwtx->meta_->pgcnt += pgid(count);

        return std::make_pair(p, Status::Ok());
    }

    Status DB::grow(int sz) {
        // Ignore if the new size is less than available file size.
        if (sz <= filesz) {
            return Status::Ok();
        }

        // If the data is smaller than the alloc size then only allocate what's needed.
        // Once it goes over the allocation size then allocate in chunks.
        if (datasz < AllocSize ){
            sz = datasz;
        } else {
            sz += AllocSize;
        }

        // Truncate and fsync to ensure file size metadata is flushed.
        // https://github.com/boltdb/bolt/issues/284
        if (!NoGrowSync && !readOnly) {
//            if runtime.GOOS != "windows" {
//            if err := db.file.Truncate(int64(sz)); err != nil {
//                    return fmt.Errorf("file resize error: %s", err)
//            }
            Status status = file->Truncate(sz);
            if (!status.ok()){
                return status;
            }
            status = file->Sync();
            if (!status.ok()) {
                return status;
            }
//            }
//            if err := db.file.Sync(); err != nil {
//                    return fmt.Errorf("file sync error: %s", err)
//            }
        }

        filesz = sz;
        return Status::Ok();
    }


    void DB::removeTx(Tx *tx) {
        db.mmaplock.RUnlock()

        // Use the meta lock to restrict access to the DB object.
        db.metalock.Lock()

        // Remove the transaction.
        for i, t := range db.txs {
            if t == tx {
                        last := len(db.txs) - 1
                        db.txs[i] = db.txs[last]
                        db.txs[last] = nil
                        db.txs = db.txs[:last]
                        break
                }
        }
        n := len(db.txs)

        // Unlock the meta pages.
        db.metalock.Unlock()

        // Merge statistics.
        db.statlock.Lock()
        db.stats.OpenTxN = n
        db.stats.TxStats.add(&tx.stats)
        db.statlock.Unlock()
    }


    Status DB::Update(std::function<Status(Tx *)> fn) {
        t, err := db.Begin(true)
        if err != nil {
                    return err
            }

        // Make sure the transaction rolls back in the event of a panic.
        defer func() {
            if t.db != nil {
                        t.rollback()
                }
        }()

        // Mark as a managed tx so that the inner function cannot manually commit.
        t.managed = true

        // If an error is returned from the function then rollback and return error.
        err = fn(t)
        t.managed = false
        if err != nil {
                    _ = t.Rollback()
                    return err
            }

        return t.Commit()
    }


    Status DB::Mmap(int minsz) {
        mmaplock.Lock();
        defer db.mmaplock.Unlock()

        info, err := db.file.Stat()
        if err != nil {
                    return fmt.Errorf("mmap stat error: %s", err)
            } else if int(info.Size()) < db.pageSize*2 {
            return fmt.Errorf("file size too small")
        }

        // Ensure the size is at least the minimum size.
        var size = int(info.Size())
        if size < minsz {
                    size = minsz
            }
        size, err = db.mmapSize(size)
        if err != nil {
                    return err
            }

        // Dereference all mmap references before unmapping.
        if db.rwtx != nil {
                    db.rwtx.root.dereference()
            }

        // Unmap existing data before continuing.
        if err := db.munmap(); err != nil {
                return err
        }

        // Memory-map the data file as a byte slice.
        if err := mmap(db, size); err != nil {
                return err
        }

        // Save references to the meta pages.
        db.meta0 = db.page(0).meta()
        db.meta1 = db.page(1).meta()

        // Validate the meta pages. We only return an error if both meta pages fail
        // validation, since meta0 failing validation means that it wasn't saved
        // properly -- but we can recover using meta1. And vice-versa.
        err0 := db.meta0.validate()
        err1 := db.meta1.validate()
        if err0 != nil && err1 != nil {
                    return err0
            }

        return nil
    }
    Status DB::View(std::function<Status(Tx *)> fn) {
        auto =  Begin(false);
        if err != nil {
                    return err
            }

        // Make sure the transaction rolls back in the event of a panic.
        defer func() {
            if t.db != nil {
                        t.rollback()
                }
        }()

        // Mark as a managed tx so that the inner function cannot manually rollback.
        t.managed = true

        // If an error is returned from the function then pass it through.
        err = fn(t)
        t.managed = false
        if err != nil {
                    _ = t.Rollback()
                    return err
            }

        if err := t.Rollback(); err != nil {
                return err
        }

        return nil
    }
}

