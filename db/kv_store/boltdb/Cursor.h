//
// Created by ruoshui on 12/28/18.
//
#pragma  once


#include <vector>
#include <cstdint>
#include "db.h"

namespace boltdb {

    struct page;
    struct Bucket;
    struct node;
    struct elemRef {
        elemRef(page *p, node *n, int idx = 0):page_(p), node_(n), index(idx){}
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
        std::pair<std::vector<char>, std::vector<char>> Last();
        std::pair<std::vector<char>, std::vector<char>> Next();
        std::pair<std::vector<char>, std::vector<char>> Prev();
        std::pair<std::vector<char>, std::vector<char>> Seek(std::vector<char> key);
        void first();
        void last();

        void search(std::vector<char> key, pgid id);
        void nsearch(std::vector<char> key);
        void searchPage(std::vector<char> key, page *p);
        void searchNode(std::vector<char> key, node *n);

        std::tuple<std::vector<char>, std::vector<char>, uint32_t> next();
        std::tuple<std::vector<char>, std::vector<char>, uint32_t> seek(std::vector<char> key);


        std::tuple<std::vector<char>, std::vector<char>, uint32_t> keyValue();

        Status Delete();
    };


}


