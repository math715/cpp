//
// Created by ruoshui on 12/19/18.
//

#include "bucket.h"
#include "Tx.h"


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
}