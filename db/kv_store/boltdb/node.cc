//
// Created by ruoshui on 12/28/18.
//

#include <cassert>
#include <cstring>
#include <algorithm>
#include <iostream>
#include "node.h"
#include "page.h"
#include "Tx.h"
#include "freelist.h"


namespace  boltdb {
    void node::del(boltdb_key_t &key) {
        // Find index of key.
        auto it =  std::lower_bound(inodes.begin(), inodes.end(), key, [](inode *n, const boltdb_key_t &key)->bool{
            return n->key.compare(key) < 0;
        });

        // Exit if the key isn't found.
        if (it == inodes.end()  || (*it)->key != key) {
            return ;
        }

        // Delete inode from the node.
        inodes.erase(it);
//        n.inodes = append(n.inodes[:index], n.inodes[index+1:]...)

        // Mark the node as needing rebalancing.
        unbalanced = true;
    }

    node* node::root() {
        if (parent == nullptr) {
            return this;
        }
        return parent->root();
    }

    int node::minKeys() {
        if (isLeaf) {
            return 1;
        }
        return 2;
    }

    int node::size() {
        auto sz = pageHeaderSize;
        auto elsz = pageElementSize();
        for (int i = 0; i < inodes.size(); i++) {
            auto item = inodes[i];
            sz += elsz + item->key.size() + item->value.size();
        }
        return sz;
    }

    int node::pageElementSize() {
        if (isLeaf) {
            return leafPageElementSize;
        }
        return branchPageElementSize;
    }

    node* node::childAt(int index) {
        if (isLeaf) {
//            assert(true);
            auto err = Status::DatabaseError("node is isLeaf ", __FUNCTION__);
            std::cerr << err.ToString() << std::endl;
            abort();
        }
        return bucket->Node(inodes[index]->pgid_, this);
    }

    int node::childIndex(boltdb::node *child) {
        auto it = std::lower_bound(inodes.begin(), inodes.end(), child->key, [](inode *n, boltdb_key_t key){
            return n->key.compare(key) < 0;
        });
        auto index = it - inodes.begin();
        return index;
    }

    void node::free() {
        if (pgid_ != 0 ){
            bucket->tx->db_->freelist_->free(bucket->tx->meta_->txid, bucket->tx->Page(pgid_));
            pgid_ = 0;
        }
    }

    void node::removeChild(boltdb::node *target) {
//        std::remove_if(children.begin(), children.end(), target , [](node *a, node *t){
//
//        });
//        for (auto child : children) {
//            if (child == target ){
//                        n.children = append(n.children[:i], n.children[i+1:]...)
//                                return;
//            }
//        }
        children.erase(std::remove(children.begin(), children.end(), target), children.end());
    }

    node* node::nextSibling() {
        if (parent == nullptr) {
                    return nullptr;
            }
        auto index = parent->childIndex(this);
        if (index >= parent->numChildren()-1) {
            return nullptr;
        }
        return parent->childAt(index + 1);
    }
    node* node::prevSibling() {
        if (parent == nullptr) {
            return nullptr;
        }
        auto index = parent->childIndex(this);
        if (index == 0) {
            return nullptr;
        }
        return parent->childAt(index - 1);
    }

    void node::read(boltdb::page *p) {
        pgid_ = p->id;
        isLeaf = ((p->flags & leafPageFlag) != 0);
//        n.inodes = make(inodes, int(p.count))
        for (int i = 0; i < int(p->count); i++ ){
            auto inode_ptr = new inode();
            if (isLeaf) {
                auto elem = p->LeafPageElement(uint16_t(i));
                inode_ptr->flags = elem->flags;
                inode_ptr->key = elem->key();
                inode_ptr->value = elem->value();
            } else {
                auto elem = p->BranchPageElement(uint16_t(i));
                inode_ptr->pgid_ = elem->pgid_;
                inode_ptr->key = elem->key();
            }
            assert(!inode_ptr->key.empty());
            inodes.push_back(inode_ptr);
        }

        // Save first key so we can find the node in the parent when we spill.
        if (inodes.size() > 0) {
            key = inodes[0]->key;
            assert(!key.empty());
        } else {
            key = {};
        }
    }

    void node::write(boltdb::page *p) {
//        p->id = 0;
        p->overflow = 0;
        p->flags = 0;
        if (isLeaf) {
            p->flags |= leafPageFlag;
        } else {
            p->flags |= branchPageFlag;
        }
        if (inodes.size() > 0XFFFF) {
            assert(true);
        }
        p->count = uint16_t(inodes.size());
        if (p->count == 0) {
            return ;
        }
        char *buf = &(reinterpret_cast<char *>(&(p->ptr))[pageElementSize() * inodes.size()]);
        for (int i = 0; i < p->count; ++i) {
            if (isLeaf) {
                auto elem = p->LeafPageElement(uint16_t(i));
                elem->pos = uint32_t (reinterpret_cast<uint32_t *>(&(buf[0])) - reinterpret_cast<uint32_t *>(elem));
                elem->flags = inodes[i]->flags;
                elem->ksize = inodes[i]->key.size();
                elem->vsize = inodes[i]->value.size();
            } else {
                auto elem = p->BranchPageElement(uint16_t(i));
                elem->pos = uint32_t(reinterpret_cast<uint32_t *>(&buf[0]) - reinterpret_cast<uint32_t *>(elem));
                elem->ksize = inodes[i]->key.size();
                elem->pgid_ = inodes[i]->pgid_;
            }
            auto klen = inodes[i]->key.size();
            auto vlen = inodes[i]->value.size();
            //FIXME the length of key + value is larger than the max allocation size
            //
//            if (klen + vlen > )
            memcpy(buf, inodes[i]->key.data(), inodes[i]->key.size());
            buf += inodes[i]->key.size();
            memcpy(buf, inodes[i]->value.data(), inodes[i]->value.size());
            buf += inodes[i]->value.size();
        }

    }


    std::vector<node*> node::split(int pageSize) {
        std::vector<node*> result;
        node *n = this;
        while (true) {
            // Split node into two.
            auto pair_node = n->splitTwo(pageSize);
            result.push_back(pair_node.first);

            // If we can't split then exit the loop.
            if (pair_node.second == nullptr) {
                break;
            }

            // Set node to b so it gets split on the next iteration.
            n = pair_node.second;
        }

        return result;
    }

    node::~node() {
        for (auto n : inodes) {
            delete n;
        }
    }

    std::pair<node*, node*> node::splitTwo(int pageSize) {
        if (inodes.size() <= minKeysPerPage * 2 || sizeLessThan(pageSize)) {
            return std::make_pair(this, nullptr);
        }

        auto fillPercent = bucket->FillPercent;
        if (fillPercent < minFillPercent) {
            fillPercent = minFillPercent;
        } else if (fillPercent > maxFillPercent) {
            fillPercent = maxFillPercent;
        }
        auto threshold = int(pageSize * fillPercent);
        auto split_index = splitIndex(threshold);
        // create parent node, add self to parent children
        if (parent == nullptr) {
            parent = new node();
            parent->bucket = this->bucket;
            parent->children.push_back(this);

        }
        auto next = new node();
        next->bucket = bucket;
        next->isLeaf = isLeaf;
        next->parent = parent;
        parent->children.push_back(next);
        for (int idx = split_index.first, i = 0; idx < inodes.size(); ++idx) {
//            next->inodes[i] = inodes[idx];
                next->inodes.push_back(inodes[idx]);
        }
        inodes.resize(split_index.first);
        // Update the statistics.
        bucket->tx->stats.Split++;
        return std::make_pair(this, next);
    }

   std::pair<int, int> node::splitIndex(int threshold) {
        std::pair<int, int> result;
       auto &index = result.first;
       auto &sz = result.second;
       sz = pageHeaderSize;

       // Loop until we only have the minimum number of keys required for the second page.
       for (int i = 0; i < inodes.size() - minKeysPerPage; i++) {
           index = i;
           auto n = inodes[i];
           auto elsize = pageElementSize() + n->key.size() + n->value.size();

           // If we have at least the minimum number of keys and adding another
           // node would put us over the threshold then exit and return.
           if (i >= minKeysPerPage && sz+elsize > threshold) {
               break;
           }
           // Add the element size to the total size.
           sz += elsize;
       }

       return result;
    }

    bool node::sizeLessThan(int v) {
        auto sz = pageHeaderSize;
        auto elsz = pageElementSize();
        for (int i = 0; i < inodes.size(); i++) {
            auto &item = inodes[i];
            sz += elsz + item->key.size() + item->value.size();
            if (sz >= v) {
                return false;
            }
        }
        return true;
    }


    void node::put(boltdb_key_t &oldKey, boltdb_key_t &newKey, const boltdb_key_t &value,
                   boltdb::pgid id,
                   uint32_t flags) {
        if (id >= bucket->tx->meta_->pgcnt) {
//            panic(fmt.Sprintf("pgid (%d) above high water mark (%d)", pgid, n.bucket.tx.meta.pgid))
            assert(true);
        } else if (oldKey.size() <= 0) {
//            panic("put: zero-length old key")
            assert(true);
        } else if (newKey.size() <= 0) {
//            panic("put: zero-length new key")
            assert(true);
        }

        // Find insertion index.
        int index = Sort::Search(inodes, oldKey);
        // Add capacity and shift nodes if we don't have an exact match and need to insert.
        bool exact = inodes.size() > 0 && index < inodes.size() && (inodes[index]->key == oldKey);
        if (!exact) {
            inode *n = new inode(flags, id, newKey, value);
            assert(n->key.size() > 0);
            inodes.insert(inodes.begin() + index, n);
        } else {
            auto n = inodes[index];
            n->flags = flags;
            n->key = newKey;
            n->value = value;
            n->pgid_ = id;
            assert(n->key.size() > 0);
        }
    }

    Status node::spill() {

        auto tx = bucket->tx;
        if (spilled) {
            return Status::Ok();
        }

        sort(children.begin(), children.end());
        for (int i = 0; i < children.size(); ++i) {
            Status status = children[i]->spill();
            if (!status.ok()){
                return status;
            }
        }

        children.clear();

        auto ns = split(tx->db_->pageSize);
        for (int i= 0; i < ns.size(); ++i) {
            if (ns[i]->pgid_ > 0) {
                tx->db_->freelist_->free(tx->meta_->txid, tx->Page(pgid_));
                ns[i]->pgid_= 0;
            }
            auto page = tx->allocate(size()/tx->db_->pageSize + 1);
            if (!page.second.ok()){
                return page.second;
            }
            auto p = page.first;
            if (page.first->id >= tx->meta_->pgcnt) {
//                panic(fmt.Sprintf("pgid (%d) above high water mark (%d)", p.id, tx.meta.pgid))
                assert(true);
            }
            pgid_ = p->id;
            write(p);
            spilled = true;

            // Insert into parent inodes.
            if (parent != nullptr){
                auto k = key;
                if (k.empty()) {
                    k = inodes[0]->key;
                }
                parent->put(k, inodes[0]->key, nil, pgid_, 0);
                key = inodes[0]->key;
                assert(!key.empty());
            }
            tx->stats.Spill++;

        }
        if (parent != nullptr && parent->pgid_== 0 ){
            children.clear();
            return parent->spill();
        }
        return Status::Ok();
    }


    void node::rebalance() {
        if (!unbalanced) {
            return;
        }
        unbalanced = false;

        // Update statistics.
        bucket->tx->stats.Rebalance++;

        // Ignore if node is above threshold (25%) and has enough keys.
        auto threshold = bucket->tx->db_->pageSize / 4;
        if (size() > threshold && inodes.size() > minKeys() ){
            return;
        }

        // Root node has special handling.
        if (parent == nullptr) {
                    // If root node is a branch and only has one node then collapse it.
            if (isLeaf && inodes.size() == 1 ){
                // Move root's child up.
                auto child = bucket->Node(inodes[0]->pgid_, this);
                isLeaf = child->isLeaf;
                inodes = child->inodes;
                children = child->children;

                // Reparent all child nodes being moved.
                for ( auto &n : inodes) {
                    if (bucket->nodes.find(n->pgid_) != bucket->nodes.end()) {
                        child->parent = this;
                    }
//                            auto child = bucket->nodes[pgid_];
//                            if child, ok := n.bucket.nodes[inode.pgid]; ok {
//                                    child.parent = n
//                            }
                }

                // Remove old child.
                child->parent = nullptr;
                bucket->nodes.erase(child->pgid_);
            }

            return;
        }

        // If node has no keys then just remove it.
        if (numChildren() == 0 ) {
            parent->del(key);
            parent->removeChild(this);
            bucket->nodes.erase(pgid_);
            free();
            parent->rebalance();
            return;
        }

        assert(parent->numChildren() > 1);

        // Destination node is right sibling if idx == 0, otherwise left sibling.
        node * target;
        bool  useNextSibling = (parent->childIndex(this) == 0);
        if (useNextSibling) {
            target = nextSibling();
        } else {
            target = prevSibling();
        }

        // If both this node and the target node are too small then merge them.
        if (useNextSibling) {
                // Reparent all child nodes being moved.
            for (auto n : target->inodes ){
                if (bucket->nodes.find(n->pgid_) != bucket->nodes.end()) {
                    auto child = bucket->nodes[n->pgid_];
                    child->parent->removeChild(child);
                    child->parent = this;
                    child->parent->children.push_back(child);
                }
            }

            // Copy over inodes from target and remove target.
            for (auto i : target->inodes) {
                inodes.push_back(i);
            }
            parent->del(target->key);
            parent->removeChild(target);
            bucket->nodes.erase(target->pgid_);
            target->free();
        } else {
            // Reparent all child nodes being moved.
            for (auto n : target->inodes ){
                if (bucket->nodes.find(n->pgid_) != bucket->nodes.end()) {
                    auto child = bucket->nodes[n->pgid_];
                    child->parent->removeChild(child);
                    child->parent = target;
                    child->parent->children.push_back(child);
                }
            }

            // Copy over inodes to target and remove node.
            for (auto i : inodes) {
                target->inodes.push_back(i);
            }
            parent->del(key);
            parent->removeChild(this);
            bucket->nodes.erase(pgid_);
            free();
        }

        // Either this node or the target node was deleted from the parent so rebalance it.
        parent->rebalance();
    }



    void node::dereference() {
        if (!key.empty()) {
//            key_t k = key;
//            key = k;
//            assert(pgid_ == 0 || key.size() > 0);
        }

        for (int i = 0; i < inodes.size(); ++i) {
//            auto inode = inodes[i];

//            key := make([]byte, len(inode.key))
//            copy(key, inode.key)
//            inode.key = key;
//            _assert(len(inode.key) > 0, "dereference: zero-length inode key")

//            value := make([]byte, len(inode.value))
//            copy(value, inode.value)
//            inode.value = value
        }

        // Recursively dereference children.
        for (auto  child :children) {
            child->dereference();
        }

        // Update statistics.
        bucket->tx->stats.NodeDeref++;
    }

}