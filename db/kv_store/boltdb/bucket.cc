//
// Created by ruoshui on 12/19/18.
//

#include <cassert>
#include "bucket.h"
#include "Tx.h"
#include "node.h"


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
}