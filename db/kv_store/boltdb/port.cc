//
// Created by ruoshui on 1/2/19.
//

#include "port.h"
#include "error.h"
#include "db.h"
#include "node.h"
#include <unistd.h>
#include <cassert>
#include <sys/mman.h>
#include <sys/file.h>
#include <algorithm>

namespace boltdb {
    Status File::Open(const char *path, int flags, mode_t mode) {
        fd_ = open(path, flags, mode);
        if (fd_ == -1) {
            return Status::IOError(path);
        }
        file = fdopen(fd_, "w+");
        if (file == nullptr) {
            return Status::IOError(path);
        }
        return Status::Ok();
    }
    Status File::Truncate(uint64_t sz) {
        int ret = ftruncate(fd_, sz);
        if (ret == 0) {
            return Status::Ok();
        } else {
            return Status::IOError("File Truncate");
        }
    }
    Status File::Sync() {
        int ret = fdatasync(fd_);
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
        auto ret = fdatasync(fd_);
        if (ret == -1) {
            Status::IOError("Fdatasync");
        }
        return Status::Ok();
    }
    int64_t File::fileSize() {
        struct stat sb;
        if (fstat(fd_, &sb) == -1) {
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

    Status File::Mmap(boltdb::DB *db, int fd, int sz) {
        // Map the data file to memory.
//        mmap(NULL, size, PROT_WRITE, MAP_PRIVATE, fd, 0)
        char * ptr = reinterpret_cast<char *>(mmap( nullptr, sz, PROT_READ, MAP_SHARED|db->MmapFlags, fd, 0));
        if (ptr == MAP_FAILED) {
                    return Status::IOError("mmmap ");
        }

        // Advise the kernel that the mmap is accessed randomly.
        int ret = madvise(ptr, sz, MADV_RANDOM);
        if (ret != 0){
                return Status::IOError("madvise: %s");
        }

        // Save the original byte slice and convert to a byte array pointer.
        db->dataref = ptr;
        db->data = reinterpret_cast<char(*)[maxMapSize]>(&ptr[0]);
        db->datasz = sz;
        return Status::Ok();
    }

    void File::Close() {
        fclose(file);

    }

    void File::Flock() {
        flock(fd_, LOCK_EX);
    }
    void File::Funlock() {
        flock(fd_, LOCK_UN);
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

    Mutex::Mutex() {
        pthread_mutex_init(&mtx, nullptr);
    }
    Mutex::~Mutex() {
        pthread_mutex_destroy(&mtx);
    }
    void Mutex::Lock() {
        pthread_mutex_lock(&mtx);
    }
    void Mutex::Unlock() {
        pthread_mutex_unlock(&mtx);

    }

    RWMutex::RWMutex() {
        pthread_rwlock_init(&rwlock, nullptr);
    }
    RWMutex::~RWMutex() {
        pthread_rwlock_destroy(&rwlock);
    }

    void RWMutex::RLock() {
        pthread_rwlock_rdlock(&rwlock);
    }
    void RWMutex::RUnlock() {
        pthread_rwlock_unlock(&rwlock);
    }
    void RWMutex::Unlock() {
        pthread_rwlock_unlock(&rwlock);
    }

    void RWMutex::Lock() {
        pthread_rwlock_wrlock(&rwlock);
    }

    int Sort::Search(std::vector<boltdb::inode *> &inodes, boltdb_key_t &key) {
        auto it = std::lower_bound(inodes.begin(), inodes.end(), key, [](inode *n, const boltdb_key_t &key)->bool {
            return n->key.compare(key) < 0;
        });
        int index = it - inodes.begin();
        return index;
    }
}