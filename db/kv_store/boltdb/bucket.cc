//
// Created by ruoshui on 12/19/18.
//

#include <cassert>
#include "bucket.h"
#include "Tx.h"
#include "node.h"
#include "page.h"
#include "Cursor.h"


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
        for (auto child = buckets) {
            child->rebalance();
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


    Cursor* Bucket::newCursor() {
        Cursor *c = new Cursor();
        c->bucket = this;
        return c;
    }
    char * Bucket::write() {
        auto  n = rootNode;
//        var value = make([]byte, bucketHeaderSize+n.size());
        char *value = new char[bucketHeaderSize + n->size()];
        // Write a bucket header.
        Bucket* b = reinterpret_cast<Bucket *>(value);
        b->root = root;
        b->sequence = sequence;

        // Convert byte slice to a fake page and write the root node.
        page  *p = reinterpret_cast<page *>(&value[bucketHeaderSize]);
        n->write(p);

        return value;

    }

    Status Bucket::spill() {
        // Spill all child buckets first.
        for( auto &b :buckets ){
            auto &name = b.first;
            auto &child = b.second;
            // If the child bucket is small enough and it has no child buckets then
            // write it inline into the parent bucket's page. Otherwise spill it
            // like a normal bucket and make the parent value a pointer to the page.
//            var value []byte
            char *value;
            if (child->inlineable()) {
                child->free();
                value = child->write();
            } else {
                auto err = child->spill();
                if (!err.ok()){
                    return err;
                }

                // Update the child bucket header in this bucket.
                value = new char[sizeof(bucket)];
                bucket *b = reinterpret_cast<bucket*>(&value[0]);
                b->root = child->root;
                b->sequence = child->sequence;
            }

            // Skip writing the bucket if there are no materialized nodes.
            if (child->rootNode == nullptr) {
                        continue;
            }

            // Update parent node.
            Cursor  *c = newCursor();
//            k, _, flags
            auto kv = c->seek(name);
            auto &k = std::get<0>(kv);
            auto &flags= std::get<2>(kv);

            if (k != name) {
                assert(true);
//                panic(fmt.Sprintf("misplaced bucket header: %x -> %x", []byte(name), k))
            }
            if (flags&bucketLeafFlag == 0) {
                panic(fmt.Sprintf("unexpected bucket header flag: %x", flags));
                assert(true);
            }
            c->Node().
//            c.node().put([]byte(name), []byte(name), value, 0, bucketLeafFlag)
        }

        // Ignore if there's not a materialized root node.
        if b.rootNode == nil {
                    return nil
            }

        // Spill nodes.
        if err := b.rootNode.spill(); err != nil {
                return err
        }
        b.rootNode = b.rootNode.root()

        // Update the root node for this bucket.
        if b.rootNode.pgid >= b.tx.meta.pgid {
            panic(fmt.Sprintf("pgid (%d) above high water mark (%d)", b.rootNode.pgid, b.tx.meta.pgid))
        }
        b.root = b.rootNode.pgid

        return nil
    }
}