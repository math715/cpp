//
// Created by ruoshui on 1/2/19.
//



#include <iostream>
#include <string>
#include <cstring>
#include <gtest/gtest.h>
#include "db.h"
#include "Tx.h"


int tmain(int argc, char **argv){
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
int main(int argc, char *argv[] ) {
    if (argc > 0) {
        if (strcasecmp(argv[1], "test") == 0){
            return tmain(argc - 1, argv + 1);
        }
    }

    std::string path="test.db";
    boltdb::DB *db;
    boltdb::Status status = boltdb::DB::open(path, nullptr, &db);
    if (!status.ok()){
        std::cerr << status.ToString() << std::endl;
        return 1;
    }

    auto func = [](boltdb::Tx *tx)->boltdb::Status{
        // Create a bucket.
        boltdb::boltdb_key_t bucketkey ("widgets");
        auto bucket_status= tx->CreateBucket(bucketkey);
        if (!bucket_status.ok()) {
            return bucket_status.status();
        }

        // Set the value "bar" for the key "foo".
        boltdb::Bucket *b = bucket_status.value();
        boltdb::boltdb_key_t k("foo");
        boltdb::boltdb_key_t v("bar");
        auto err = b->Put(k, v);
        if (!err.ok()) {
            return err;
        }
        return boltdb::Status::Ok();
    };
    auto viewfunc = [](boltdb::Tx *tx) ->boltdb::Status {
        boltdb::boltdb_key_t bucket_name("widgets");
        boltdb::boltdb_key_t key("foo");
        auto value = tx->GetBucket(bucket_name)->Get(key);
        std::cerr << value << std::endl;
        return boltdb::Status::Ok();
    };
    status = db->Update(func);
    if (!status.ok()){
        std::cerr << status.ToString() << std::endl;
    }

    status = db->View(viewfunc);

    status = db->Close();

    if (!status.ok()){
        std::cout << status.ToString() << std::endl;
    }



    return 0;
}