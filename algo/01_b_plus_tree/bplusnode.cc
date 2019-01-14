//
// Created by ruoshui on 1/14/19.
//

#include <cassert>
#include "bplusnode.h"
#include <algorithm>
namespace algo {

    BplusNode::~BplusNode() {
        parent = nullptr;
        next = nullptr;
    }
    int BplusNode::Insert(const algo::Key &key, const algo::Value &value) {
        auto it = std::lower_bound(records.begin(), records.end(), key, [](const BplusRecord &rec, const Key &key) {
            return rec.key < key;
        });
        if (it != records.end() && it->key == key){
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
        int index = parent->childIndex(this);
        if (index > 0 ) {
            return parent->children[index-1];
        } else {
            return nullptr;
        }

    }

    BplusNode* BplusNode::nextSibling() {
        auto right = RightNode();
        if (right == nullptr) {
            auto p = parent->RightNode();
            if (p == nullptr) {
                return nullptr;
            } else {
                assert(!p->children.empty());
                return p->children[0];
            }
        }
        return right;
    }


    BplusNode* BplusNode::prevSibling() {
        auto prev = LeftNode();
        if (prev == nullptr) {
            auto p = parent->LeftNode();
            if (p == nullptr) {
                return nullptr;
            } else {
                assert(!p->children.empty());
                return p->children.back();
            }
        }
        return prev;
    }
    int BplusNode::childIndex(algo::BplusNode *node) {
        for (int i = 0; i < children.size(); ++i) {
            if (children[i] == node) {
                return i;
            }
        }
        return -1;
    }
    BplusNode* BplusNode::RightNode() {
        if (parent == nullptr) {
            return nullptr;
        }
        auto index = parent->childIndex(this);
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

    int BplusNode::removeIndex(int index) {
        assert(children.size() > index);
        children.erase(children.begin() + index);
        if (children.size() == 0) {
            key = children[0]->key;
        }
        return children.size();
    }

    BplusNode * BplusNode::SplitTwo(int min_key_size, int max_key_size) {
        auto right = new BplusNode();
        right->level = level;
        right->isleaf = isleaf;
        right->parent = parent;
        if (isleaf) {
            for (int i = min_key_size + 1; i < records.size(); ++i) {
                right->records.push_back(records[i]);
            }
            right->key = right->records[0].key;
            next = right;
            records.erase(records.begin() + min_key_size);
        } else {
            for (int i = min_key_size + 1; i < children.size(); ++i) {
                right->children.push_back(children[i]);
            }
            right->key = right->children[0]->key;
            children.erase(children.begin() + min_key_size);
        }
        if (parent == nullptr) { // root node
            auto p = new BplusNode();
//            level++;
//            right->level++;
            UpdateLevel(0);
            right->UpdateLevel(0);
            parent = p;
            right->parent = p;
            p->isleaf = false;
            p->children.push_back(this);
            p->children.push_back(right);
            p->key = p->children[0]->key;
            p->next = nullptr;
            return right;
        } else {
            int sz = parent->InsertNode(right);
            if (sz > max_key_size) {
                return parent->SplitTwo(min_key_size, max_key_size);
            }

        }

    }


    int BplusNode::InsertNode(algo::BplusNode *node) {
        int index = 0;
        for (int i = 0; i < children.size(); ++i) {
            if (children[i]->key > node->key) {
                index = i;
                break;
            }
        }
        assert(index > 0);
        children.insert(children.begin()+ index, node);
        return children.size();
    }

    BplusNode* BplusNode::RootNode() {
        if (parent == nullptr) {
            return this;
        } else {
            return parent->RootNode();
        }
    }

    void BplusNode::UpdateLevel(int value) {
        if (value > 0) {
            level++;
            for (auto child: children) {
                child->UpdateLevel(value);
            }
        } else {
            level--;
            for (auto child:children) {
                child->UpdateLevel(value);
            }
        }
    }


    BplusNode * BplusNode::RemoveNode(BplusNode *node, int min_key_size, int max_key_size) {
//        auto right = RightNode();
        auto m = MergeNode(node, max_key_size);
        if (m != nullptr) {
            if (m->size() < min_key_size)
            remove(node, min_key_size, max_key_size);
        } else {
            if (node->size() == 0) {
                remove(node, min_key_size, max_key_size);
            }
        }

        // delete null node
        // basecause  this node have no brother and can not merge this sibling node



    }

    int BplusNode::MergeLeftNode(algo::BplusNode *node) {
        assert(level == node->level);

        if (!isleaf) {
            std::vector<BplusNode *> vs;
            for (auto child : node->children) {
                child->parent = parent;
                vs.push_back(child);
//                node->children.push_back(child);
            }
            for (auto child : children) {
                vs.push_back(child);
            }
            children = vs;
//            remove(node);
            return children.size();
        } else  {
            node->next = next;
            std::vector<BplusRecord> vs;
            for (auto rec : node->records) {
                vs.push_back(rec);
            }
            for (auto rec : records) {
                vs.push_back(rec);
            }
            records = vs;
//            remove(node);
            return records.size();
        }

    }

    int BplusNode::MergeRightNode(algo::BplusNode *node) {
        assert(level == node->level);
        if (!isleaf) {
            for (auto child : node->children) {
                child->parent = parent;
                children.push_back(child);
            }
            return children.size();
        } else  {
            next = node->next;
            for (auto rec : node->records) {
                records.push_back(rec);
            }
            return records.size();
        }

//        return  left;
    }

    BplusNode * BplusNode::remove(algo::BplusNode *node, int min_key_size, int max_key_size) {
        if (node->parent != nullptr) {
            int index = node->parent->childIndex(node);
            auto sz = node->parent->removeIndex(index);
            BplusNode *p;
            if (sz == 0) {
               p = remove(node->parent, min_key_size, max_key_size);
               if (p == nullptr) {
                   return nullptr;
               }
            } else if (sz < min_key_size) {
               p = RemoveNode(node->parent, min_key_size, max_key_size);
               if (p == nullptr) {
                   return nullptr;
               }
            }
            p = node->parent ;
            delete node;
            return p;
        } else { // delete root node
            if (!node->isleaf) {
                for (auto child : node->children) {
                    child->UpdateLevel(-1);
                }
            } else {
                if (node->size() > 0) {
                    return node;
                } else {
                    delete node;
                    return nullptr;
                }
            }

        }
    }

    BplusNode * BplusNode::MergeNode(BplusNode *node, int max_key_size) {
        auto left = LeftNode();
        if (left != nullptr) {
            if (left->size() + size() < max_key_size) {
                auto sz = left->MergeRightNode(this);
                return left;
            }
        }
        auto right = RightNode();
        if (right != nullptr) {
            if (right->size() + size() < max_key_size) {
                right->MergeLeftNode(this);
                return right;
            }
        }
        if (right == nullptr && right == nullptr) {
            auto prev = prevSibling();
            if (prev != nullptr) {
                if (prev->size() + size() < max_key_size) {
                    int sz = prev->MergeRightNode(this);
                    return prev;
                }
            }
            auto n = nextSibling();
            if (n != nullptr) {
                if (n->size() + size() < max_key_size) {
                    int sz = n->MergeLeftNode(this);
                    return n;
                }
            }
        }
        return nullptr;
    }

}