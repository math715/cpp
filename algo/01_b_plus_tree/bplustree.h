//
// Created by ruoshui on 1/12/19.
//

#ifndef INC_01_B_PLUS_TREE_BPLUSTREE_H
#define INC_01_B_PLUS_TREE_BPLUSTREE_H

#include <string>
#include <vector>

namespace algo {
    using Key = std::string;
    using Value = std::string;

    struct BplusRecord {
        BplusRecord(const Key &k, const Value &v):key(k), value(v){}
        Key key;
        Value value;
    };
    struct BplusNode {
        BplusNode() = default;
        bool isleaf;
        int index; // parent idx child;
        int level;
        BplusNode *parent;
        BplusNode *next;
        Key key;
        std::vector<BplusNode *> children; // branch node key
        std::vector<BplusRecord> records; // leaf node key value;
        int size(){
            return records.size();
        }
        BplusNode *LeftNode();
        BplusNode *RightNode();
        int Insert(const Key &key, const Value &value);
        int Delete(const Key &key);
    };

    class BplusTree {
    public:
        BplusTree (int key_size);

        // 0 : success
        // 1 : failed if key is exist
        int  Insert(const Key &key, const Value &value);

        // whether the key is exist or not exist, insert new value;
        int Replace(const Key &key, const Value &value);
        // delete key
        // 0: success
        // 1: failed not found key;
        // 2: failed fount key;
        int  Delete(const Key &key);

        // search key
        // 0 : success
        // 1 : failed
        int Search( const Key &key, Value *value);


        //
        bool Contain(const Key &key);

        class Iterator {
        public:
            explicit Iterator(BplusTree *btree);
            BplusNode * First();
            BplusNode * Last();
            BplusNode * Prev();
            BplusNode * Next();
            //find the records less or equal key;
            void Seek(const Key &key);


        private:
            BplusTree *tree_;
            BplusNode *node_;
            int node_idx;

        };
        void PrintTree();
    private:

        BplusNode *search(const Key &key);
        int balance(BplusNode *node);
        int remove(BplusNode *node);
        int total_records_;
        int level_;
        int max_key_size;
        int min_key_size;

        BplusNode *rootNode= nullptr;



    };
}


#endif //INC_01_B_PLUS_TREE_BPLUSTREE_H
