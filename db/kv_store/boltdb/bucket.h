//
// Created by ruoshui on 12/19/18.
//

#pragma once

#include <cstdint>
#include <string>
#include <map>
#include <vector>
#include "error.h"
#include "config.h"

namespace  boltdb {
    struct Tx;
    struct node;
    struct page;
    const int MaxKeySize = 32768;
    const int MaxValueSize = 2147483646; // (1<<31) - 2
    const uint32_t maxUint = 0xffffffff;
    const uint32_t minUint = 0;
    const int32_t  maxInt = INT32_MAX;
    const int32_t  minInt = INT32_MIN;

    using pgid = uint64_t ;
    struct bucket {
        pgid root = 0;
        uint64_t  sequence = 0;
    };
    const uint32_t bucketHeaderSize = sizeof(bucket);


    const double  minFillPercent = 0.1;
    const double  maxFillPercent = 1.0;

    const double DefaultFillPercent = 0.5;

    struct cursor;
    struct Bucket : public bucket  {
    public:
        Bucket(){
            Bucket(nullptr);
        }
        Bucket(Tx *tx);
        Tx *tx;
//        bucket bucket_;

        std::map<boltdb_key_t , Bucket*> buckets;
        page *page_ = nullptr;
        node *rootNode;
        std::map<pgid, node *> nodes;
        double FillPercent;
        std::pair<page *, node *>  pageNode(pgid id);
        node *Node(pgid id, node *parent);
        void rebalance();
        Status spill();

        bool inlineable();
        int maxInlineBucketSize();
        void free();
        boltdb_key_t write();
        cursor *newCursor();
        void dereference();

        XXStatus<Bucket*> CreateBucket(boltdb_key_t name);
        Bucket *GetBucket(boltdb_key_t &key);
        Bucket *openBucket(boltdb_key_t &value);

        boltdb_key_t Get(boltdb_key_t &key);
        Status Put(boltdb_key_t &key, boltdb_key_t &value);
    };


}


