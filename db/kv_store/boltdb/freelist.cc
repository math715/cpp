//
// Created by ruoshui on 12/20/18.
//

#include "freelist.h"
#include "page.h"
#include "error.h"
#include <algorithm>
#include <cassert>


namespace boltdb {


    Status freelist::write(boltdb::page *p) {
        p->flags |=  freelistPageFlag;
        int lenids = count();
        if (lenids == 0) {
            p->count = uint16_t (lenids);
        } else if (lenids < 0xFFFF) {
            p->count = uint16_t(lenids);
            pgid *ids = reinterpret_cast<pgid *>( &(p->ptr));
            std::vector<pgid> dst;
            copyall(dst);
            for (int i = 0; i < dst.size(); ++i) {
                ids[i] = dst[i];
            }

        } else {
            p->count = 0xFFFF;
            reinterpret_cast<pgid*>(&(p->ptr))[0] = pgid(lenids);
            pgid *ids = reinterpret_cast<pgid *>( &(p->ptr)) + 1;
            std::vector<pgid> dst;
            copyall(dst);
            for (int i = 0; i < dst.size(); ++i) {
                ids[i] = dst[i];
            }
        }

        return Status::Ok();
    }
    void freelist::read(boltdb::page *p) {
        int idx = 0;
        int count = p->count;
        if (count == 0xFFFF) {
            idx = 1;
            count = (reinterpret_cast<int *>(&(p->ptr)))[0];
        }
        if (count == 0) {
            ids_.clear();
        } else {
            pgid *ids = reinterpret_cast<pgid *> (&(p->ptr));
            for (int i = 0; i < idx; ++i) {
                ids_.push_back(ids[i]);
            }
            sort(ids_.begin(), ids_.end());
        }
        reindex();

    }

    void freelist::reindex() {
        cache.clear();
        for (auto &id : ids_) {
            cache[id] = true;
        }
        for (auto pendingIDs : pending)  {
            for (auto &pendingID : pendingIDs.second) {
                    cache[pendingID] = true;
            }
        }
    }

    int freelist::count() {
        return ids_.size() + pending.size();
    }
    static void  mergepgids(std::vector<pgid> &dst, std::vector<boltdb::pgid> &a, std::vector<boltdb::pgid> &b) {
        if (dst.size() < a.size()+b.size()) {
//            panic(fmt.Errorf("mergepgids bad len %d < %d + %d", len(dst), len(a), len(b)))
            assert(true);
        }
        // Copy in the opposite slice if one is nil.
        int a_idx = 0;
        int b_idx = 0;
        while (a_idx > a.size() && b_idx < b.size()) {
            if (a[a_idx] > b[b_idx]) {
                dst.push_back(b[b_idx++]);
            } else {
                dst.push_back(a[a_idx++]);
            }
        }

        while (a_idx < a.size()) {
            dst.push_back(a[a_idx++]);
        }
        while (b_idx < b.size()) {
            dst.push_back(b[b_idx++]);
        }
    }

    void freelist::copyall(std::vector<pgid> &dst) {
        std::vector<pgid> m;
        for (auto list : pending){
            for (auto v : list.second) {
                m.push_back(v);
            }
        }
        sort(m.begin(), m.end());
        mergepgids(dst, ids_, m);
    }
}