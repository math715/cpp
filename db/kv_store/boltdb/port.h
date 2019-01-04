#pragma once
#include <unistd.h>
#include <cstdint>
#include <cstdio>


#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <mutex>
#include <vector>
#include <cassert>
#include "config.h"


namespace boltdb {
    const uint32_t maxAllocSize = 0xFFFFFFF;
    const int maxMapSize = 0x7FFFFFFF; // 2GB

    class Status;
    class SysCall {
        public:
            static int Getpagesize() {
                return getpagesize();
            }
            static int fdatasync(int fd) {
                return fdatasync(fd);
            }

    };
    class FNV {
    public:
        static uint64_t FNV_64a(const char *data, size_t sz ) {
            uint64_t offset64        = 14695981039346656037ULL;
            uint64_t prime64         = 1099511628211ULL;
            uint64_t  hash = offset64;
            for (int i = 0; i < sz; ++i) {
                hash ^= uint64_t (data[i]);
                hash *= prime64;

            }
            return hash;
        }
    };
    class DB;
    class  File {

    public:
        File ():file(nullptr),path(nullptr),fd_(-1){}
        Status Open(const char *path, int flags, mode_t mode);
        Status Truncate(uint64_t sz);
        Status Sync();
        Status writeAt(const char *buf, int64_t sz, int64_t offset);
        Status Fdatasync();
        int64_t fileSize();
        size_t Write(const void *buf, size_t sz, size_t nmemb);
        size_t Read(void *ptr, size_t size, size_t nmemb);
        int fd() {
            assert(fd_ != -1);
            return fd_;
        }
        void Close();
        void Funlock();
        void Flock();
        static Status Mmap(boltdb::DB *db, int fd, int sz);

    private:
        FILE *file;
        int fd_;
        char *path;

    };
    struct page;
    class PagePool {
    public:
        explicit PagePool(int pz):pagesize(pz) {};
        ~PagePool();
        char * get();
        void put(char *p);

    private:
        int pagesize;
        std::mutex mtx;
        std::vector<char *> freelist_;
    };


    class Mutex {
    public:
        Mutex();
        ~Mutex();
        void Lock();
        void Unlock();

    private:
        pthread_mutex_t mtx;

    };
    class RWMutex {
    public:
        RWMutex();
        ~RWMutex();
        void RLock();
        void RUnlock();
        void Lock();
        void Unlock();

    private:
        pthread_rwlock_t rwlock;

    };

    struct inode;
    class Sort {
    public:
        static int Search(std::vector<inode *> &inodes, boltdb_key_t &key);
    };
    
} // blotdb
