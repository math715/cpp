//
// Created by ruoshui on 1/7/19.
//


#include <gtest/gtest.h>
#include "node.h"
#include "Tx.h"
#include "page.h"

namespace boltdb {

    TEST(NodeTest, Put) {
        node *n = new node();
        meta *m = new meta();
        m->pgcnt = 1;
        Tx *tx = new Tx();
        tx->meta_ = m;
        n->bucket = new Bucket(tx);

        boltdb_key_t oldkey("baz"), newKey("baz"), value("2");
        n->put(oldkey, newKey, value, 0, 0);
        oldkey = boltdb_key_t("foo");
        newKey = boltdb_key_t("foo");
        value = boltdb_key_t("0");
        n->put(oldkey, newKey, value, 0, 0);
        oldkey = boltdb_key_t("bar");
        newKey = boltdb_key_t("bar");
        value = boltdb_key_t("1");
        n->put(oldkey, newKey, value, 0, 0);
        oldkey = boltdb_key_t("foo");
        newKey = boltdb_key_t("foo");
        value = boltdb_key_t("3");
        n->put(oldkey, newKey, value, 0, boltdb::leafPageFlag);

//        if (n->inodes.size() != 3) {
        EXPECT_EQ(n->inodes.size(), 3);
        EXPECT_EQ(n->inodes[0]->key, "bar");
        EXPECT_EQ(n->inodes[0]->value, "1");
        EXPECT_EQ(n->inodes[1]->key, "baz");
        EXPECT_EQ(n->inodes[1]->value, "2");
        EXPECT_EQ(n->inodes[2]->key, "foo");
        EXPECT_EQ(n->inodes[2]->value, "3");
        EXPECT_EQ(n->inodes[2]->flags, leafPageFlag);
//        }

        delete tx;
        delete m;
        delete n->bucket;
        delete n;
    }

    TEST(NodeTest, ReadLeafPage) {
        char buf[4096];
        page *p = reinterpret_cast<page *>(&buf[0]);
        p->flags = leafPageFlag;
        p->count = 2;

// Insert 2 elements at the beginning. sizeof(leafPageElement) == 16
        auto nodes = reinterpret_cast<leafPageElement *>(&(p->ptr));
        nodes[0].flags = 0;
        nodes[0].pos = 32;
        nodes[0].ksize = 3;
        nodes[0].vsize = 4;
        nodes[1].flags = 0;
        nodes[1].pos = 23;
        nodes[1].ksize = 10;
        nodes[1].vsize = 3;

// Write data for the nodes at the end.
        char *data = reinterpret_cast< char *>((&nodes[2]));
        memcpy(data, "barfooz", strlen("barfooz"));
        memcpy(data + 7, "helloworldbye", strlen("helloworldbye"));

// Deserialize page into a leaf.
        node *n = new node();
        n->read(p);

// Check that there are two inodes with correct data.
        EXPECT_TRUE(n->isLeaf);
        EXPECT_EQ(n->inodes.size(), 2);
        EXPECT_EQ(n->inodes[0]->key, "bar");
        EXPECT_EQ(n->inodes[0]->value, "fooz");
        EXPECT_EQ(n->inodes[1]->key, "helloworld");
        EXPECT_EQ(n->inodes[1]->value, "bye");
    }


    TEST(NodeTest, WriteLeafPage) {
// Create a node.
//        n := &node{isLeaf: true, inodes: make(inodes, 0), bucket: &Bucket{tx: &Tx{db: &DB{}, meta: &meta{pgid: 1}}}}
        node *n = new node();
        n->isLeaf = true;
        n->bucket = new Bucket();
        DB *db = new DB();
        meta *m = new meta();
        m->pgcnt = 1;
        Tx *tx = new Tx();
        tx->db_ = db;
        tx->meta_ = m;
        n->bucket->tx = tx;
        boltdb_key_t oldkey("susy"), newkey("susy"), value("que");
        n->put(oldkey, newkey, value, 0, 0);
        oldkey = "ricki", newkey = "ricki", value = "lake";
        n->put(oldkey, newkey, value, 0, 0);
        oldkey = "john", newkey = "john", value = "johnson";
        n->put(oldkey, newkey, value, 0, 0);

        // Write it to a page.
        char buf[4096];
        page *p = reinterpret_cast<page *>(&buf[0]);
        n->write(p);

        // Read the page back in.
        node *n2 = new node();
        n2->read(p);

        // Check that the two pages are the same.
        EXPECT_EQ(n->inodes.size(), 3);
        EXPECT_EQ(n->inodes[0]->key, "john");
        EXPECT_EQ(n->inodes[0]->value, "johnson");
        EXPECT_EQ(n->inodes[1]->key, "ricki");
        EXPECT_EQ(n->inodes[1]->value, "lake");
        EXPECT_EQ(n->inodes[2]->key, "susy");
        EXPECT_EQ(n->inodes[2]->value, "que");

        // clear memory
        delete m;
        delete db;
        delete tx;
        delete n->bucket;
        delete n;


    }


    TEST(NodeTEST, Split) {
        node *n = new node();
        n->isLeaf = true;
        n->bucket = new Bucket();
        DB *db = new DB();
        meta *m = new meta();
        m->pgcnt = 1;
        Tx *tx = new Tx();
        tx->db_ = db;
        tx->meta_ = m;
        n->bucket->tx = tx;
        boltdb_key_t oldkey, newkey, value;
        oldkey = "00000001", newkey = "00000001", value ="0123456701234567";
        n->put(oldkey, newkey, value, 0, 0);
        oldkey = "00000002", newkey = "00000002", value ="0123456701234567";
        n->put(oldkey, newkey, value, 0, 0);
        oldkey = "00000003", newkey = "00000003", value ="0123456701234567";
        n->put(oldkey, newkey, value, 0, 0);
        oldkey = "00000004", newkey = "00000004", value ="0123456701234567";
        n->put(oldkey, newkey, value, 0, 0);
        oldkey = "00000005", newkey = "00000005", value ="0123456701234567";
        n->put(oldkey, newkey, value, 0, 0);

        // Split between 2 & 3.
        n->split(100);

        node * parent = n->parent;
        ASSERT_EQ(parent->children.size(), 2);
        ASSERT_EQ(parent->children[0]->inodes.size(), 2);
        ASSERT_EQ(parent->children[1]->inodes.size(), 3);

        delete m;
        delete db;
        delete tx;
        delete n->bucket;
        delete n;

    }


    TEST(NodeTest, SplitMinKeys) {
        node *n = new node();
        n->isLeaf = true;
        n->bucket = new Bucket();
        DB *db = new DB();
        meta *m = new meta();
        m->pgcnt = 1;
        Tx *tx = new Tx();
        tx->db_ = db;
        tx->meta_ = m;
        n->bucket->tx = tx;
        boltdb_key_t oldkey, newkey, value;
        oldkey = "00000001", newkey = "00000001", value ="0123456701234567";
        n->put(oldkey, newkey, value, 0, 0);
        oldkey = "00000002", newkey = "00000002", value ="0123456701234567";
        n->put(oldkey, newkey, value, 0, 0);
        // Split.
        n->split(20);
        ASSERT_TRUE(n->parent == nullptr);

        delete m;
        delete db;
        delete tx;
        delete n->bucket;
        delete n;

    }

    TEST(NodeTEST, SplitSinglePage) {
        node *n = new node();
        n->isLeaf = true;
        n->bucket = new Bucket();
        DB *db = new DB();
        meta *m = new meta();
        m->pgcnt = 1;
        Tx *tx = new Tx();
        tx->db_ = db;
        tx->meta_ = m;
        n->bucket->tx = tx;
        boltdb_key_t oldkey, newkey, value;
        oldkey = "00000001", newkey = "00000001", value ="0123456701234567";
        n->put(oldkey, newkey, value, 0, 0);
        oldkey = "00000002", newkey = "00000002", value ="0123456701234567";
        n->put(oldkey, newkey, value, 0, 0);
        oldkey = "00000003", newkey = "00000003", value ="0123456701234567";
        n->put(oldkey, newkey, value, 0, 0);
        oldkey = "00000004", newkey = "00000004", value ="0123456701234567";
        n->put(oldkey, newkey, value, 0, 0);
        oldkey = "00000005", newkey = "00000005", value ="0123456701234567";
        n->put(oldkey, newkey, value, 0, 0);


        n->split(4096);
        ASSERT_TRUE(n->parent == nullptr);
        delete m;
        delete db;
        delete tx;
        delete n->bucket;
        delete n;

    }
}