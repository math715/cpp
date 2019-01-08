//
// Created by ruoshui on 1/4/19.
//

#pragma  once

#include <string>
#include <vector>
//#include "Slice.h"

namespace boltdb {
//    using boltdb_key_t = Slice;
    using boltdb_key_t = std::string;
//    using boltdb_value_t = Slice;
    using pgid = uint64_t ;
    using txid = uint64_t ;
//    const Slice nil = Slice();
    const boltdb_key_t  nil ;
    const bool IgnoreNoSync = false;



}