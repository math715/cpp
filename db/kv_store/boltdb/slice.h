//
// Created by ruoshui on 12/30/18.
//


namespace boltdb {
    class slice {
    public:
        slice(): data_(""), size_(0), cap_(0){}
        slice(const char *data, int n, bool flag = false) {
            if (!flag) {
                data_ = data;
                size_ = n;
                cap_ = 0;
            } else {
                cap_ = (cap + 7) >> 3 << 3;
                data_ = new char[cap];
                memcpy(data_, data, n);
                size_ = n;
            }
        }
        ~slice(){
            if (cap_ > 0) {
                delete data_;
            }
        }

        const char *data() const {
            return data_;
        }
        int size() {
            return size_;
        }
        int cap() const {
            return cap_;
        }
        bool empty() const {
            return size_ == 0;
        }


    private:
        char *data_;
        int size_;
        int cap_; // memory, cap = 0? no alloc memory, just ptr;
    };

    inline bool operator==(const slice& x, const slice& y) {
        return (x.size() == y.size()) &&
            (memcmp(x.data(), y.data(), x.size()) == 0);
    }
    inline bool operator!=(const slice& x, const slice&y) {
        return !(x == y);
    }
}


