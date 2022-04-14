#include <assert.h>
#include <utility>
#include "HashTable.hpp"

struct alignas(32) StringAVX {
    char data_[32];

    friend std::ostream& operator<< (std::ostream &out, const StringAVX &str) {
        std::cout << "str = " << str.data_;
        return out;
    }
};


static size_t Hash_zero(const char* val) {
    return 0;
}

static size_t Hash(const char* val) {
    size_t ans = 0;
    for (size_t i = 0; i < 32; ++i) {
        ans += val[i];
    }
    return ans;
}

HashTable::HashTable() : capacity_(data_.Capacity_()), size_(0), occupancy_(0) {
    data_.ReserveChunk(1);
    for (size_t i = 0; i < capacity_; ++i) {
        data_[i] = nullptr;
    }
    hash_func_ = Hash;
}

HashTable::~HashTable() {

}

void HashTable::Insert(const char* str, size_t len) noexcept {
    assert(str);

    StringAVX strAVX;
    memcpy(strAVX.data_, str, len);

    size_t ind = hash_func_(strAVX.data_) % capacity_;
    if (data_[ind] == nullptr) data_[ind] = new List<StringAVX>;
    data_[ind]->Insert(strAVX, 0);
    //data_[ind]->Insert(std::move(strAVX), 0);
}

void HashTable::Erase(const char* str, size_t len) noexcept {

}

void HashTable::Dump() const noexcept {
    data_.Dump();
}

void HashTable::PrintStatistics() const noexcept {
    for (size_t i = 0; i < capacity_; ++i) {
        if (data_[i] == nullptr) {
            std::cout << i << " = " << 0 << "\n";
        } else {
            std::cout << i << " = " << data_[i]->Size() << "\n";
        }
    }
}