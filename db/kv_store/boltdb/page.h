//
// Created by ruoshui on 12/20/18.
//


#pragma  once

#include <cstdint>
#include <string>

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
    struct page {
        std::string typ();
        meta *Meta();
        pgid id;
        uint16_t  flags;
        uint16_t  count;
        uint32_t  overflow;
        uint32_t  *ptr;

        branchPageElement*       BranchPageElement(uint16_t index) ;

    };
}


