#ifndef TOOLS__HASH_TABLE_HPP_
#define TOOLS__HASH_TABLE_HPP_

#include <ctype.h>
#include "List.hpp"
#include "ChunkedArray_.hpp"

struct alignas(32) StringAVX;

enum class HashFunction {
    HashZero,
    HashOneASCII,
    HashSumASCII,
    HashLenght,
    HashSumRoll,
    HashPolynom,
    HashCRC32
};

// Hash table for rows up to 32 in length
class HashTable {
public:
    HashTable();
    HashTable(HashFunction eFunction);
    HashTable(size_t (*HashFunc)(const char*));

    ~HashTable();

    void Insert(const char* str, size_t len) noexcept;

    void Erase(const char* str, size_t len) noexcept;

    void Dump() const noexcept;

    void PrintStatistics() const noexcept;
    void CreateOccupancyStateCSV(const char* file_name, char separator = ',') const noexcept;
    

private:
    ChunkedArray_<List<StringAVX>*> data_;

    size_t capacity_;
    size_t size_;
    size_t occupancy_;

    size_t (*HashFunc_)(const char*);
};

#endif // TOOLS__HASH_TABLE_HPP_