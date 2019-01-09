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
//        ASSERT_TRUE(status.ok());

//        db->MustClose();
    }


}