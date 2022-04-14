#ifndef TOOLS__HASH_TABLE_HPP_
#define TOOLS__HASH_TABLE_HPP_

#include <ctype.h>
#include "List.hpp"
#include "ChunkedArray_.hpp"

struct alignas(32) StringAVX;

// Hash table for rows up to 32 in length
class HashTable {
public:
    HashTable();
    ~HashTable();

    void Insert(const char* str, size_t len) noexcept;

    void Erase(const char* str, size_t len) noexcept;

    void Dump() const noexcept;

    void PrintStatistics() const noexcept;
    

private:
    ChunkedArray_<List<StringAVX>*> data_;

    const size_t& capacity_;
    size_t size_;
    size_t occupancy_;

    size_t (*hash_func_)(const char*);
};

#endif // TOOLS__HASH_TABLE_HPP_