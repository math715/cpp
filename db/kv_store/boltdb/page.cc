//
// Created by ruoshui on 12/20/18.
//

#include "page.h"


namespace boltdb {
    meta * page::Meta() {
        return reinterpret_cast<meta*> (ptr);
    }

    std::vector<char> leafPageElement::key() {
        char *buf = reinterpret_cast<char*>(this);
        &buf[pos]
        return (*[maxAllocSize]byte)(unsafe.Pointer(&buf[n.pos]))[:n.ksize:n.ksize]
    }

    std::vector<char> leafPageElement::value() {
        buf := (*[maxAllocSize]byte)(unsafe.Pointer(n))
        return (*[maxAllocSize]byte)(unsafe.Pointer(&buf[n.pos]))[:n.ksize:n.ksize]

    }
    branchPageElement* page::BranchPageElement(uint16_t index) {
        return &(reinterpret_cast<branchPageElement *>(&(this->ptr))[index]);
    }

    leafPageElement* page::LeafPageElement(uint16_t index) {
        return &(reinterpret_cast<leafPageElement*>(&(this->ptr))[index]);
    }
}