cmake_minimum_required(VERSION 3.10)
project(boltdb)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_COMPILER  g++-7)

include_directories(.)

add_executable(boltdb
        db.cc
        db.h
        error.cc
        error.h
        port.h
        bucket.cc
        bucket.h
        Tx.cc
        Tx.h
        page.cc
        page.h
        freelist.cc
        freelist.h
        cursor.cc
        cursor.h
        node.cc
        node.h
        port.cc
        main.cc
        config.h)

target_link_libraries(boltdb pthread)

