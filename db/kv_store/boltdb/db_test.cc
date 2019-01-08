//
// Created by ruoshui on 1/7/19.
//

#include <string>
#include <unistd.h>
#include <cassert>
#include <iostream>
#include "db.h"

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

boltdb::DB *MustOpen() {
    std::string temp_path = tempfile();
    boltdb::DB *db;
    auto status = boltdb::DB::open(temp_path, nullptr, &db);
    if (!status.ok()) {
        std::cerr << status.ToString() << std::endl;
    }
    return db;
}

// Close closes the database and deletes the underlying file.
func (db *DB) Close() error {
// Log statistics.
if *statsFlag {
db.PrintStats()
}

// Check database consistency after every test.
db.MustCheck()

// Close database and remove file.
defer os.Remove(db.Path())
return db.DB.Close()
}

// MustClose closes the database and deletes the underlying file. Panic on error.
func (db *DB) MustClose() {
    if err := db.Close(); err != nil {
            panic(err)
    }
}
