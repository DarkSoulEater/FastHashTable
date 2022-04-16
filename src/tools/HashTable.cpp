#include <assert.h>
#include <fstream>
#include <string.h>
#include <immintrin.h>
#include <utility>

#include "HashTable.hpp"

static size_t ToNearSimple(size_t val) {
    while (true) {
        for (size_t i = 2; i * i < val; ++i) {
            if (val % i == 0) goto Fail;
        }
        return val;

        Fail:
        --val;
    }
    
}

// --------------------String for AVX (alignas 32)-------------------

struct alignas(32) StringAVX {
    //char data_[32] = {};
    __m256i data_;

    friend std::ostream& operator<<(std::ostream &out, const StringAVX &str) {
        //std::cout << "str = " << str.data_; // TODO: 
        return out;
    }

    bool operator==(const StringAVX& other) const noexcept {
        //return strcmp(data_, other.data_) == 0;
        //assert((size_t)data_ % 32 == 0);
        //assert((size_t)other.data_ % 32 == 0);

        //__m256i a = _mm256_load_si256((__m256i const*)data_);
        //__m256i b = _mm256_load_si256((__m256i const*)other.data_);

        //__m256i res = _mm256_cmpeq_epi8(a, b);

        __m256i res = _mm256_cmpeq_epi8(data_, other.data_);

        return (_mm256_movemask_epi8(res) + 1) == 0;
    }
};

// ------------------------------------------------------------------

// --------------------------HashTable-------------------------------

HashTable::HashTable(size_t (*HashFunc)(const char*)) : capacity_(0), size_(0), occupancy_(0), HashFunc_(HashFunc) {
    data_.ReserveChunk(32);
    capacity_ = ToNearSimple(data_.GetCapacity());
    for (size_t i = 0; i < capacity_; ++i) {
        data_[i] = nullptr;
    }
}

HashTable::~HashTable() {

}


void HashTable::Insert(const char* str, size_t len) noexcept {
    assert(str);

    StringAVX strAVX;
    memcpy(&strAVX.data_, str, len);

    size_t ind = HashFunc_((const char*)&strAVX.data_) % capacity_;
    if (data_[ind] == nullptr) data_[ind] = new TData;
    //else if (data_[ind]->Find(strAVX) != 0) return;

    data_[ind]->Insert(strAVX, 0);

    ++size_;
    // data_[ind]->Insert(std::move(strAVX), 0);
}

void HashTable::Erase(const char* str, size_t len) noexcept {
    assert(0 && "No implement");
}

bool HashTable::Find(const char* str, size_t len) const noexcept {
    assert(str);

    StringAVX strAVX;
    memcpy(&strAVX.data_, str, len);

    size_t ind = HashFunc_((const char*)&strAVX.data_) % capacity_;
    if (data_[ind] == nullptr) return false;
    
    return data_[ind]->Find(strAVX) != 0;
}

void HashTable::Dump() const noexcept {
    data_.Dump();
}

void HashTable::CreateOccupancyStateCSV(const char* file_name, char separator) const noexcept {
    std::ofstream file(file_name);
        if (!file.is_open()) {
        perror("Error: failed open file\n");
        return;
    }
    file << "# Occupancy state " << file_name << "\n";
    
    for (size_t i = 0; i < capacity_; ++i) {
        if (data_[i] == nullptr) {
            file << i << separator << " " << 0 << "\n";
        } else {
            file << i << separator << " " << data_[i]->Size() << "\n";
        }
    }

    file.close();
}

void HashTable::PrintStatistics() const noexcept {
    
    for (size_t i = 0; i < capacity_; ++i) {
        if (data_[i] == nullptr) {
            std::cout << i << " = " << 0 << "    ";
        } else {
            std::cout << i << " = " << data_[i]->Size() << "    ";
        }
        if (i && (i + 1) % 8 == 0) std::cout << "\n";
    }
}

// ------------------------------------------------------------------