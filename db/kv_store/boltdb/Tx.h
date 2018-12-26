//
// Created by ruoshui on 12/19/18.
//

#pragma once

#include <chrono>

namespace boltdb {
    using txid = uint64_t ;
    class TxStats {
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
    class Tx {




    };
}


