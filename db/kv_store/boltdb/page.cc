//
// Created by ruoshui on 12/20/18.
//

#include "page.h"
#include "config.h"


namespace boltdb {
    meta * page::Meta() {
        return reinterpret_cast<meta*> (&ptr);
    }


    boltdb_key_t branchPageElement::key() {
        char *buf = reinterpret_cast<char*>(&(this->pos));
        buf = buf + pos;
        boltdb_key_t key(buf, ksize);
        return key;
    }

    boltdb_key_t leafPageElement::key() {
        char *buf = reinterpret_cast<char*>(this);
        buf = buf + pos;
        boltdb_key_t key(buf, ksize);
//        for (int i =0; i < ksize; ++i) {
//            key.push_back(buf[i]);
//        }
        return key;
    }

    boltdb_key_t leafPageElement::value() {
        char *buf = reinterpret_cast<char *>(this);
        buf = buf + pos + ksize;
        boltdb_key_t value(buf, vsize);
//        for (int i =0; i < vsize; ++i) {
//            value.push_back(buf[i]);
//        }
        return value;

    }
    branchPageElement* page::BranchPageElement(uint16_t index) {
        return &(reinterpret_cast<branchPageElement *>(&(this->ptr))[index]);
    }

    branchPageElement* page::BranchPageElements() {
        if (count == 0){
            return nullptr;
        }
        return reinterpret_cast<branchPageElement*>(&(this->ptr));
    }

    leafPageElement* page::LeafPageElement(uint16_t index) {
        return &(reinterpret_cast<leafPageElement*>(&(this->ptr))[index]);
    }

    leafPageElement* page::LeafPageElements() {
        if (count == 0) {
            return nullptr;
        }
        return reinterpret_cast<leafPageElement*>(&(this->ptr));
    }
}