//
// Created by ruoshui on 1/7/19.
//

#include <gtest/gtest.h>
#include "db.h"
#include "cursor.h"
#include "Tx.h"
#include "bucket.h"

namespace boltdb {
    DB *MustOpenDB();
    TEST(CursorTest, Bucket) {
        DB *db = MustOpenDB();
        auto fn = [](Tx *tx)->Status{
            boltdb_key_t bucket_name("widgets");
            XXStatus<Bucket*> bucketstatus= tx->CreateBucket(bucket_name);
            assert(bucketstatus.ok());
            Bucket *cb = bucketstatus.value()->newCursor()->bucket;
            assert(cb == bucketstatus.value());
            return Status::Ok();
        };
        auto status = db->Update(fn);
        ASSERT_TRUE(status.ok());
//        db->Close();
        db->MustClose();
    }
    TEST(CursorTest, Seek) {
            DB *db = MustOpenDB();
            auto fn = [](Tx *tx) -> Status {
                boltdb_key_t bucket_name("widgets");
                XXStatus<Bucket *> bucketstatus = tx->CreateBucket(bucket_name);
                assert(bucketstatus.ok());
                auto b = bucketstatus.value();
                Status err = b->Put("foo", "0001");
                assert(err.ok());
                err = b->Put("bar", "0002");
                assert(err.ok());
                err = b->Put("baz", "0003");
                assert(err.ok());
                auto berr = b->CreateBucket("bkt");
                assert(berr.ok());

//                Bucket *cb = bucketstatus.value()->newCursor()->bucket;
//                assert(cb == bucketstatus.value());
                return Status::Ok();
            };
            auto status = db->Update(fn);
            ASSERT_TRUE(status.ok());

            auto viewfn = [&](Tx *tx) -> Status {
                auto b= tx->GetBucket("widgets");
                auto c = b->newCursor();

                // Exact match should go to the key.
                auto kv = c->Seek("bar");
                assert(kv.first == "bar");
                assert(kv.second == "0002");
                kv = c->Seek("bas");
                assert(kv.first == "baz");
                assert(kv.second == "0003");


                // Low key should go to the first key.
                kv = c->Seek("");
                assert(kv.first == "bar");
                assert(kv.second == "0002");
                // High key should return no key.
                kv = c->Seek("zzz");
                assert(kv.first == "");
                assert(kv.second == "");

                // Buckets should return their key but no value.
                kv = c->Seek("bkt");
                assert(kv.first == "bkt");
                assert(kv.second.empty());
                return Status::Ok();
            };
            status = db->View(viewfn);
            ASSERT_TRUE(status.ok());

            db->MustClose();
    }



    TEST(CursorTest, Delete) {

            DB *db = MustOpenDB();
            const int count = 1000;

            auto fn = [](Tx *tx) -> Status {
                boltdb_key_t bucket_name("widgets");
                XXStatus<Bucket *> bucketstatus = tx->CreateBucket(bucket_name);
                assert(bucketstatus.ok());
                auto b = bucketstatus.value();

                for (int i = 0; i < count; ++i ) {
                        uint64_t  k = i;
                        char *k_ptr = reinterpret_cast<char *>(&k);
                        boltdb_key_t key(k_ptr, sizeof(k));
                        boltdb_key_t value(100, '0');
                        auto err = b->Put(key, value);
                        assert(err.ok());
                }
                auto berr = b->CreateBucket("sub");
                assert(berr.ok());
//                Bucket *cb = bucketstatus.value()->newCursor()->bucket;
//                assert(cb == bucketstatus.value());
                return Status::Ok();
            };
            auto status = db->Update(fn);
            ASSERT_TRUE(status.ok());

            auto fn1 = [](Tx *tx) -> Status {
                auto c = tx->GetBucket("widgets")->newCursor();
                uint64_t bound = count / 2;
                for (auto kv = c->First();
                     (reinterpret_cast<uint64_t *>(const_cast<char *>(kv.first.data())))[0] < bound; kv = c->Next()) {
                        auto err = c->Delete();
                        assert(err.ok());
                }
                c->Seek("sub");
                auto err = c->Delete();
                assert(!err.ok());

                return Status::Ok();
            };
            status = db->Update(fn1);
            ASSERT_TRUE(status.ok());
            auto viewfn = [&](Tx *tx) -> Status {
                auto stats = tx->GetBucket("widgets")->Stats();
//                assert(stats.KeyN != count/2+1);
                return Status::Ok();
            };
            status = db->View(viewfn);
            ASSERT_TRUE(status.ok());

            db->MustClose();

    }
}