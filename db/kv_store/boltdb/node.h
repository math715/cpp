//
// Created by ruoshui on 12/28/18.
//


#pragma once
#include <vector>
#include <cstdint>
#include "db.h"

namespace  boltdb {
    struct Bucket;
    struct inode {
        uint32_t flags;
        pgid pgid_;
        boltdb_key_t key;
        boltdb_key_t value;
    };
    struct node {
        Bucket *bucket;
        bool isLeaf;
        bool      unbalanced;
        bool      spilled;
        boltdb_key_t key;
        pgid       pgid_;
        node       *parent;
        std::vector<node*>   children;
        std::vector<inode*>  inodes;
        void del(boltdb_key_t &key);
        node* root();
        int minKeys();
        int size();

        int pageElementSize();
        node *childAt(int index);
        int childIndex(node *child );
        void free();
        int numChildren(){
            return inodes.size();
        }
        void removeChild(node * target);

        node *nextSibling();
        node *prevSibling();
        void read(page *p);

        void write(page *p);
        std::vector<node*> split(int pageSize);
        std::pair<node*, node*> splitTwo(int pageSize);
        std::pair<int, int> splitIndex(int threshold);
        bool sizeLessThan(int v);
        void put(boltdb_key_t &oldKey, boltdb_key_t &newKey, const boltdb_key_t &value, pgid id, uint32_t flags);

        Status spill();
        void rebalance();
        void dereference();
    };
}


