#ifndef TOOLS__HASH_TABLE_HPP_
#define TOOLS__HASH_TABLE_HPP_

#include <ctype.h>
#include "List.hpp"
#include "SmallList.hpp"
#include "ChunkedArray_.hpp"

struct alignas(32) StringAVX;

// Hash table for rows up to 32 in length
class HashTable {
    using TData = SmallList<StringAVX>;
    //using TData = List<StringAVX>;
public:
    HashTable(size_t (*HashFunc)(const char*));

    ~HashTable();

    void Insert(const char* str, size_t len) noexcept;

    void Erase(const char* str, size_t len) noexcept;

    bool Find(const char* str, size_t len) const noexcept;

    void Dump() const noexcept;

    void PrintStatistics() const noexcept;
    void CreateOccupancyStateCSV(const char* file_name, char separator = ',') const noexcept;

    inline void* GetHash() const noexcept { return (void*) HashFunc_; }

    inline size_t Size() const noexcept { return size_; }

private:
    ChunkedArray_<TData*> data_;

    size_t capacity_;
    size_t size_;
    size_t occupancy_;

    size_t (*HashFunc_)(const char*);
};

#endif // TOOLS__HASH_TABLE_HPP_