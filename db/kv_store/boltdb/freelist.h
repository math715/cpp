//
// Created by ruoshui on 12/20/18.
//

#pragma once

#include <vector>
#include <map>
#include <cstdint>
#include "config.h"

namespace boltdb {

    class page;
    class Status;
    class freelist {
    public:
        std::vector<pgid> ids_;
        std::map<txid, std::vector<pgid>> pending;
        std::map<pgid, bool> cache;
        void read(page *p);
        Status write(page *p);
        void reindex();
        int count();
        void copyall(std::vector<pgid> &dst);
        void free(txid tid, page *p);
        pgid allocate(int n);
        void rollback(txid tid);
        void release(txid tid);
        void reload(page *p);
        int size();
        int free_count();
        int pending_count();

    };

}
