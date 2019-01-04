//
// Created by ruoshui on 12/20/18.
//


#pragma  once

#include <cstdint>
#include <string>
#include <vector>
#include "config.h"

namespace boltdb {
    using pgid = uint64_t ;
    struct meta;
    enum {
        branchPageFlag = 0x01,
        leafPageFlag = 0x02,
        metaPageFlag = 0x04,
        freelistPageFlag = 0x10
    };
    enum {
        bucketLeafFlag = 0x01
    };
    enum {
        minKeysPerPage = 2,
    };



    struct branchPageElement  {
        uint32_t pos;
        uint32_t ksize;
        pgid  pgid_;
        boltdb_key_t key();
    };
    struct leafPageElement {
        uint32_t flags;
        uint32_t pos;
        uint32_t ksize;
        uint32_t vsize;
        boltdb_key_t key();
        boltdb_key_t value();
    };
    struct page {
        std::string typ();
        meta *Meta();
        pgid id;
        uint16_t  flags;
        uint16_t  count;
        uint32_t  overflow;
        uint32_t  *ptr;

        branchPageElement*       BranchPageElement(uint16_t index) ;
        leafPageElement * LeafPageElement(uint16_t index);
        leafPageElement * LeafPageElements();
        branchPageElement* BranchPageElements();

    };
    const size_t  pageHeaderSize = (size_t) (&((page*)0)->ptr);
    const size_t  branchPageElementSize = sizeof(branchPageElement);
    const size_t  leafPageElementSize = sizeof(leafPageElement);
}


