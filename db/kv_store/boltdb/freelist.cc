//
// Created by ruoshui on 12/20/18.
//

#include "freelist.h"
#include "page.h"


namespace boltdb {

    void freelist::read(boltdb::page *p) {
        int idx = 0;
        int count = p->count;
        if (count == 0xFFFF) {
            idx = 1;
            count = p->ptr
        }
    }

}