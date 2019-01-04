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
        elemRef er(elem.first, elem.second, 0);
        stack.push_back(er);
        first();
        if (stack[stack.size()-1].count() == 0) {
                next();
        }

        auto kv = keyValue();
        std::vector<char> v;
        if ( (std::get<2>(kv) & uint32_t(bucketLeafFlag)) != 0 ) {
            return std::make_pair(std::get<0>(kv), v);
        }
        return std::make_pair(std::get<0>(kv), std::get<1>(kv));
    }


    std::pair<std::vector<char>, std::vector<char>> Cursor::Last() {
        assert(bucket->tx->db_ != nullptr);
        stack.clear();
        auto elem = bucket->pageNode(bucket->root);
        elemRef er(elem.first, elem.second);
        er.index = er.count() - 1;
        stack.push_back(er);
        last();
        auto kv  = keyValue();
        std::vector<char> v;

        if ((std::get<2>(kv)  & uint32_t(bucketLeafFlag)) != 0) {
            return std::make_pair(std::get<0>(kv), v);
        }
        return std::make_pair(std::get<0>(kv), std::get<1>(kv));
    }

    std::pair<std::vector<char>, std::vector<char>> Cursor::Next() {
        auto kv = next();
        std::vector<char> v;
        if ((std::get<2>(kv) & uint32_t(bucketLeafFlag)) != 0 ) {
            return std::make_pair(std::get<0>(kv), v);
        }
        return std::make_pair(std::get<0>(kv), std::get<1>(kv));
    }

    std::pair<std::vector<char>, std::vector<char>> Cursor::Prev() {
        assert(bucket->tx->db_ != nullptr);

        // Attempt to move back one element until we're successful.
        // Move up the stack as we hit the beginning of each page in our stack.
        for ( int i = stack.size() - 1; i >= 0; i--) {
            auto &elem = stack[i];
            if (elem.index > 0 ) {
                elem.index--;
                break;
            }
            stack.resize(i);
        }
        std::vector<char> k;
        // If we've hit the end then return nil.
        if (stack.size() == 0) {
            return std::make_pair(k, k);
        }

        // Move down the stack to find the last element of the last leaf under this branch.
        last();
        auto kv= keyValue();
        std::vector<char> nil;
        if ((std::get<2>(kv) & uint32_t(bucketLeafFlag)) != 0) {
            return std::make_pair(std::get<0>(kv), nil);
        }
        return std::make_pair(std::get<0>(kv), std::get<1>(kv));
    }



    std::pair<std::vector<char>, std::vector<char>> Cursor::Seek(std::vector<char> key) {
        auto kv = seek(key);
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
                            std::vector<char> key, value;
                            return std::make_tuple(key, value, 0);
                    }

                    // Otherwise start from where we left off in the stack and find the
                    // first element of the first leaf page.
//                    stack = c.stack[:i+1];
                    stack.resize(i+1);
                    first();

                    // If this is an empty page then restart and move back up the stack.
                    // https://github.com/boltdb/bolt/issues/450
                    if (stack[stack.size()-1].count() == 0){
                            continue;
                    }

                    return keyValue();
            }

    }

    std::tuple<std::vector<char>, std::vector<char>, uint32_t> Cursor::seek(std::vector<char> key) {
        stack.resize(0);
        search(key, bucket->root);
        auto ref = stack.back();
        if (ref.index >= ref.count()) {
            std::vector<char> k, v;
            return std::make_tuple(k, v, 0);
        }

        // If this is a bucket then return a nil value.
        return keyValue();

    }


    void Cursor::last() {
        while (true) {
            // Exit when we hit a leaf page.
            auto &ref = stack[stack.size()-1];
            if (ref.isLeaf()) {
                break;
            }

            // Keep adding pages pointing to the last element in the stack.
            pgid id;
            if (ref.node_ != nullptr) {
                id = ref.node_->inodes[ref.index]->pgid_;
            } else {
                id = ref.page_->BranchPageElement(uint16_t(ref.index))->pgid_;
            }
            auto elem = bucket->pageNode(id);

            elemRef er(elem.first, elem.second);
            er.index = er.count() - 1;
            stack.push_back(er);
        }
    }

    void Cursor::search(std::vector<char> key, boltdb::pgid id) {
        auto elem = bucket->pageNode(id);
        if (elem.first != nullptr && (elem.first->flags &(branchPageFlag|leafPageFlag) == 0)) {
            assert(true);
        }
        auto e = elemRef(std::get<0>(elem), std::get<1>(elem));
        stack.push_back(e);
        if (e.isLeaf()) {
            nsearch(key);
            return ;
        }
        if (elem.second != nullptr) {
            searchNode(key, elem.second);
            return ;
        }
        searchPage(key, elem.first);

    }

    void Cursor::nsearch(std::vector<char> key) {
        auto &e = stack[stack.size()-1];
        auto p = e.page_;
        auto n = e.node_;

        // If we have a node then search its inodes.
        if (n != nullptr) {
            auto it = std::lower_bound(n->inodes.begin(), n->inodes.end(), key);
            e.index = it - n->inodes.begin();
            return ;
        }

        // If we have a page then search its leaf elements.
        //TODO b search
        auto inodes = p->LeafPageElements();
        int index = p->count;
        for (int i = 0; i < p->count; ++i) {
            if (inodes[i].key() == key) {
                index = i;
                break;
            }
        }
        e.index = index;

    }


    void Cursor::searchPage(std::vector<char> key, boltdb::page *p) {
        auto inodes = p->BranchPageElements();
        bool exact = false;
        int index = p->count;
        for (int i = 0; i < p->count; ++i) {
            if (inodes->key() == key) {
                index = i;
                exact = true;
                break;
            }
        }
        if (index > 0 && !exact) {
            index --;
        }
        stack.back().index = index;

        search(key, inodes[index].pgid_);

    }

    void Cursor::searchNode(std::vector<char> key, boltdb::node *n) {
        bool exact = false;
        int index = n->inodes.size();
        for (int i = 0; i < n->inodes.size(); ++i) {
            if (n->inodes[i]->key == key) {
                index = i;
                exact = true;
                break;
            }
        }
        if (!exact && index > 0){
            index--;
        }
        stack.back().index = index;
        search(key, n->inodes[index]->pgid_);

    }

    std::tuple<std::vector<char>, std::vector<char>, uint32_t> Cursor::keyValue() {
            std::vector<char> key, value;
        auto &ref = stack[stack.size()-1];
        if (ref.count() == 0 || ref.index >= ref.count()) {
            return std::make_tuple(key, value, 0);
        }

// Retrieve value from node.
        if (ref.node_ != nullptr) {
            auto inode = ref.node_->inodes[ref.index];
            return std::make_tuple(inode->key, inode->key, inode->flags);
        }

// Or retrieve value from page.
        auto elem = ref.page_->LeafPageElement(uint16_t(ref.index));
        return std::make_tuple(elem->key(), elem->value(), elem->flags);
    }

    Status Cursor::Delete() {
        if (bucket->tx->db_ == nullptr) {
            return Status::Corruption("ex closed");
        } else if (!bucket->tx->writable) {
            return Status::Corruption("tx not writable");
        }

        auto kv = keyValue();
        // Return an error if current value is a bucket.
        if ((std::get<2>(kv) & bucketLeafFlag) != 0) {
            return Status::Corruption("incompatible value");
        }
        node().del(std::get<0>(kv));

        return Status::Ok();
    }


    node* Cursor::Node() {
        assert(stack.size() > 0);
//        , "accessing a node with a zero-length cursor stack")

        // If the top of the stack is a leaf node then just return it.
        auto ref = stack.back();
        if (ref.node_ != nullptr && ref.isLeaf()) {
            return ref.node_;
        }

        // Start from root and traverse down the hierarchy.
        auto n = stack[0].node_;
        if (n == nullptr) {
            n = bucket->Node(stack[0].page_->id, nullptr);
        }
        for (auto ref : stack) {
            auto n = ref.node_;
            assert(!n->isLeaf);
            n = n->childAt(int(ref.index));
        }
        assert(n->isLeaf);
        return n;
    }
}