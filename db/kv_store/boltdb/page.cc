//
// Created by ruoshui on 12/20/18.
//

#include "page.h"


namespace boltdb {
    meta * page::Meta() {
        return reinterpret_cast<meta*> (ptr);
    }
}