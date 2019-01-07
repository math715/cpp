//
// Created by ruoshui on 1/7/19.
//

#include "Slice.h"

namespace boltdb {
    Slice Slice::AlignedClone(boltdb::Slice &s) {
        s.cap_ = (s.size_ + 7 )>> 3 << 3;
        char *data = new char[s.cap_];
        memcpy(data, s.data(), s.size());
        delete [] s.data();
        s.data_ = data;
        return s;
    }

    std::ostream& operator<<(std::ostream &os, const Slice &s){
        os << s.toString() ;
        return os;
    }
}