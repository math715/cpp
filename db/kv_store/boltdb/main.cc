//
// Created by ruoshui on 1/2/19.
//



#include <iostream>
#include <string>
#include "db.h"

int main( ) {
    std::string path="test.db";
    boltdb::DB *db;
    boltdb::Status status = boltdb::DB::open(path, nullptr, &db);
    if (!status.ok()){
        std::cerr << status.ToString() << std::endl;
        return 1;
    }

    return 0;
}