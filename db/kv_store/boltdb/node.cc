//
// Created by ruoshui on 12/28/18.
//

#include <cassert>
#include <cstring>
#include "node.h"
#include "page.h"
#include "Tx.h"


namespace  boltdb {
    void node::del(std::vector<char> &key) {
        // Find index of key.
        auto it =  std::lower_bound(inodes.begin(), inodes.end(), key);

        // Exit if the key isn't found.
        if (it == inodes.end()  || (*it)->key != key) {
            return
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


    }

    int node::pageElementSize() {
        if (isLeaf) {
            return leafPageElementSize;
        }
        return branchPageElementSize;
    }

    node* node::childAt(int index) {
        if (isLeaf) {
            assert(true);
        }
        return bucket->no()
    }


    void node::read(boltdb::page *p) {
        pgid_ = p->id;
        isLeaf = ((p.flags & leafPageFlag) != 0);
//        n.inodes = make(inodes, int(p.count))
        inodes.resize(p->count);
        for (int i = 0; i < int(p->count); i++ ){
            auto inode = inodes[i];
            if (isLeaf) {
                auto elem = p->LeafPageElement(uint16_t(i));
                inode->flags = elem->flags;
                inode->key = elem->key();
                inode->value = elem.value();
            } else {
                auto elem = p->BranchPageElement(uint16_t(i));
                inode->pgid_ = elem->pgid_;
                inode->key = elem->key();
            }
            assert(!inode->key.empty());
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
        if (parent == nullptr) {
            parent = new node();
            parent->bucket = this->bucket;

        }
        auto next = new node();
        next->bucket = bucket;
        next->isLeaf = isLeaf;
        next->parent = parent;
        parent->children.push_back(next);
        for (int idx = split_index.first, i = 0; idx < inodes.size(); ++idx) {
            next->inodes[i] = inodes[idx];
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


    void node::put(boltdb::key_t &oldKey, boltdb::key_t &newKey, boltdb::key_t &value, boltdb::pgid id,
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
//        index := sort.Search(len(n.inodes), func(i int) bool { return bytes.Compare(n.inodes[i].key, oldKey) != -1 })
        auto it = std::lower_bound(inodes.begin(), inodes.end(), oldKey);
        int index = it - inodes.begin();
        // Add capacity and shift nodes if we don't have an exact match and need to insert.
        bool exact = inodes.size() > 0 && index < inodes.size() && (inodes[index]->key == oldKey);
        if (!exact) {
                    inodes.resize(inodes.size() + 1);
                    for (int idx = inodes.size() - 1; idx >= index; --idx) {
                        inodes[idx+1] = inodes[idx];
                    }
        }

        auto n = inodes[index];
        n->flags = flags;
        n->key = newKey;
        n->value = value;
        n->pgid_ = id;
        assert(n->key.size() > 0);
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
                node.pgid = 0
            }
        }
    }





}