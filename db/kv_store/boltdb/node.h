//
// Created by ruoshui on 12/28/18.
//


#pragma once
#include <vector>
#include <cstdint>
#include "db.h"

namespace  boltdb {
    using key_t = std::vector<char>;
    struct Bucket;
    struct inode {
        uint32_t flags;
        pgid pgid_;
        std::vector<char> key;
        std::vector<char> value;
    };
    struct node {
        Bucket *bucket;
        bool isLeaf;
        bool      unbalanced;
        bool      spilled;
        std::vector<char>  key;
        pgid       pgid_;
        node       *parent;
        std::vector<node*>   children;
        std::vector<inode*>  inodes;
        void del(std::vector<char> &key);
        node* root();
        int minKeys();
        int size();

        int pageElementSize();
        node *childAt(int index);
        void read(page *p);

        void write(page *p);
        std::vector<node*> split(int pageSize);
        std::pair<node*, node*> splitTwo(int pageSize);
        bool sizeLessThan(int v);
        std::pair<int, int> splitIndex(int threshold);
        void put(key_t &oldKey, key_t &newKey, key_t &value, pgid id, uint32_t flags);
        int pageElementSize();

        Status spill();
    };
}


