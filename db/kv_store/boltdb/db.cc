#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include "db.h"
#include "error.h"
#include "port.h"
#include "page.h"
#include "bucket.h"
#include "Tx.h"
#include "freelist.h"

#include <cstdio>
#include <cassert>

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
//        db->MaxBatchDelay = DefaultMaxBatchDelay;
        db->AllocSize = DefaultAllocSize;

        int flag  = O_RDWR;
        if (pops->ReadOnly) {
            flag = O_RDONLY;
            db->readOnly = true;

        }
//        int fd = ::open(path.c_str(), flag|O_CREAT, 0666);
        db->file = fopen(path.c_str(), "r+");
        int fd = fileno(db->file);
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
            auto m = db->pageInBuffer(buf, 0)->Meta();
            Status status = m->Validate();
            if (!status.ok()) {
                return status;
            }
            db->pageSize = 0x1000;
        }

//        mmaplock.lock();
        auto size = sb.st_size < pops->InitialMmapSize? pops->InitialMmapSize: sb.st_size;

        for (uint32_t i = 15; i <= 30; ++i) {
            if (size <= (1 << i)) {
                size = 1 << i;
                break;
            }
        }
        db->dataref = reinterpret_cast<char *>(mmap(NULL, size, PROT_WRITE, MAP_PRIVATE, fd, 0));
        if (db->dataref == MAP_FAILED) {
            Status status = Status::IOError("mmap");
            return status;
        }
        db->data = db->dataref;
        db->meta0 = db->Page(0)->Meta();
        db->meta1 = db->Page(1)->Meta();


        db->freelist_ = new freelist();
        db->freelist_->read(db->Page(db->Meta()->freelist));
        return Status::Ok();
    }

    page * DB::pageInBuffer(char *byte, boltdb::pgid id) {
        page *p =  reinterpret_cast<page *>(&byte[pageSize * id]);
        return p;
    }

//    page* DB::page(boltdb::pgid id) {
//        auto pos = id * pageSize;
//        return reinterpret_cast<page*>(&data[pos]);
//    }
//    meta * DB::meta() {
//        meta *metaA = meta0;
//        meta *metaB = meta1;
//        if (meta1->txid > meta0->txid) {
//            metaA = meta1;
//            metaB = meta0;
//        }
//        if (metaA->Validate() )
//            return metaA;
//        if ( metaB->Validate()) {
//            return metaB;
//        }
//        return nullptr;
//    }


    Status DB::init() {
        pageSize = SysCall::Getpagesize();
        char *buf = new char[4 * pageSize];

        for (int i = 0; i < 2; ++i) {
            page * p = pageInBuffer(buf, i);
            p->id = pgid(i);
            p->flags = metaPageFlag;
            meta *m = p->Meta();
            m->magic = magic;
            m->version = version;
            m->pageSize = uint32_t (pageSize);
            m->freelist = 2;
            m->root.root = 3;
            m->root.sequence = 0;
            m->pgcnt = 4;
            m->txid = txid(i);
            m->checksum = m->sum64();
        }

        page *p = pageInBuffer(buf, 2);
        p->id = pgid(2);
        p->flags = freelistPageFlag;
        p->count = 0;

        p = pageInBuffer(buf, 3);
        p->id = pgid(3);
        p->flags = leafPageFlag;
        p->count = 0;



        auto result = fwrite(buf, sizeof(char), 4 * pageSize, file);
        if (result != 4 * pageSize) {
            Status status = Status::IOError("write file ");
            return status;
        }
        int fd = fileno(file);
        if (fdatasync(fd) != 0) {
            Status status = Status::IOError("data sync file ");

            return status;
        }
        Status status = Status::Ok();
        return status;
    }


    page* DB::Page(boltdb::pgid id) {
        auto pos = id * pageSize;
        return reinterpret_cast<page *>(&data[pos]);
    }

    meta* DB::Meta() {
        auto metaA = meta0;
        auto metaB = meta1;

        if (meta1->txid > meta0->txid) {
            metaA = meta1;
            metaB = meta0;
        }
        if (metaA->Validate().ok()) {
            return metaA;
        }
        if (metaB->Validate().ok()) {
            return metaB;
        }
        assert(true);

    }


     Status meta::Validate() {
        if (magic != boltdb::magic) {
                    Status stat = Status::InvalidArgument("magic ");
                    return stat;
        } else if (version != boltdb::version) {
                    Status stat = Status::InvalidArgument("version ");
                    return stat;
         } else if (checksum != 0 && checksum != sum64()) {
                Status stat = Status::InvalidArgument("checksum ");
                return stat;
        }
         Status stat = Status::Ok();
         return stat;
    }

    uint64_t meta::sum64() {
        return FNV::FNV_64a(reinterpret_cast<char*>(this),sizeof (meta));
    }

}

