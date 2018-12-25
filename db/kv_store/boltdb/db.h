#pragma once
#include <cstdio>
#include <cstdint>
#include <string>
#include <fstream>
#include <vector>
#include <mutex>

namespace boltdb {
    const uint64_t maxMmapStep = 1<<30;
    const uint32_t version = 2;
    const uint32_t magic = 0xED0CDAED;

// Default values if not set in a DB instance.

    const int DefaultMaxBatchSize = 1000;
    const std::chrono::milliseconds DefaultMaxBatchDelay = 10;
    const int DefaultAllocSize = 16 * 1024 * 1024;
    const int defaultPageSize = 4 * 1024; // 4k
    using pgid = uint64_t;
    class bucket;
    class page;
    class meta {
        private:
        uint32_t magic;
        uint32_t version;
        uint32_t pageSize;
        uint32_t flags;
        bucket   root;
        pgid     freelist;
        pgid     pgid;
        pgid     txid;  
        uint64_t checksum;   
    };


    struct Options {
        Options():timeout(0),NoGrowSync(false){}
        std::chrono::milliseconds timeout;
        bool NoGrowSync;
	    bool ReadOnly;
        bool MmapFlags;
        int InitialMmapSize;   
    };

    const Options DefautlOptions;
    class Tx;
    class freelist;
    class TxStats;

    class Stats {
        int FreePageN;
        int PendingPageN;
        int FreeAlloc;
        int FreeListInues;
        int TxN;
        int OpenTxN;
        TxStats TxStats;
    };

    class DB;
    class Status;
    class batch {
        DB *db;


    };
    class DB {
        
    public:
        std::string path;
        bool   StrictMode;
        bool 	   NoSync;
        bool   NoGrowSync;
        int    MmapFlags;
        int    MaxBatchSize;
        std::chrono::milliseconds MaxBatchDelay;
        int    AllocSize;
        std::fstream file;
        char * dataref;
        char data[maxMmapStep];
        int  datasz;
        int  filesz;
        meta * meta0;
        meta * meta1;
        int pageSize;
        bool opened;
        Tx *   rwtx;
        std::vector<Tx *> txs;
        freelist *freelist_;
        Stats stats;
//        sync.Pool pagePool;
        std::mutex batchMu;
        batch      *batck;
        std::mutex rwlock;
        std::mutex metalock;
        std::mutex mmaplock;
        std::mutex statlock;
        bool readOnly;

        page *pageInBuffer(char *byte, pgid id);
        Status mmap(int minsz) ;
        page *page(pgid id);
        meta *meta() ;
    std::string Path( ) {
        return path;
    }
    std::string Gostring() {
        std::string result = "blot.DB{path:" + path + "}";
        return result;
    }
    static Status open( std::string path, Options *ops, DB **pDB) ;
	private:


    };
}
