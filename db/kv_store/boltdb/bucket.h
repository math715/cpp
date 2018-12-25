//
// Created by ruoshui on 12/19/18.
//

#pragma once

#include <cstdint>
#include <string>
#include <map>

namespace  boltdb {
    class Tx;
    class node;
    class page;
    const int MaxKeySize = 32768;
    const int MaxValueSize = (1<<31) - 2;
    const uint32_t maxUint = 0xffffffff;
    const uint32_t minU int = 0;
    const int32_t  maxInt = INT32_MAX;
    const int32_t  minInt = INT32_MIN;

    using pgid = uint64_t ;
    struct bucket {
        pgid root;
        uint64_t  sequence;
    };
    const uint32_t bucketHeaderSize = sizeof(bucket);


    const double  minFillPercent = 0.1;
    const double  maxFillPercent = 1.0;

    const double DefaultFillPercent = 0.5;


    class Bucket {
    public:
        Bucket(Tx *tx);
    private:
        bucket *bucket;

        Tx *tx;

        std::map<std::string, Bucket*> buckets;
        page *page;
        node *rootNode;
        std::map<pgid, node *> nodes;
        double FillPercent;
    };


}


#endif //BOLTDB_BUCKET_H