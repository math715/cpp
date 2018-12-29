//
// Created by ruoshui on 12/20/18.
//


#pragma  once

#include <cstdint>
#include <string>
#include <vector>

namespace boltdb {
    using pgid = uint64_t ;
    struct meta;
    enum {
        branchPageFlag = 0x01,
        leafPageFlag = 0x02,
        metaPageFlag = 0x04,
        freelistPageFlag = 0x10
    };
    struct branchPageElement  {
        uint32_t pos;
        uint32_t ksize;
        pgid  pgid_;
    };
    struct leafPageElement {
        uint32_t pos;
        uint32_t ksize;
        uint32_t vsize;
        pgid  pgid_;
        std::vector<char> key();
        std::vector<char> value();
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

    };
}


