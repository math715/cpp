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
            return 0;
        } else {
            return 1;
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

    BplusNode* BplusNode::RightNode() {
        if (parent == nullptr) {
            return nullptr;
        }
        auto index = parent->childIndex(this);
        if (index < parent->children.size() - 1) {
            return parent->children[index+1];
        } else {
            return nullptr;
        }
    }

    BplusNode* BplusNode::nextSibling() {
        auto right = RightNode();
        if (right == nullptr) {
            if (parent != nullptr) {
                auto p = parent->RightNode();
                if (p == nullptr) {
                    return nullptr;
                } else {
                    assert(!p->children.empty());
                    return p->children[0];
                }
            } else {
                return nullptr;
            }
        }
        return right;
    }


    BplusNode* BplusNode::prevSibling() {
        auto prev = LeftNode();
        if (prev == nullptr) {
            if (parent != nullptr) {
                auto p = parent->LeftNode();
                if (p == nullptr) {
                    return nullptr;
                } else {
                    assert(!p->children.empty());
                    return p->children.back();
                }
            } else {
                return nullptr;
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
            for (int i = min_key_size ; i < records.size(); ++i) {
                right->records.push_back(records[i]);
            }
            right->key = right->records[0].key;
            next = right;
            records.erase(records.begin() + min_key_size, records.end());
        } else {
            for (int i = min_key_size ; i < children.size(); ++i) {
                right->children.push_back(children[i]);
            }
            right->key = right->children[0]->key;
            children.erase(children.begin() + min_key_size, children.end());
        }
        if (parent == nullptr) { // root node
            auto p = new BplusNode();
//            level++;
//            right->level++;
            UpdateLevel(1);
            right->UpdateLevel(1);
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
        int index = children.size();
        for (int i = 0; i < children.size(); ++i) {
            if (children[i]->key > node->key) {
                index = i;
                break;
            }
        }
        assert(index >= 0);
        node->parent = this;
        node->level = level + 1;
        children.insert(children.begin()+ index, node);
        key = children[0]->key;
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

    void BplusNode::UpdateKey() {
        if(isleaf) {
            key = records[0].key ;
        } else {
            key = children[0]->key;
        }
        if (parent != nullptr) {
            parent->UpdateKey();
        }
    }

    BplusNode * BplusNode::RemoveNode(BplusNode *node, int min_key_size, int max_key_size) {
//        auto right = RightNode();
        auto m = MergeNode(node, max_key_size);
        if (m != nullptr) {
            remove(node, min_key_size, max_key_size);
            if (m->size() < min_key_size) {
                if (RootNode() != nullptr) {
                   return RootNode()->RemoveNode(m, min_key_size, max_key_size);
                }
            }

        } else {
            if (node->size() == 0) {
                remove(node, min_key_size, max_key_size);
                return nullptr;
            }
            return node;
        }
        return m;

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
            key = children[0]->key;
            if (parent != nullptr) {
                parent->UpdateKey();
            }
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
            key = records[0].key;
            if (parent != nullptr) {
                parent->UpdateKey();
            }
//            remove(node);
            return records.size();
        }

    }

    int BplusNode::MergeRightNode(algo::BplusNode *node) {
        assert(level == node->level);
        if (!isleaf) {
            assert(!children.empty());
            for (auto child : node->children) {
                child->parent = parent;
                children.push_back(child);
            }
            return children.size();
        } else  {
            assert(!records.empty());
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
        auto left = node->LeftNode();
        if (left != nullptr) {
            if (left->size() + node->size() < max_key_size) {
                auto sz = left->MergeRightNode(node);
                return left;
            }
        }
        auto right = node->RightNode();
        if (right != nullptr) {
            if (right->size() + node->size() < max_key_size) {
                right->MergeLeftNode(node);
                return right;
            }
        }
        if (right == nullptr && right == nullptr) {
            auto prev = node->prevSibling();
            if (prev != nullptr) {
                if (prev->size() + node->size() < max_key_size) {
                    int sz = prev->MergeRightNode(node);
                    return prev;
                }
            }
            auto n = node->nextSibling();
            if (n != nullptr) {
                if (n->size() + node->size() < max_key_size) {
                    int sz = n->MergeLeftNode(node);
                    return n;
                }
            }
        }
        return nullptr;
    }

}