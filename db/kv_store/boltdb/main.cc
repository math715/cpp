//
// Created by ruoshui on 1/2/19.
//



#include <iostream>
#include <string>
#include "db.h"
#include "Tx.h"

int main( ) {
    std::string path="test.db";
    boltdb::DB *db;
    boltdb::Status status = boltdb::DB::open(path, nullptr, &db);
    if (!status.ok()){
        std::cerr << status.ToString() << std::endl;
        return 1;
    }

    auto func = [](boltdb::Tx *tx)->boltdb::Status{
        // Create a bucket.
        std::string bucketname = "widgets";
        boltdb::Slice bucketkey (bucketname);
        auto bucket_status= tx->CreateBucket(bucketkey);
        if (!bucket_status.ok()) {
                    return bucket_status.status();
        }

        // Set the value "bar" for the key "foo".
        boltdb::Bucket *b = bucket_status.value();
        boltdb::Slice k("foo");
        boltdb::Slice v("bar");
        auto err = b->Put(k, v);
        if (!err.ok()) {
            return err;
        }
        return boltdb::Status::Ok();
    };
    auto viewfunc = [](boltdb::Tx *tx) ->boltdb::Status {
        auto value = tx->Bucket("widgets")->Get("foo");
        std::cerr << value << std::endl;
        return boltdb::Status::Ok();
    };
    status = db->Update(func);
    if (!status.ok()){
        std::cerr << status.ToString() << std::endl;
    }

    return 0;
}