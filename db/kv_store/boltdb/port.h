#include <string>
#include "error.h"
#include <sys/mman.h>
#include <unistd.h>
#include "db.h"



namespace blotdb {
    class Status;
    class port {
        public:
            Status syscall_mmap(DB &db, const std::string &path, size_t sz, int fd) {
                char *addr = mmap(NULL, sz,  PROT_WRITE, MAP_PRIVATE, fd, 0);
                db.dataref = addr;
            }
    };
    
} // blotdb
