cmake_minimum_required(VERSION 3.13)
project(algo)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_COMPILER g++-7)

include_directories(01_b_plus_tree)

#add_executable(algo_bplus_tree
#        01_b_plus_tree/bplusnode.cc
#        01_b_plus_tree/bplusnode.h
#        01_b_plus_tree/bplusnode_test.cc
#        01_b_plus_tree/bplustree.cc
#        01_b_plus_tree/bplustree.h
#        01_b_plus_tree/bplustree_test.cc
#        01_b_plus_tree/main.cpp)

add_executable(
        algo_a_star
        02_a_star_algo/

        02_a_star_algo/a_star.cc 02_a_star_algo/a_star.h 02_a_star_algo/main.cc)
