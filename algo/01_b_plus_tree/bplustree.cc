//
// Created by ruoshui on 1/12/19.
//

#include "bplustree.h"
#include <algorithm>
#include <cassert>


namespace algo {
    int BplusNode::Insert(const algo::Key &key, const algo::Value &value) {
        auto it = std::lower_bound(records.begin(), records.end(), key, [](const BplusRecord &rec, const Key &key) {
            return rec.key < key;
        });
        if (it->key == key){
            it->value = value;
        } else {
            BplusRecord rec(key, value);
            records.insert(it, rec);
        }
        this->key = records[0].key;
        return records.size();
    }


    int BplusNode::Delete(const algo::Key &key) {
        auto it = std::lower_bound(records.begin(), records.end(), key, [](const BplusRecord &rec, const Key &key) {
            return rec.key < key;
        });
        if (it->key == key){
            records.erase(it);
            this->key = records[0].key;
        } else {
            return 0;
        }
    }

    BplusNode* BplusNode::LeftNode() {
        if (parent == nullptr) {
            return nullptr;
        }
        if (index > 0 ) {
            return parent->children[index-1];
        } else {
              auto p = parent->LeftNode();
              if (p == nullptr) {
                  return nullptr;
              } else {
                  assert(!p->children.empty());
                  return p->children[p->children.size() - 1];
              }
        }

    }

    BplusNode* BplusNode::RightNode() {
        if (parent == nullptr) {
            return nullptr;
        }

        if (index < parent->children.size() - 1) {

        } else {
            auto p = parent->RightNode();
            if (p == nullptr) {
                return nullptr;
            } else {
                assert(!p->children.empty());
                return p->children[0];
            }
        }
    }


    BplusTree::BplusTree(int key_size) {
        max_key_size = key_size;
        min_key_size = max_key_size / 2 + 1;
    }

    int BplusTree::Insert(const algo::Key &key, const algo::Value &value) {
        if (rootNode == nullptr) {
            rootNode = new BplusNode();
            assert(rootNode->Insert(key, value) == 1);
            return 0;
        }
        if (!Contain(key)) {
            return 1;
        } else {
            BplusNode *node  = search(key);
            assert(node != nullptr);
            if (node->Insert(key, value) >= max_key_size) {
                balance(node);
            }
        }

        return 0;

    }

    int BplusTree::Replace(const algo::Key &key, const algo::Value &value) {
        if (rootNode == nullptr) {
            rootNode = new BplusNode();
            assert(rootNode->Insert(key, value) == 1);
            return 0;
        }
        BplusNode *node  = search(key);
        assert(node != nullptr);
        if (node->Insert(key, value) >= max_key_size) {
            balance(node);
        }
    }

    int BplusTree::Delete(const algo::Key &key) {
        if (rootNode != nullptr) {
            BplusNode *node  = search(key);
            int ret = node->Delete(key);
            if ( ret < min_key_size && ret >= 0) {
                return balance(node);
            } else if (ret == 0) {
                return remove(node);
            } else if (ret == 1) {
                return 1;
            }
        } else {
            return 1;
        }

    }


    bool BplusTree::Contain(const algo::Key &key) {
        auto node = search(key);
        if (node == nullptr) {
            return false;
        }
        assert(node->isleaf);
        auto it = std::lower_bound(node->records.begin(), node->records.end(), key, [](const BplusRecord &rec, const Key &key){
            return rec.key < key ;
        });
        if (it->key == key) {
            return true;
        }
        return false;
    }

    BplusNode* BplusTree::search(const algo::Key &key) {
        BplusNode *root = rootNode;
        if (root != nullptr) {
            while (!root->isleaf) {
                auto it = std::lower_bound(root->children.begin(), root->children.end(), key,
                                           [](const BplusNode *n, const Key &key) {
                                               return n->key < key;
                                           });
                if ((*it)->key > key && it != root->children.begin()) {
                    it--;
                } else if ((*it)->key > key && it == root->children.begin()) {
                    return nullptr;
                }
                root = *it;
            }
        } else {
            return nullptr;
        }
    }

    int BplusTree::balance(algo::BplusNode *node) {

        if (node->size() > max_key_size) {


        } else if (node->size() < min_key_size) {
            auto right = node->RightNode();
            if (right == nullptr) {
                auto left = node ->LeftNode();
                if (left->size() + node->size() > max_key_size) {
                    merge(node, left);
                } else {
                    node = merge(node, left);
                    d
                }

            } else {

            }

        }
    }


    int BplusTree::remove(algo::BplusNode *node) {

    }


    BplusTree::Iterator::Iterator(algo::BplusTree *btree) :tree_(btree), node_(nullptr){

    }
    BplusNode* BplusTree::Iterator::First() {
        if(tree_->rootNode != nullptr) {
            BplusNode *node = tree_->rootNode;
            while (!node->isleaf) {
                assert(node->children[0]);
                node = node->children[0];
            }
            return node_ = node;
        } else {
            return nullptr;
        }
    }
    BplusNode* BplusTree::Iterator::Next() {
        if (node_ != nullptr) {
            if (node_->size() > node_idx - 1) {
                node_idx++;
            } else {
                node_ = node_->next;
                if (node_ != nullptr && node_->size() > 0) {
                    node_idx = 0;
                }
            }
        }
    }
    BplusNode* BplusTree::Iterator::Prev() {
        if (node_ != nullptr) {
            if (node_idx > 0) {
                node_idx--;
            } else {
                node_ = node_->LeftNode();
                if (node_ != nullptr) {
                    assert(node_->size() > 0);
                    node_idx = node_->size() - 1;
                }
            }
        }
        return node_;
    }
}