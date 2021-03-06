#pragma once 

#include <string>

namespace boltdb {
    class Status {
    public:
        Status() noexcept : state_(nullptr){}
        ~Status(){ delete [] state_;}
        Status(const Status& rhs);
        Status& operator=(const Status& rhs);
        Status(Status&& rhs) noexcept : state_(rhs.state_) { rhs.state_ = nullptr; }
        Status& operator=(Status&& rhs) noexcept;
        static Status Ok() {return Status();}
        static Status BucketError(const std::string &msg, const std::string &msg2 = {}){
            return Status(kBucketError, msg, msg2);
        }
        static Status DatabaseError(const std::string &msg, const std::string &msg2 = {}){
            return Status(kDatabaseError, msg, msg2);
        }
        static Status NotSupported(const std::string &msg, const std::string &msg2 = {}) {
            return Status(kNotSupported, msg, msg2);
        }
        static Status InvalidArgument(const std::string &msg, const std::string &msg2 = {}) {
            return Status(kInvalidArgument, msg, msg2);
        }
        static Status IOError(const std::string &msg, const std::string &msg2 = {} ) {
            return Status(kIOError, msg, msg2);
        }
        static Status TxError(const std::string &msg, const std::string &msg2 = {}) {
            return Status(kTxError, msg, msg2);
        }
        bool ok() const {   return state_ == nullptr;        }
        bool IsBucketError() const {  return code() == kBucketError;  }
        bool IsDatabaseError() const { return code() == kDatabaseError;}
        std::string ToString() const;

    private:
        static const char* CopyState(const char* state);
        const char * state_;
        enum Code {
            kOK = 0,
            kBucketError = 1,
            kDatabaseError = 2,
            kNotSupported = 3,
            kInvalidArgument = 4,
            kIOError = 5,
            kTxError = 6
        };
        Code code() const {
        	return (state_ == nullptr)? kOK: static_cast<Code>(state_[4]);
        }
        Status(Code code, const std::string &msg, const std::string &msg2);

    };

    inline Status::Status(const Status& rhs) {
        state_ = (rhs.state_ == nullptr) ? nullptr : CopyState(rhs.state_);
    }
    inline Status& Status::operator=(const Status& rhs) {
        // The following condition catches both aliasing (when this == &rhs),
        // and the common case where both rhs and *this are ok.
        if (state_ != rhs.state_) {
            delete[] state_;
            state_ = (rhs.state_ == nullptr) ? nullptr : CopyState(rhs.state_);
        }
        return *this;
    }
    inline Status& Status::operator=(Status&& rhs) noexcept {
        std::swap(state_, rhs.state_);
        return *this;
    }


    template <typename T, typename S = Status>
    class XXStatus {
    public:
        XXStatus(T &t, S& s): xx_status(t, s){
        }
        XXStatus(T t, S s): xx_status(t,s){

        }

        bool ok() {
            return xx_status.second.ok();
        }
        std::string ErrString() {
            return xx_status.second.toString();
        }
        S  status() {
            return xx_status.second;
        }
        T & value() {
            return xx_status.first;
        }
    private:
        std::pair<T, S> xx_status;

    };
    extern const char *ErrDatabaseNotOpen ;
    extern const char *ErrDatabaseOpen ;
    extern const char *ErrInvalid ;
    extern const char *ErrVersionMismatch ;
    extern const char *ErrChecksum ;
    extern const char *ErrTimeout ;

    // ErrTxNotWritable is returned when performing a write operation on a
    // read-only transaction.
    extern const char *ErrTxNotWritable ;

    // ErrTxClosed is returned when committing or rolling back a transaction
    // that has already been committed or rolled back.
    extern const char *ErrTxClosed ;

    // ErrDatabaseReadOnly is returned when a mutating transaction is started on a
    // read-only database.
    extern const char *ErrDatabaseReadOnly;


    // ErrBucketNotFound is returned when trying to access a bucket that has
    // not been created yet.
    extern const char *ErrBucketNotFound ;

    // ErrBucketExists is returned when creating a bucket that already exists.
    extern const char *ErrBucketExists ;

    // ErrBucketNameRequired is returned when creating a bucket with a blank name.
    extern const char *ErrBucketNameRequired ;

    // ErrKeyRequired is returned when inserting a zero-length key.
    extern const char *ErrKeyRequired ;

    // ErrKeyTooLarge is returned when inserting a key that is larger than MaxKeySize.
    extern const char *ErrKeyTooLarge ;

    // ErrValueTooLarge is returned when inserting a value that is larger than MaxValueSize.
    extern const char *ErrValueTooLarge ;

    // ErrIncompatibleValue is returned when trying create or delete a bucket
    // on an existing non-bucket key or when trying to create or delete a
    // non-bucket key on an existing bucket key.
    extern const char *ErrIncompatibleValue ;
}


/*

// These errors can be returned when opening or calling methods on a DB.
var (
	// ErrDatabaseNotOpen is returned when a DB instance is accessed before it
	// is opened or after it is closed.
	ErrDatabaseNotOpen = errors.New("database not open")

	// ErrDatabaseOpen is returned when opening a database that is
	// already open.
	ErrDatabaseOpen = errors.New("database already open")

	// ErrInvalid is returned when both meta pages on a database are invalid.
	// This typically occurs when a file is not a bolt database.
	ErrInvalid = errors.New("invalid database")

	// ErrVersionMismatch is returned when the data file was created with a
	// different version of Bolt.
	ErrVersionMismatch = errors.New("version mismatch")

	// ErrChecksum is returned when either meta page checksum does not match.
	ErrChecksum = errors.New("checksum error")

	// ErrTimeout is returned when a database cannot obtain an exclusive lock
	// on the data file after the timeout passed to Open().
	ErrTimeout = errors.New("timeout")
)

// These errors can occur when beginning or committing a Tx.
var (
	// ErrTxNotWritable is returned when performing a write operation on a
	// read-only transaction.
	ErrTxNotWritable = errors.New("tx not writable")

	// ErrTxClosed is returned when committing or rolling back a transaction
	// that has already been committed or rolled back.
	ErrTxClosed = errors.New("tx closed")

	// ErrDatabaseReadOnly is returned when a mutating transaction is started on a
	// read-only database.
	ErrDatabaseReadOnly = errors.New("database is in read-only mode")
)

// These errors can occur when putting or deleting a value or a bucket.
var (
	// ErrBucketNotFound is returned when trying to access a bucket that has
	// not been created yet.
	ErrBucketNotFound = errors.New("bucket not found")

	// ErrBucketExists is returned when creating a bucket that already exists.
	ErrBucketExists = errors.New("bucket already exists")

	// ErrBucketNameRequired is returned when creating a bucket with a blank name.
	ErrBucketNameRequired = errors.New("bucket name required")

	// ErrKeyRequired is returned when inserting a zero-length key.
	ErrKeyRequired = errors.New("key required")

	// ErrKeyTooLarge is returned when inserting a key that is larger than MaxKeySize.
	ErrKeyTooLarge = errors.New("key too large")

	// ErrValueTooLarge is returned when inserting a value that is larger than MaxValueSize.
	ErrValueTooLarge = errors.New("value too large")

	// ErrIncompatibleValue is returned when trying create or delete a bucket
	// on an existing non-bucket key or when trying to create or delete a
	// non-bucket key on an existing bucket key.
	ErrIncompatibleValue = errors.New("incompatible value")
)
*/