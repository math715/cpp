//
// Created by ruoshui on 12/28/18.
//

#include <cassert>
#include "Cursor.h"
#include "bucket.h"
#include "Tx.h"
#include "node.h"
#include "page.h"

namespace boltdb {
    std::pair<std::vector<char>, std::vector<char>> Cursor::First() {
        assert(bucket->tx->db_ == nullptr);
        stack.clear();
        auto elem = bucket->pageNode(bucket->root);
        elemRef er;
        er.page_ = elem.first;
        er.node_ = elem.second;
        er.index = 0;
        stack.push_back(er);
        first();
        if (stack[stack.size()-1].count() == 0) {
                next();
        }





    }

    bool elemRef::isLeaf() {
            if (node_ != nullptr) {
                    return node_->isLeaf;
            }
            return (page_->flags & leafPageFlag) != 0;

    }

    int elemRef::count() {
            if (node_ != nullptr) {
                    return node_->inodes.size();
            }
            return int(page_->count);
    }

    void Cursor::first() {
            while (true) {
                    // Exit when we hit a leaf page.

                    auto &ref = stack[stack.size()-1];
                    if (ref.isLeaf()) {
                            break;
                    }

                    // Keep adding pages pointing to the first element to the stack.
                    pgid id;
                    if (ref.node_ != nullptr) {
                            id = ref.node_->inodes[ref.index]->pgid_;
                    } else {
                            id = ref.page_->BranchPageElement(uint16_t(ref.index))->pgid_;
                    }
                    auto pn= bucket->pageNode(id);
                    stack.push_back(elemRef{pn.first, pn.second, index: 0});
            }
    }

    std::tuple<std::vector<char>, std::vector<char>, uint32_t> Cursor::next() {
            while(true) {
                    // Attempt to move over one element until we're successful.
                    // Move up the stack as we hit the end of each page in our stack.
                    int i;
                    for (i = stack.size() - 1; i >= 0; i--) {
                        auto &elem = stack[i];
                        if (elem.index < elem.count()-1) {
                                    elem.index++;
                                    break;
                        }
                    }

                    // If we've hit the root page then stop and return. This will leave the
                    // cursor on the last element of the last page.
                    if (i == -1) {
                            return std::make_tuple(nullptr, nullptr, 0);
                    }

                    // Otherwise start from where we left off in the stack and find the
                    // first element of the first leaf page.
                    c.stack = c.stack[:i+1];
                    first();

                    // If this is an empty page then restart and move back up the stack.
                    // https://github.com/boltdb/bolt/issues/450
                    if (stack[stack.size()-1].count() == 0){
                            continue;
                    }

                    return keyValue()
            }

    }
}