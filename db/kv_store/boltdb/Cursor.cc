//
// Created by ruoshui on 12/28/18.
//

#include <cassert>
#include "Cursor.h"
#include "bucket.h"

namespace boltdb {
    std::pair<std::vector<char>, std::vector<char>> Cursor::First() {
        assert(bucket->tx->db == nullptr);

    }
}