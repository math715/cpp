//
// Created by ruoshui on 12/19/18.
//

#include "Tx.h"
#include "db.h"


namespace  boltdb {
    void Tx::init(boltdb::DB *db) {
        this->db_ = db;
        pages.clear();
        meta_ = new meta();
        *meta_ = *(db->Meta());
        root = Bucket(this);
        root.root = meta_->root.root;
        root.sequence = meta_->root.sequence;
        if (writable) {
                meta_->txid += txid(1);
        }
    }

    page* Tx::Page(boltdb::pgid id) {
            if (!pages.empty()) {
                 if (pages.find(id) != pages.end()) {
                         return pages[id];
                 }
            }
            return db_->Page(id);
    }

}