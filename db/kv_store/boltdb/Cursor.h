//
// Created by ruoshui on 12/28/18.
//
#pragma  once


#include <vector>

namespace boltdb {

    struct page;
    struct Bucket;
    struct node;
    struct elemRef {
        page *page_;
        node *node_;
        int   index;
        bool isLeaf();
        int count();
    };
    struct Cursor {
        Bucket *bucket;
        std::vector<elemRef> stack;
        std::pair<std::vector<char>, std::vector<char>> First();
        void first();
        std::tuple<std::vector<char>, std::vector<char>, uint32_t> next();
    };


}


