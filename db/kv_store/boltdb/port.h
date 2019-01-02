#pragma once
#include <unistd.h>
#include <cstdint>
#include <cstdio>


#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


namespace boltdb {
    const uint32_t maxAllocSize = 0xFFFFFFF;
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
    class  File {

    public:
        File ():file(nullptr),path(nullptr),fd(-1){}
        Status Open(const char *path, int flags, mode_t mode);
        Status Truncate(uint64_t sz);
        Status Sync();
        Status writeAt(const char *buf, int64_t sz, int64_t offset);
        Status Fdatasync();


    private:
        FILE *file;
        int fd;
        char *path;

    };


    
} // blotdb
