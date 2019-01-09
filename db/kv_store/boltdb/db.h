#pragma once
#include <cstdio>
#include <cstdint>
#include <string>
#include <fstream>
#include <vector>
#include <mutex>
#include <functional>
#include "error.h"
#include "port.h"
#include "bucket.h"

namespace boltdb {
    const uint64_t maxMmapStep = 1<<30;
    const uint32_t version = 2;
    const uint32_t magic = 0xED0CDAED;

// Default values if not set in a DB instance.

    const int DefaultMaxBatchSize = 1000;
//    const std::chrono::milliseconds DefaultMaxBatchDelay = 10;
    const int DefaultAllocSize = 16 * 1024 * 1024;
    const int defaultPageSize = 4 * 1024; // 4k
    using pgid = uint64_t;
    class page;
    struct meta {
        uint32_t magic;
        uint32_t version;
        uint32_t pageSize;
        uint32_t flags;
        bucket   root;
        pgid     freelist;
        pgid     pgcnt;
        pgid     txid;  
        uint64_t checksum;
        uint64_t sum64();
        Status Validate();
        void write(page *p);
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
    struct Tx;
    class freelist;
    struct TxStats;

    struct Stats {
        int FreePageN;
        int PendingPageN;
        int FreeAlloc;
        int FreeListInues;
        int TxN;
        int OpenTxN;
        TxStats *txStats;
    };

    class DB;
    class Status;
//    class batch {
//        DB *db;
//
//
//    };
    class File;
    class DB {
        
    public:
        PagePool *pagePool = nullptr;
        std::string path;
        bool   StrictMode;
        bool 	   NoSync;
        bool   NoGrowSync;
        int    MmapFlags;
        int    MaxBatchSize;
        std::chrono::milliseconds MaxBatchDelay;
        int    AllocSize;
        File   *file;
        char * dataref; // map
        char * data;
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
//        std::mutex batchMu;
//        batch      *batck;
//        std::mutex rwlock;
//        std::mutex metalock;
//        std::mutex mmaplock;
//        std::mutex statlock;
        Mutex rwlock;
        Mutex metalock;
        RWMutex mmaplock;
        RWMutex statlock;

        bool readOnly;

//        Status Mmap(int minsz) ;
        std::string Path( ) {
            return path;
        }
        std::string Gostring() {
            std::string result = "blot.DB{path:" + path + "}";
            return result;
        }
        static Status open( std::string path, Options *ops, DB **pDB) ;
        Status close();
        Status Close();
        Status Munmap();
        Status init();
        page * pageInBuffer(char *buf, pgid id);
        page * Page(pgid id);
        meta * Meta();
        std::pair<page *, Status> allocate(int count);
        Status grow( int sz);
        void removeTx(Tx *tx);
        Status Update(std::function<Status(Tx*)> fn);
        Status View(std::function<Status(Tx*)> fn);

        Status Mmap( int minsz);
        std::pair<int, Status> MmapSize(int size);

//    private:
        std::pair<Tx *, Status> Begin(bool writeable);
        std::pair<Tx *, Status> BeginTx() ;
        std::pair<Tx *, Status> BeginRWTx();

    };
}
