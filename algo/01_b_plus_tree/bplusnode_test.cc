//
// Created by ruoshui on 1/14/19.
//

#include <gtest/gtest.h>
#include "bplusnode.h"

namespace algo {
    BplusNode *CreateLeafNode(const Key &key, const Value &value){
        auto node = new BplusNode();
        node->isleaf = true;
        node->Insert(key, value);
        return node;
    }
    BplusNode *CreateBranchNode(const Key &key) {
        auto node = new BplusNode();
        node->isleaf = false;
        node->key = key;
        return node;
    }
    TEST(NodeTest, Node) {
        auto root = CreateBranchNode("root");
        EXPECT_TRUE(root->LeftNode() == nullptr);
        EXPECT_TRUE(root->prevSibling() == nullptr);
        auto left = CreateBranchNode("001");
        auto la = CreateLeafNode("001", "cpp");
        auto lb = CreateLeafNode("002", "java");
        left->InsertNode(la);
        left->InsertNode(lb);
        auto right = CreateBranchNode("003");
        auto ra = CreateLeafNode("003", "rust");
        auto rb = CreateLeafNode("004", "golang");
        right->InsertNode(ra);
        right->InsertNode(rb);
        root->InsertNode(left);
        root->InsertNode(right);
        EXPECT_TRUE(left->LeftNode() == nullptr);
        EXPECT_TRUE(left->RightNode() == right);
        EXPECT_TRUE(right->LeftNode() == left);
        EXPECT_TRUE(la->RightNode() == lb);
        EXPECT_TRUE(lb->LeftNode() == la);
        EXPECT_TRUE(lb->RightNode() == nullptr);
        EXPECT_TRUE(lb->nextSibling() == ra);
        EXPECT_TRUE(ra->prevSibling() == lb);

    }

    TEST(NodeTest, InsertAndDelete) {
        auto node = CreateLeafNode("002", "java");
        node->Insert("001", "cpp");
        EXPECT_TRUE(node->size() == 2);
        EXPECT_EQ(node->key, "001");
        node->Insert("003", "rust");
        EXPECT_TRUE(node->size() == 3);
        EXPECT_EQ(node->key, "001");

        node->Delete("001");
        EXPECT_TRUE(node->size() == 2);
        EXPECT_EQ(node->key, "002");
        EXPECT_TRUE(node->Delete("004") == 1);
        EXPECT_TRUE(node->size() == 2);
        EXPECT_EQ(node->key, "002");
        delete node;

        auto b = CreateBranchNode("001");
        auto b1 = CreateLeafNode("002", "java");
        auto b2 = CreateLeafNode("001", "cpp");
        b->InsertNode(b1);
        b->InsertNode(b2);
        EXPECT_EQ(b->size() , 2);
        EXPECT_EQ(b->key, "001");

        b->remove(b1, 3, 4);
        EXPECT_EQ(b->size(), 1);
        EXPECT_EQ(b->key, "002");
        b->remove(b2, 3, 4);
        EXPECT_EQ(b->size(), 0);
        EXPECT_EQ(b->key, "002");



    }
}