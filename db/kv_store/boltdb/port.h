#pragma once
#include <unistd.h>
#include <cstdint>



namespace boltdb {
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
    
} // blotdb
