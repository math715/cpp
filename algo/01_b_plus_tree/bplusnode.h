//
// Created by ruoshui on 1/14/19.
//

#ifndef INC_01_B_PLUS_TREE_BPLUSNODE_H
#define INC_01_B_PLUS_TREE_BPLUSNODE_H

#include <vector>
#include <string>

namespace  algo {
    using Key = std::string;
    using Value = std::string;

    struct BplusRecord {
        BplusRecord(const Key &k, const Value &v):key(k), value(v){}
        Key key;
        Value value;
    };
    struct BplusNode {
        BplusNode() = default;
        ~BplusNode() ;
        bool isleaf;
        int level;
        BplusNode *parent;
        BplusNode *next;
        Key key;
        std::vector<BplusNode *> children; // branch node key
        std::vector<BplusRecord> records; // leaf node key value;
        int size(){
            return records.size();
        }

        // same parent
        BplusNode *LeftNode();
        // same parent big brother
        BplusNode *RightNode();
        // same grandfather
        BplusNode *nextSibling();
        // same grandfather
        BplusNode *prevSibling();


        int removeIndex(int index);


        BplusNode *SplitTwo(int min_key_size, int max_key_size);

        BplusNode * RemoveNode(BplusNode *node, int min_key_size, int max_key_size);
        BplusNode * MergeNode(BplusNode *node, int max_key_size);

        // merge right node ,
        int MergeRightNode(BplusNode *node);
        int MergeLeftNode(BplusNode *node);


        BplusNode * remove(algo::BplusNode *node, int min_key_size, int max_key_size);

        // return : Root Node
        BplusNode *RootNode();
        int InsertNode(BplusNode *node);
        // -1 : failed;
        int childIndex(BplusNode *node);
        int Insert(const Key &key, const Value &value);
        int Delete(const Key &key);

        void UpdateLevel(int value);
    };
}


#endif //INC_01_B_PLUS_TREE_BPLUSNODE_H
