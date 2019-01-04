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
        elemRef(page *p = nullptr, node *n = nullptr, int idx = 0):page_(p), node_(n), index(idx){}
        page *page_;
        node *node_;
        int   index;
        bool isLeaf();
        int count();
    };
    struct cursor {
        Bucket *bucket;
        std::vector<elemRef> stack;
        std::pair<boltdb_key_t, boltdb_key_t> First();
        std::pair<boltdb_key_t, boltdb_key_t> Last();
        std::pair<boltdb_key_t, boltdb_key_t> Next();
        std::pair<boltdb_key_t, boltdb_key_t> Prev();
        std::pair<boltdb_key_t, boltdb_key_t> Seek(boltdb_key_t &key);
        void first();
        void last();

        void search(boltdb_key_t key, pgid id);
        void nsearch(boltdb_key_t key);
        void searchPage(boltdb_key_t &key, page *p);
        void searchNode(boltdb_key_t &key, node *n);

        std::tuple<boltdb_key_t, boltdb_key_t, uint32_t> next();
        std::tuple<boltdb_key_t, boltdb_key_t, uint32_t> seek(boltdb_key_t &key);
        std::tuple<boltdb_key_t, boltdb_key_t, uint32_t> keyValue();

        Status Delete();
        node *Node();
    };


}


