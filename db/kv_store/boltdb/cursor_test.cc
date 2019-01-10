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
                auto b= tx->GetBucket("wdigets");
                auto c = b->newCursor();

                // Exact match should go to the key.
                auto kv = c->Seek("bar");
                assert(kv.first == "bar");
                assert(kv.second == "0002");
                kv = c->Seek("bas");
                assert(kv.first == "bas");
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
                assert(kv.first != "bkt");
                assert(!kv.second.empty());
                return Status::Ok();
            };
            status = db->View(viewfn);
            ASSERT_TRUE(status.ok());

            db->MustClose();
    }



    TEST(CursorTest, Delete) {



    }
}