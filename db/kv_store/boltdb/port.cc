//
// Created by ruoshui on 1/2/19.
//

#include "port.h"
#include "error.h"
#include <unistd.h>
#include <cassert>

namespace boltdb {
    Status File::Open(const char *path, int flags, mode_t mode) {
        int fd = open(path, flags, mode);
        if (fd == -1) {
            return Status::IOError(path);
        }
        file = fdopen(fd, "w+");
        if (file == nullptr) {
            return Status::IOError(path);
        }
        return Status::Ok();
    }
    Status File::Truncate(uint64_t sz) {
        int ret = ftruncate(fd, sz);
        if (ret == 0) {
            return Status::Ok();
        } else {
            return Status::IOError("File Truncate");
        }
    }
    Status File::Sync() {
        int ret = fdatasync(fd);
        if (ret == 0) {
            return Status::Ok();
        } else {
            return Status::IOError("File Sync");
        }
    }

    Status File::writeAt(const char *buf, int64_t sz, int64_t offset) {
        int ret = fseek(file, offset, SEEK_SET);
        if (ret == -1) {
            return Status::IOError("write at");
        }
        size_t size = fwrite(buf, sizeof(char), sz, file);
        assert(size == sz);

    }

    Status File::Fdatasync() {
        auto ret = fdatasync(fd);
        if (ret == -1) {
            Status::IOError("Fdatasync");
        }
        return Status::Ok();
    }
    int64_t File::fileSize() {
        struct stat sb;
        if (fstat(fd, &sb) == -1) {
            assert(true);
//            Status status = Status::NotFound(path);
//            return status;
        }
        return sb.st_size;
    }

    size_t File::Write(const void *buf, size_t sz, size_t nmemb) {
        return fwrite(buf, sz, nmemb, file);
    }

    size_t File::Read(void *ptr, size_t size, size_t nmemb) {
        return fread(ptr, size, nmemb, file);
    }


    char* PagePool::get() {
        std::unique_lock<std::mutex> lock;
        if (freelist_.empty()) {
            freelist_.push_back( new char[pagesize]);
        }
        auto p = freelist_.back();
        freelist_.pop_back();
        return p;

    }
    void PagePool::put(char *p) {
        std::unique_lock<std::mutex> lock;
        freelist_.push_back(p);
    }

    PagePool::~PagePool() {
        for (auto f : freelist_) {
            delete [] f;
        }
    }
}