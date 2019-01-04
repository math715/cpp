//
// Created by ruoshui on 12/30/18.
//


#include <cstring>

namespace boltdb {
    class Slice {
    public:
        Slice(): data_(nullptr), size_(0), cap_(0){}
        Slice(char *data, int n, bool flag = false) {
            if (!flag) {
                data_ = data;
                size_ = n;
                cap_ = 0;
            } else {
                size_ = n;
                cap_ = (n + 7) >> 3 << 3;
                data_ = new char[cap_];
                memcpy(data_, data, n);

            }
        }
        Slice(std::string &s){
            size_ = s.size();
            cap_ = (size_ + 7) >> 3 << 3;
            data_ = new char[cap_];
            memcpy(data_, s.data(), s.size());
        }

        ~Slice(){
            if (cap_ > 0) {
                delete data_;
            }
        }

        const char *data() const {
            return data_;
        }
        int size() const {
            return size_;
        }
        int cap() const {
            return cap_;
        }
        bool empty() const {
            return size_ == 0;
        }

        std::string toString() const {
            return std::string(data_, size_);
        }

        int compare(const Slice& b) const;

    private:
        char *data_;
        int size_;
        int cap_; // memory, cap = 0? no alloc memory, just ptr;
    };

    inline bool operator==(const Slice& x, const Slice& y) {
        return (x.size() == y.size()) &&
            (memcmp(x.data(), y.data(), x.size()) == 0);
    }
    inline bool operator!=(const Slice& x, const Slice&y) {
        return !(x == y);
    }
//    inline bool operator<(const Slice &x, const Slice &y) {
//        int size = std::min(x.size(), y.size());
//        int ret = memcmp(x.data(), y.data(), size);
//        if (ret == 0) {
//            ret = x.size() < y.size();
//        }
//        return ret < 0;
//    }
    inline int Slice::compare(const boltdb::Slice &b) const {
        const size_t min_len = (size_ < b.size_) ? size_ : b.size_;
        int r = memcmp(data_, b.data_, min_len);
        if (r == 0) {
            if (size_ < b.size_) r = -1;
            else if (size_ > b.size_) r = +1;
        }
        return r;
    }
}


