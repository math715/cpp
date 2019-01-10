//
// Created by ruoshui on 1/7/19.
//

#include <string>
#include <unistd.h>
#include <cassert>
#include <iostream>
#include <gtest/gtest.h>
#include "db.h"


namespace  boltdb {
    std::string tempfile() {
        std::string f = "bolt-" + std::to_string(rand()) + ".db";
        int ret = access(f.c_str(), F_OK);
        if (ret == 0 ) { // find path;
            if (remove(f.c_str()) != 0) {
                assert(true);
            }
        }
        return f;
    }

    boltdb::DB *MustOpenDB() {
        std::string temp_path = tempfile();
        boltdb::DB *db;
        auto status = boltdb::DB::open(temp_path, nullptr, &db);
        if (!status.ok()) {
            std::cerr << status.ToString() << std::endl;
        }
        return db;
    }

    TEST(DBTEST, Open) {

    }
}




