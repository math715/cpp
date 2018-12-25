//
// Created by ruoshui on 12/20/18.
//


#pragma  once;

#include <cstdint>
#include <string>

namespace boltdb {
    using pgid = uint64_t ;
    class meta;
    class page {
    public:
        std::string typ();
        meta *meta() {
            return reinterpret_cast<meta*>(ptr);
        }
        pgid id;
        uint16_t  flags;
        uint16_t  count;
        uint32_t  overflow;
        uint32_t  *ptr;

    };
}


