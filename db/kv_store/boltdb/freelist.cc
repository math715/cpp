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


    void freelist::free(boltdb::txid tid, boltdb::page *p) {
        if (p->id <= 1) {
            assert(true);
        }

        auto ids = pending[tid];
        for (auto id = p->id; id <= p->id + pgid(p->overflow); ++id) {
            if (cache[id]) {
                assert(true);
            }
            ids.push_back(id);
            cache[id] = true;
        }
        pending[tid] = ids;
    }


    pgid freelist::allocate(int n) {
        if (ids_.size() == 0) {
            return 0;
        }

        pgid initial = 0, previd = 0;
        for (int i = 0; i < ids_.size(); ++i ) {
            pgid id = ids_[i];
            if (id <= 1) {
                assert(true);
//                panic(fmt.Sprintf("invalid page allocation: %d", id))
            }

            // Reset initial page if this is not contiguous.
            if (previd == 0 || id-previd != 1) {
                initial = id;
            }

            // If we found a contiguous block then remove it and return it.
            if ((id-initial)+1 == pgid(n)) {
                // If we're allocating off the beginning then take the fast path
                // and just adjust the existing slice. This will use extra memory
                // temporarily but the append() in free() will realloc the slice
                // as is necessary.
                if ((i + 1) == n) {
//                   ids_ = ids_[i+1:]
                    ids_.erase(ids_.begin(), ids_.begin() + n);
                } else {
                    for (int idx = i - n + 1; idx + n < ids_.size(); ++idx){
                        ids_[idx] = ids_[idx+n];
                    }
                    ids_.resize(ids_.size() - n);
                }

                // Remove from the free cache.
                for ( pgid  i = pgid(0); i < pgid(n); i++ ){
//                    delete(f.cache, initial+i)
                    cache.erase(initial+i);
                }

                return initial;
            }

            previd = id;
        }
        return 0;
    }

    void freelist::rollback(txid tid) {
        for ( auto id : pending[tid] ){
            cache.erase(id);
        }

        // Remove pages from pending list.
//        delete(f.pending, txid)
        pending.erase(tid);
    }

    void freelist::release(boltdb::txid tid) {
        std::vector<pgid> m ;
        for ( auto ids : pending) {
            if (ids.first <= tid) {
                        // Move transaction's pending pages to the available freelist.
                        // Don't remove from the cache since the page is still free.
                        for (auto id : ids.second) {
                            m.push_back(id);
                        }
                        pending.erase(ids.first);
//                        delete(f.pending, tid)
                }
        }
        sort(m.begin(), m.end());

//        ids_ = merge(m);
        std::vector<pgid> dst;
        std::merge(m.begin(), m.end(), ids_.begin(), ids_.end(), std::back_inserter(dst));
        ids_ = dst;
    }

    void freelist::reload(boltdb::page *p) {
        read(p);

        // Build a cache of only pending pages.
        std::map<pgid, bool> pcache;
        for ( auto pendingIDs : pending ) {
            for ( auto pendingID : pendingIDs.second) {
                    pcache[pendingID] = true;
            }
        }

        // Check each page in the freelist and build a new available freelist
        // with any pages not in the pending lists.
//        var a []pgid
        std::vector<pgid> a;
        for ( auto id : ids_) {
            if (pcache.find(id) != pcache.end()) {
                a.push_back(id);
            }
        }
        ids_ = a;

        // Once the available list is rebuilt then rebuild the free cache so that
        // it includes the available and pending free pages.
        reindex();
    }


    int freelist::size() {
        int n = count();
        if (n >= 0XFFFF) {
            n++;
        }
        return pageHeaderSize + (int(sizeof(pgid)) * n);
    }

    int freelist::free_count() {
        return ids_.size();
    }
    int freelist::pending_count() {
        int  count = 0;
        for (auto ls : pending) {
            count += ls.second.size();
        }
        return count;
    }
}