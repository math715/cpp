//
// Created by ruoshui on 1/7/19.
//

#include <gtest/gtest.h>
#include "db.h"
#include "Tx.h"

namespace  boltdb {
    DB * MustOpenDB();
    // Ensure that a bucket that gets a non-existent key returns nil.
    TEST(BUCKETSET, GetNonExistent) {
        auto db =  MustOpenDB();
        auto fn = [](Tx *tx) -> Status {
            boltdb_key_t  bn("widgets");
            auto berr = tx->CreateBucket(bn);
            assert(berr.ok());
            boltdb_key_t k = "foo";
            auto v = berr.value()->Get(k);
            assert(v.empty());
            return Status::Ok();
        };
        auto err = db->Update(fn);
        EXPECT_TRUE(err.ok());
        db->MustClose();

    }
    TEST(BUCKETSET, Get_FromNode) {
            auto db =  MustOpenDB();
            auto fn = [](Tx *tx) -> Status {
                boltdb_key_t  bn("widgets");
                auto berr = tx->CreateBucket(bn);
                assert(berr.ok());
                auto b = berr.value();
                auto err = b->Put("foo", "bar");
                assert(err.ok());
                boltdb_key_t k = "foo";
                auto v = b->Get(k);
                assert(v == "bar");
                return Status::Ok();
            };
            auto err = db->Update(fn);
            EXPECT_TRUE(err.ok());
            db->MustClose();

    }

    // Ensure that a bucket retrieved via Get() returns a nil.

    TEST(BUCKETSET, GetIncompatibleValue) {
            auto db =  MustOpenDB();
            auto fn = [](Tx *tx) -> Status {
                boltdb_key_t  bn("widgets");
                auto berr = tx->CreateBucket(bn);
                berr = tx->GetBucket(bn)->CreateBucket("foo");
                boltdb_key_t  bn1 = "foo";
                auto v = tx->GetBucket(bn)->Get(bn1);
                assert(v.empty());
                return Status::Ok();
            };
            auto err = db->Update(fn);
            EXPECT_TRUE(err.ok());
            db->MustClose();
    }
}