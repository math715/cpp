//
// Created by ruoshui on 12/19/18.
//

#include <cassert>
#include <stdint-gcc.h>
#include "bucket.h"
#include "Tx.h"
#include "node.h"
#include "page.h"
#include "cursor.h"


namespace boltdb {
    Bucket::Bucket(boltdb::Tx *tx):tx(tx), FillPercent(DefaultFillPercent) {

    }

    std::pair<page *, node *> Bucket::pageNode(boltdb::pgid id) {
        if (root == 0) {
            if (id != 0) {
                perror("inline bucket non-zero page access(2): %d != 0");
            }
            if (rootNode != nullptr) {
                return std::make_pair(nullptr,  rootNode);
            }
            return std::make_pair(page_, nullptr);
        }

        // Check the node cache for non-inline buckets.
        if (!nodes.empty()) {
            if (nodes[id] != nullptr) {
                return std::make_pair(nullptr, nodes[id]);
            }
        }

        // Finally lookup the page from the transaction if no node is materialized.
        return std::make_pair(tx->Page(id), nullptr);
    }


    node* Bucket::Node(boltdb::pgid id, boltdb::node *parent) {
        assert(nodes.empty());
        auto n = nodes[id];
        if (n != nullptr) {
            return n;
        }
        n = new node;
        n->bucket = this;
        n->parent = parent;
        if (parent == nullptr) {
            rootNode  = n;
        } else {
            parent->children.push_back(n);
        }
        auto p = page_;
        if (p == nullptr) {
            p = tx->Page(id);
        }
        n->read(p);
        nodes[id] = n;
        tx->stats.NodeCount++;
        return n;
    }

    void Bucket::rebalance() {
        for (auto n : nodes) {
            n.second->rebalance();
        }
        for (auto child : buckets) {
            child.second->rebalance();
        }
    }


    int Bucket::maxInlineBucketSize() {
        return tx->db_->pageSize/4;
    }
    bool Bucket::inlineable() {

        auto  n = rootNode;

        // Bucket must only contain a single leaf node.
        if (n == nullptr || !n->isLeaf) {
            return false;
        }

        // Bucket is not inlineable if it contains subbuckets or if it goes beyond
        // our threshold for inline bucket size.
        auto size = pageHeaderSize;
        for (auto &ns : n->inodes) {
            size += leafPageElementSize + ns->key.size() + ns->value.size();

            if (ns->flags&bucketLeafFlag != 0 ){
                return false;
            } else if (size > maxInlineBucketSize()) {
                return false;
            }
        }

        return true;
    }

    void Bucket::free() {
        if (root == 0 ){
            return;
        }

//        auto tx = tx;
//        auto
/*  TODO
        b.forEachPageNode(func(p *page, n *node, _ int) {
            if p != nil {
                        tx.db.freelist.free(tx.meta.txid, p)
                } else {
                n.free()
            }
        })
        */
        root = 0;
    }


    cursor* Bucket::newCursor() {
        tx->stats.CursorCount++;
        cursor *c = new cursor();
        c->bucket = this;
        return c;
    }

    void Bucket::dereference() {
        if (rootNode != nullptr) {
            rootNode->root()->dereference();
        }

        for ( auto child : buckets) {
            child.second->dereference();
        }
    }
    boltdb_key_t Bucket::write() {
        auto  n = rootNode;
//        var value = make([]byte, bucketHeaderSize+n.size());
        auto sz = bucketHeaderSize + n->size();
        char *value = new char[sz] {0};

        // Write a bucket header.
        bucket* b = reinterpret_cast<bucket *>(value);
        b->root = root;
        b->sequence = sequence;

        // Convert byte slice to a fake page and write the root node.
        page  *p = reinterpret_cast<page *>(&value[bucketHeaderSize]);
        n->write(p);
        boltdb_key_t slice(value, sz);
        delete [] value;
        return slice;

    }

    Status Bucket::spill() {
        // Spill all child buckets first.
        for( auto &b :buckets ){
            boltdb_key_t name = b.first;
            auto &child = b.second;
            // If the child bucket is small enough and it has no child buckets then
            // write it inline into the parent bucket's page. Otherwise spill it
            // like a normal bucket and make the parent value a pointer to the page.
//            var value []byte
            boltdb_key_t value;
            if (child->inlineable()) {
                child->free();
                value = child->write();
            } else {
                auto err = child->spill();
                if (!err.ok()){
                    return err;
                }

                // Update the child bucket header in this bucket.
//                value = new char[sizeof(bucket)];
                char *v = new char[sizeof(bucket)];
//                char *v = const_cast<char *>(value.data());
                bucket *b = reinterpret_cast<bucket*>(&v[0]);
                b->root = child->root;
                b->sequence = child->sequence;
                value = boltdb_key_t(v, sizeof(bucket));
            }

            // Skip writing the bucket if there are no materialized nodes.
            if (child->rootNode == nullptr) {
                continue;
            }

            // Update parent node.
            cursor  *c = newCursor();
//            k, _, flags
            auto kv = c->seek(name);
            auto &k = std::get<0>(kv);
            auto &flags= std::get<2>(kv);

            if (k != name) {
                assert(true);
//                panic(fmt.Sprintf("misplaced bucket header: %x -> %x", []byte(name), k))
            }
            if (flags&bucketLeafFlag == 0) {
//                panic(fmt.Sprintf("unexpected bucket header flag: %x", flags));
                assert(true);
            }
            c->Node()->put(name, name, value, 0, bucketLeafFlag);
            delete c;
//            c.node().put([]byte(name), []byte(name), value, 0, bucketLeafFlag)
        }

        // Ignore if there's not a materialized root node.
        if (rootNode == nullptr) {
            return Status::Ok();
        }

        // Spill nodes.
        auto err = rootNode->spill();
        if (!err.ok()) {
            return err;
        }
        rootNode = rootNode->root();

        // Update the root node for this bucket.
        if (rootNode->pgid_ >= tx->meta_->pgcnt) {
            assert(true);
//            panic(fmt.Sprintf("pgid (%d) above high water mark (%d)", b.rootNode.pgid, b.tx.meta.pgid))
        }
        root = rootNode->pgid_;

        return Status::Ok();
    }



    XXStatus<Bucket *> Bucket::CreateBucket(boltdb::boltdb_key_t name) {
        if (tx->db_ == nullptr ){
            Status    ErrTxClosed =  Status::TxError("Tx Closed");
            return XXStatus<Bucket *> (nullptr, ErrTxClosed);
        } else if (!tx->writable ){
            return XXStatus<Bucket *>(nullptr, Status::TxError("Tx not writable"));
        } else if (name.size() == 0) {
//            return nil, ErrBucketNameRequired
            return XXStatus<Bucket*> (nullptr, Status::TxError("Bucket name required "));
        }

        // Move cursor to correct position.
        auto c = newCursor();
        auto kvflag = c->seek(name);

        // Return an error if there is an existing key.
        if (std::get<0>(kvflag) == name) {
            if ( (std::get<2>(kvflag) & bucketLeafFlag) != 0) {
                return XXStatus<Bucket *>(nullptr, Status::BucketError("bucket already exists"));
            }
            return XXStatus<Bucket*> (nullptr, Status::BucketError("incompatible value"));
        }


        // Create empty, inline bucket.
        Bucket *b = new Bucket();
        b->root = 0;
        b->sequence = 0;
        b->rootNode = new node;
        b->rootNode->isLeaf = true;
        b->FillPercent = DefaultFillPercent;
        boltdb_key_t v = b->write();

        c->Node()->put(name, name, v, 0, bucketLeafFlag);
        // Insert into node.
//        key = cloneBytes(key)
//        c->Node()->put(key, key, v, 0, bucketLeafFlag);

        // Since subbuckets are not allowed on inline buckets, we need to
        // dereference the inline page, if it exists. This will cause the bucket
        // to be treated as a regular, non-inline bucket for the rest of the tx.
        page_ = nullptr;
        delete c;
        delete b->rootNode;
        delete b;

        return XXStatus<Bucket*> (GetBucket(name), Status::Ok());
    }


    Bucket* Bucket::GetBucket(boltdb::boltdb_key_t &key) {
        if (!buckets.empty()) {
            if (buckets.find(key) != buckets.end()){
                return buckets[key];
            }
        }

        // Move cursor to key.
        auto c = newCursor();
        auto kvflag = c->seek(key);

        // Return nil if the key doesn't exist or it is not a bucket.
        if ((std::get<0>(kvflag) != key) || (std::get<2>(kvflag) &bucketLeafFlag) == 0 ) {
            return nullptr;
        }

        // Otherwise create a bucket and cache it.
        auto  child = openBucket(std::get<1>(kvflag));
        if (buckets.empty()) {
            buckets[key] = child;
        }
        delete c;
        return child;
    }


    Bucket* Bucket::openBucket(boltdb::boltdb_key_t &value) {
        auto child = new Bucket(tx);

        // If unaligned load/stores are broken on this arch and value is
        // unaligned simply clone to an aligned byte array.
//        unaligned := brokenUnaligned && uintptr(unsafe.Pointer(&value[0]))&3 != 0
        ;
        bool unaligned = (reinterpret_cast<uintptr_t >(value.data()) & 3) != 0;
        if (unaligned) {
//            value  = value.AlignedClone(value);
            value = value;
        }

        // If this is a writable transaction then we need to copy the bucket entry.
        // Read-only transactions can point directly at the mmap entry.
        if (tx->writable && !unaligned) {
            bucket *b = reinterpret_cast<bucket *>(const_cast<char *> (value.data()));
            child->root = b->root;
            child->sequence = b->sequence;
        } else {
            bucket *b = reinterpret_cast<bucket *> (const_cast<char *>(value.data()));
            child->root = b->root;
            child->sequence = b->sequence;
        }

        // Save a reference to the inline page if the bucket is inline.
        if (child->root == 0) {
            child->page_ = reinterpret_cast<page *>(const_cast<char *>(&(value.data()[bucketHeaderSize])));
        }

        return child;
    }

    boltdb_key_t Bucket::Get(boltdb::boltdb_key_t &key) {
        auto c = newCursor();
        auto key_value_flags = c->seek(key);

        // Return nil if this is a bucket.
        if ((std::get<2>(key_value_flags) & bucketLeafFlag) != 0) {
            return nil;
        }

        // If our target node isn't the same key as what's passed in then return nil.
        if (key != std::get<0>(key_value_flags)) {
            return nil;
        }
        return std::get<1>(key_value_flags);
    }


    Status Bucket::Put(boltdb_key_t key, boltdb_key_t value) {
        if (tx->db_ == nullptr) {
            return Status::TxError(ErrTxClosed);
        } else if (!Writeable()) {
            return Status::TxError(ErrTxNotWritable);
        } else if (key.size() == 0) {
            return Status::TxError(ErrKeyRequired);
        } else if (key.size() > MaxKeySize) {
            return Status::TxError(ErrKeyTooLarge);
        } else if (value.size() > MaxValueSize) {
            return Status::TxError(ErrValueTooLarge);
        }

        // Move cursor to correct position.
        auto c = newCursor();
        auto kvf = c->seek(key);

        // Return an error if there is an existing key with a bucket value.
        if ( (std::get<0>(kvf) == key)
                && ((std::get<2>(kvf)&bucketLeafFlag) != 0 )) {
            return Status::TxError(ErrIncompatibleValue);
        }

        // Insert into node.
        c->Node()->put(key, key, value, 0, 0);

        return Status::Ok();

    }


    Status Bucket::Delete(boltdb::boltdb_key_t key) {
        if (tx->db_ == nullptr) {
            return Status::TxError(ErrTxClosed);
        } else if (!Writeable()) {
            return Status::TxError(ErrTxNotWritable);
        }

        // Move cursor to correct position.
        auto c = newCursor();
        auto kvf = c->seek(key);

        // Return an error if there is already existing bucket value.
        if ((std::get<2>(kvf) & bucketLeafFlag) != 0) {
            return Status::TxError(ErrIncompatibleValue);
        }

        // Delete the node if we have a matching key.
        c->Node()->del(key);

        return Status::Ok();

    }

    Status Bucket::DeleteBucket(boltdb::boltdb_key_t key) {
        if (tx->db_ == nullptr) {
            return Status::TxError(ErrTxClosed);
        } else if (!Writeable()) {
            return Status::TxError(ErrTxNotWritable);
        }

        // Move cursor to correct position.
        auto c = newCursor();
        auto kvf = c->seek(key);

        // Return an error if bucket doesn't exist or is not a bucket.
        if (std::get<0>(kvf) != key ) {
            return Status::BucketError(ErrBucketNotFound);
        } else if ((std::get<2>(kvf) & bucketLeafFlag) == 0 ){
            return Status::BucketError(ErrIncompatibleValue);
        }

        // Recursively delete all child buckets.
        auto child = GetBucket(key);
        auto delfn = [&](boltdb_key_t k, boltdb_key_t v)->Status {
            if (v.empty()) {
                auto err = child->DeleteBucket(k);
                if (!err.ok()) {
                    return err;
                }
            }
        };
        auto err = child->ForEach(delfn);

        if (!err.ok()) {
                    return err;
         }

        // Remove cached copy.
        buckets.erase(key);

        // Release all bucket pages to freelist.
        child->nodes.clear();
        child->rootNode = nullptr;
        child->free();

        // Delete the node if we have a matching key.
        c->Node()->del(key);

        return Status::Ok();
    }

    Status Bucket::ForEach(std::function<Status(boltdb_key_t, boltdb_key_t)> fn) {
        if (tx->db_ == nullptr) {
            return Status::TxError(ErrTxClosed);
        }
        auto c = newCursor();
        for ( auto kv= c->First(); !kv.first.empty(); kv = c->Next()) {
            auto err = fn(kv.first, kv.second);
            if (!err.ok()) {
                return err;
            }
        }
        return Status::Ok();
    }
    inline  bool Bucket::Writeable() {
        return tx->writable;
    }

    BucketStats Bucket::Stats() {


    }
}