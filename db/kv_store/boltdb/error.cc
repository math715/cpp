
#include "error.h"
#include <cassert>
#include <cstring>
namespace boltdb {
        Status::Status(Code code, const std::string &msg, const std::string &msg2) {
            assert(code != kOK);
            const uint32_t len1 = msg.size();
            const uint32_t len2 = msg2.size();
            const uint32_t size = len1 + (len2 ? (2 + len2) :  0);
            char *result = new char[size+5];
            memcpy(result, &size, sizeof(size));
            result[4] = static_cast<char>(code);
            memcpy(result + 5, msg.data(), msg.size());
            if (len2) {
                result[5 + len1] = ':';
                result[6 + len1] = ' ';
                memcpy(result + len1 + 7, msg2.data(), len2);
            }
            state_ = result;
        }

        std::string Status::ToString() const {
            if (state_ == nullptr) {
                return "OK";
            } else {
                char tmp[32];
                const char *type;
                switch (code()) {
                    case kOK:
                        type = "OK";
                        break;
                    case kBucketError:
                        type = "Bucket error: ";
                        break;
                    case kDatabaseError:
                        type = "Database error: ";
                        break;
                    case kNotSupported:
                        type = "Not implemented: ";
                        break;
                    case kInvalidArgument:
                        type = "Invalid argument: ";
                        break;
                    case kIOError:
                        type = "IO error: ";
                        break;
                    case kTxError:
                        type = "Tx error: ";
                        break;
                    default :
                        snprintf(tmp, sizeof(tmp), "Unknown code(%d): ", static_cast<int>(code()));
                        type = tmp;
                        break;
                }
                std::string result(type);
                uint32_t length;
                memcpy(&length, state_, sizeof(length));
                result.append(state_ + 5, length);
                return result;
            }
            
        }

    const char* Status::CopyState(const char *state) {
        uint32_t size;
        memcpy(&size, state, sizeof(size));
        char* result = new char[size + 5];
        memcpy(result, state, size + 5);
        return result;
        }


    const char *ErrDatabaseNotOpen = "database not open";
    const char *ErrDatabaseOpen = "database already open";
    const char *ErrInvalid = "invalid database";
    const char *ErrVersionMismatch = "version mismatch";
    const char *ErrChecksum = "checksum error";
    const char *ErrTimeout = "timeout";

    // ErrTxNotWritable is returned when performing a write operation on a
    // read-only transaction.
    const char *ErrTxNotWritable = "tx not writable";

    // ErrTxClosed is returned when committing or rolling back a transaction
    // that has already been committed or rolled back.
    const char *ErrTxClosed = "tx closed";

    // ErrDatabaseReadOnly is returned when a mutating transaction is started on a
    // read-only database.
    const char *ErrDatabaseReadOnly = "database is in read-only mode";


    // ErrBucketNotFound is returned when trying to access a bucket that has
    // not been created yet.
    const char *ErrBucketNotFound = "bucket not found";

    // ErrBucketExists is returned when creating a bucket that already exists.
    const char *ErrBucketExists = "bucket already exists";

    // ErrBucketNameRequired is returned when creating a bucket with a blank name.
    const char *ErrBucketNameRequired = "bucket name required";

    // ErrKeyRequired is returned when inserting a zero-length key.
    const char *ErrKeyRequired = "key required";

    // ErrKeyTooLarge is returned when inserting a key that is larger than MaxKeySize.
    const char *ErrKeyTooLarge = "key too large";

    // ErrValueTooLarge is returned when inserting a value that is larger than MaxValueSize.
    const char *ErrValueTooLarge = "value too large";

    // ErrIncompatibleValue is returned when trying create or delete a bucket
    // on an existing non-bucket key or when trying to create or delete a
    // non-bucket key on an existing bucket key.
    const char *ErrIncompatibleValue = "incompatible value";
    
} // blotdb
