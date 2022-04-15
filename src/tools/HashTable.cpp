#include <assert.h>
#include <fstream>
#include <nmmintrin.h>
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
    char data_[32] = {};

    friend std::ostream& operator<< (std::ostream &out, const StringAVX &str) {
        std::cout << "str = " << str.data_;
        return out;
    }
};

// ------------------------------------------------------------------

// ----------------------------HashFunction--------------------------

static size_t HashZero(const char* val) {
    assert(val);
    return 0;
}

static size_t HashOneASCII(const char* val) {
    assert(val);
    return val[0];
}

static size_t HashSumASCII(const char* val) {
    assert(val);
    size_t hash = 0;
    for (size_t i = 0; i < 32; ++i) {
        hash += val[i];
    }
    return hash;
}

static size_t HashLenght(const char* val) {
    assert(val);
    size_t len = 0;
    for ( ; len < 32; ++len) {
        if (val[len] == 0) break;
    }
    return len;
}

static size_t HashSumRoll(const char* val) {
    assert(val);
    size_t hash = val[0];
    for (size_t i = 0; i < 32 && val[i]; ++i) {
        hash = ((hash >> 1) | (hash << 63)) ^ val[i];
    }
    return hash;
}

static size_t HashPolynom(const char* val) {
    assert(val);
    const size_t kPower = 127;
    size_t hash = 0;
    for (size_t i = 0; i < 32; ++i) {
        hash = hash * kPower + val[i];
    }
    return hash;
}

static size_t HashCRC32(const char* val) {
    assert(val);
    unsigned __int64 hash = 0;
    for (size_t i = 0; i < 4; ++i) {
        if (*((const unsigned __int64*)val + i) == 0ull) break;
        hash = _mm_crc32_u64(hash, *((const unsigned __int64*)val + i));
    }
    return (size_t)hash;
}

// ------------------------------------------------------------------


// --------------------------HashTable-------------------------------

HashTable::HashTable() : capacity_(0), size_(0), occupancy_(0), HashFunc_(HashSumASCII) {
    data_.ReserveChunk(1);
    capacity_ = ToNearSimple(data_.GetCapacity());
    for (size_t i = 0; i < capacity_; ++i) {
        data_[i] = nullptr;
    }
}

HashTable::HashTable(HashFunction eFunction) : capacity_(0), size_(0), occupancy_(0) {
    data_.ReserveChunk(1);
    capacity_ = ToNearSimple(data_.GetCapacity());
    for (size_t i = 0; i < capacity_; ++i) {
        data_[i] = nullptr;
    }

    switch (eFunction) {
    case HashFunction::HashZero:
        HashFunc_ = HashZero;
        break;

    case HashFunction::HashOneASCII:
        HashFunc_ = HashOneASCII;
        break;
    
    case HashFunction::HashSumASCII:
        HashFunc_ = HashSumASCII;
        break;

    case HashFunction::HashLenght:
        HashFunc_ = HashLenght;
        break;
    
    case HashFunction::HashSumRoll:
        HashFunc_ = HashSumRoll;
        break;

    case HashFunction::HashPolynom:
        HashFunc_ = HashPolynom;
        break;

    case HashFunction::HashCRC32:
        HashFunc_ = HashCRC32;
        break;
    
    default:
        assert(0 && "Hash function not specified");
        break;
    }
}

HashTable::HashTable::HashTable(size_t (*HashFunc)(const char*)) : capacity_(0), size_(0), occupancy_(0), HashFunc_(HashFunc) {
    data_.ReserveChunk(1);
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
    memcpy(strAVX.data_, str, len);

    size_t ind = HashFunc_(strAVX.data_) % capacity_;
    if (data_[ind] == nullptr) data_[ind] = new List<StringAVX>;
    data_[ind]->Insert(strAVX, 0);
    data_[ind]->Insert(std::move(strAVX), 0);
}

void HashTable::Erase(const char* str, size_t len) noexcept {

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
    std::ofstream file("doc/test.csv");

    if (!file.is_open()) {
        std::cerr << "Error: failed open file\n";
        return;
    }
    
    file << "Position; Score\n";
    
    for (size_t i = 0; i < capacity_; ++i) {
        if (data_[i] == nullptr) {
            file << i << "; " << 0 << "\n";
        } else {
            file << i << "; " << data_[i]->Size() << "\n";
        }
    }

    file.close();
    return;
    
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