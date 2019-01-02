//
// Created by ruoshui on 12/19/18.
//

#pragma once

#include <chrono>
#include <functional>
#include "bucket.h"

namespace boltdb {
    using txid = uint64_t ;
    struct TxStats {
        int PageCount;
        int PageAlloc;
        int CursorCount;
        int NodeCount;
        int NodeDeref;
        int Rebalance;
        std::chrono::milliseconds RebalanceTime;

        int Split;
        int Spill;
        std::chrono::milliseconds SpillTime;

        int Write;
        std::chrono::milliseconds WriteTime;


    };
    class DB;
    struct meta;
    class Status;
    struct Tx {
        bool writable;
        bool managed;
        DB   *db_;
        meta  *meta_;
        Bucket root;
        std::map<pgid, page*> pages;
        TxStats stats;
        int writeFlag;
        std::vector<std::function<void>> commitHandlers;
        void init(DB *db);
        page *Page(pgid id);
        std::pair<page *, Status> allocate(int count);
        Status write();
        Status writeMeta();
        Status Commit();
        void rollback();
        void close();
        Status Rollback()
        void forEachPage(pgid id, int depth, std::function<void(page *, int)> fn);



    };
}


