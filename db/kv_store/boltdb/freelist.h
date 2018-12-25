//
// Created by ruoshui on 12/20/18.
//

#pragma once

#include <vector>
#include <map>
#include <cstdint>

namespace boltdb {
    using pgid = uint64_t ;
    using txid = uint64_t ;
    class page;
    class freelist {
    public:
        std::vector<pgid> ids;
        std::map<txid, std::vector<pgid>> pending;
        std::map<pgid, bool> cache;
        void read(page *p);
    };

}
