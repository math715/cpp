#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include "db.h"
#include "error.h"

namespace boltdb {
    Status DB::open(std::string path, boltdb::Options *ops, boltdb::DB **pDB) {
        *pDB = nullptr;
        DB *db= new DB();
        (*pDB)->opened = true;
        const Options *pops = ops;
        if (ops == nullptr) {
            pops = &DefautlOptions;
        }
        db->NoGrowSync = pops->NoGrowSync;
        db->MmapFlags = pops->MmapFlags;

        db->MaxBatchSize = DefaultMaxBatchSize;
        db->MaxBatchDelay = DefaultMaxBatchDelay;
        db->AllocSize = DefaultAllocSize;

        int flag  = O_RDWR;
        if (pops->ReadOnly) {
            flag = O_RDONLY;
            db->readOnly = true;

        }
        int fd = open(path.c_str(), flag|O_CREAT, 0666);
        if (fd == -1) {
            Status status = Status::NotFound(path) ;
            return status;
        }
        struct stat sb;
        if (fstat(fd, &sb) == -1) {
            Status status = Status::NotFound(path);
            return status;
        }
        if (sb.st_size == 0) {
            Status status = db->init();
            if (!status.ok()){
                return status;
            }
        } else {
            char *buf = new char[0x1000];
            size_t sz = read(fd, buf, 0x1000);
            if (sz == -1) {
                Status status = Status::IOError(path);
                return status;
            }
            auto m = db->pageInBuffer(buf, 0)->meta();
            Status status = m.Validate();
            if (!status.ok()) {
                return status;
            }
            db->pageSize = 0x1000;
        }

        mmaplock.lock();
        auto size = sb.st_size < pops->InitialMmapSize? pops->InitialMmapSize: sb.st_size;

        for (uint32_t i = 15; i <= 30; ++i) {
            if (size <= (1 << i)) {
                size = 1 << i;
                break;
            }
        }
        db->dataref = mmap(NULL, size, PROT_WRITE, MAP_PRIVATE, fd, 0);
        if (db->dataref == MAP_FAILED) {
            Status status = Status::IOError("mmap");
            return status;
        }
        db->meta0 = db->page(0)->meta();
        db->meta1 = db->page(1)->meta();


        db->freelist_ = new freelist;
        db->freelist_->read()
        return ;
    }

    page * DB::pageInBuffer(char *byte, boltdb::pgid id) {
        return reinterpret_cast<page *>(&byte[pageSize * id]);
    }

    page* DB::page(boltdb::pgid id) {
        auto pos = id * pageSize;
        return reinterpret_cast<page*>(&data[pos]);
    }
    meta * DB::meta() {
        meta *metaA = meta0;
        meta *metaB = meta1;
        if (meta1->txid > meta0->txid) {
            metaA = meta1;
            metaB = meta0;
        }
        if (metaA->Validate() )
            return metaA;
        if ( metaB->Validate()) {
            return metaB;
        }
        return nullptr;
    }

}

