//
// Created by ruoshui on 1/7/19.
//

#include <gtest/gtest.h>
#include "db.h"
#include "Tx.h"
#include "cursor.h"

namespace boltdb {

    DB *MustOpenDB() ;
    TEST(TXTEST, CommitErrTxClose) {
        auto db = MustOpenDB();
//            defer db.MustClose()
        auto txstatus = db->Begin(true);
        auto tx = txstatus.first;
        EXPECT_TRUE(txstatus.second.ok());
        boltdb_key_t bucket_name = "foo";
        auto err = tx->CreateBucket(bucket_name);
        EXPECT_TRUE(err.ok());
        auto status = tx->Commit();
        EXPECT_TRUE(status.ok());
        status = tx->Commit();
        EXPECT_TRUE(!status.ok());
        db->MustClose();
    }

    TEST(TXTEST, RollbackErrTxClosed) {
            auto db = MustOpenDB();

            auto txstatus = db->Begin(true);
            auto tx = txstatus.first;
            auto err = tx->Rollback();
            EXPECT_TRUE(err.ok());
            err = tx->Rollback();
            EXPECT_TRUE(!err.ok());
            db->MustClose();
    }

    TEST(TXTEST, CommitErrTxNotWritable) {
            auto db = MustOpenDB();
            auto txstatus = db->Begin(false);
            EXPECT_TRUE(txstatus.second.ok());
            auto err = txstatus.first->Commit();
            EXPECT_TRUE(!err.ok());
            db->MustClose();
    }
    TEST(TXTEST, Cursor) {
            auto db = MustOpenDB();
            auto fn = [](Tx *tx)->Status {
                    boltdb_key_t bn("widgets");
                    auto bstatus = tx->CreateBucket(bn);
                    if (!bstatus.ok()) {
                            return bstatus.status();
                    }
                    bn = "woojits";
                    bstatus = tx->CreateBucket(bn);
                    if (!bstatus.ok()) {
                        return bstatus.status();
                    }
                    auto c = tx->newCursor();
                    auto kv = c->First();
                    assert(kv.first == "widgets");
                    assert(kv.second.empty());

                    kv = c->Next();
                    assert(kv.first == "woojits");
                    assert(kv.second.empty());
                    kv = c->Next();
                    assert(kv.first.empty());
                    assert(kv.second.empty());
                    return Status::Ok();
            };

            auto err = db->Update(fn);
            EXPECT_TRUE(err.ok());
            db->MustClose();
    }



}