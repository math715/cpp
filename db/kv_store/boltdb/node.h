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
        std::vector<char> key;
        std::vector<char> byte;
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

    };
}


