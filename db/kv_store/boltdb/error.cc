
#include "error.h"
#include <cassert>
#include <cstring>
namespace boltdb {
        Status::Status(Code code, const std::string &msg, const std::string &msg2) {
            assert(code != kOK);
            const uint32_t len1 = msg.size();
            const uint32_t len2 = msg.size();
            const uint32_t size = len1 + len2 + 5;
            char *result = new char[size];
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
                    case kNotFound:
                        type = "NotFound: ";
                        break;
                    case kCorruption:
                        type = "Corruption: ";
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
    
} // blotdb
