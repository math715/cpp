#include <iostream>
#include "bplustree.h"

int main() {
    algo::BplusTree btree(4);
    btree.Insert("001", "world");
    btree.Insert("002", "world");
    btree.Insert("003", "world");
    btree.Insert("004", "world");
    btree.Insert("005", "world");


    btree.PrintTree();
    return 0;
}