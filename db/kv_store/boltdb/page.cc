//
// Created by ruoshui on 12/20/18.
//

#include "page.h"


namespace boltdb {
    meta * page::Meta() {
        return reinterpret_cast<meta*> (ptr);
    }


    std::vector<char> branchPageElement::key() {
        char *buf = reinterpret_cast<char*>(&(this->pos));
        buf = buf + pos;
        std::vector<char> key;
        for (int i =0; i < ksize; ++i) {
            key.push_back(buf[i]);
        }
        return key;
    }

    std::vector<char> leafPageElement::key() {
        char *buf = reinterpret_cast<char*>(&(this->pos));
        buf = buf + pos;
        std::vector<char> key;
        for (int i =0; i < ksize; ++i) {
            key.push_back(buf[i]);
        }
        return key;
    }

    std::vector<char> leafPageElement::value() {
        char *buf = reinterpret_cast<char *>(this);
        buf = buf + pos + ksize;
        std::vector<char> value;
        for (int i =0; i < vsize; ++i) {
            value.push_back(buf[i]);
        }
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