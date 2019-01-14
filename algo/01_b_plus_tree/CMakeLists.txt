cmake_minimum_required(VERSION 3.13)
project(01_b_plus_tree)

set(CMAKE_CXX_STANDARD 14)

add_executable(01_b_plus_tree main.cpp bplustree.cc bplustree.h bplusnode.cc bplusnode.h bplustree_test.cc bplusnode_test.cc)
target_link_libraries(01_b_plus_tree gtest pthread)