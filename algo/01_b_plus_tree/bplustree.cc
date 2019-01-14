//
// Created by ruoshui on 1/12/19.
//

#include "bplustree.h"
#include <algorithm>
#include <cassert>
#include <queue>
#include <iostream>


namespace algo {



    BplusTree::BplusTree(int key_size) {
        max_key_size = key_size;
        min_key_size = max_key_size / 2 + 1;
    }

    int BplusTree::Insert(const algo::Key &key, const algo::Value &value) {
        if (rootNode == nullptr) {
            rootNode = new BplusNode();
            rootNode->isleaf = true;
            assert(rootNode->Insert(key, value) == 1);
            return 0;
        }
        if (Contain(key)) {
            return 1;
        } else {
            BplusNode *node  = search(key);
            assert(node != nullptr);
            if (node->Insert(key, value) > max_key_size) {
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
        if (it != node->records.end() && it->key == key) {
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
            return  root;
        } else {
            return nullptr;
        }
    }

    int BplusTree::balance(algo::BplusNode *node) {

        if (node->size() > max_key_size) { // insert
            node->SplitTwo(min_key_size, max_key_size);
            rootNode = node->RootNode();
        } else if (node->size() < min_key_size) { // delete
            auto n = rootNode->RemoveNode(node, min_key_size, max_key_size);
            if (n != nullptr) {
                rootNode = n->RootNode();
            } else {
                rootNode == nullptr;
            }
        }
    }


    int BplusTree::remove(algo::BplusNode *node) {
        if (node->parent == nullptr) { // root node
            delete node;
            rootNode = nullptr;
            return 0;
        }
        int index = node->parent->childIndex(node);
        assert(index >= 0);
        auto sz = node->parent->removeIndex(index);
        if (sz < min_key_size) {
            balance(node->parent);
        }
        return 0;
    }

    BplusNode* BplusTree::merge(algo::BplusNode *left, algo::BplusNode *right) {
        assert(left->level == right->level);

        if (!left->isleaf) {
            for (auto child : right->children) {
                left->children.push_back(child);
            }
        } else  {
            left->next = right->next;
            for (auto rec : right->records) {
                left->records.push_back(rec);
            }
        }

        remove(right);
        return  left;
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


    void BplusTree::PrintTree() {
        std::queue<BplusNode*> que;
        if (rootNode == nullptr) {
            std::cout << "null tree" << std::endl;
            return ;
        }
        que.push(rootNode);
        while (!que.empty()) {
            auto p = que.front();
            que.pop();
            if (p->isleaf) {
                std::cout << p->level << "\t" << p->key << "\t" << p->size() << "\t" << std::endl;
                std::cout << "*****************" << std::endl;
                for (auto rec : p->records) {
                    std::cout << rec.key << "\t" << rec.value << "\t" <<  std::endl;
                }
                std::cout << "*****************" << std::endl;

            } else {
                std::cout << p->level << "\t" << p->key << "\t" << p->size() << "\t" << std::endl;
                std::cout << "##################" << std::endl;
                for (auto child : p->children) {
                    std::cout << child->key << "\t" << std::endl;
                    que.push(child);
                }
                std::cout << "##################" << std::endl;
            }
        }


    }
}