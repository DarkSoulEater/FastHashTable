#ifndef TOOLS_SMALL_LIST_HPP_
#define TOOLS_SMALL_LIST_HPP_

#include <assert.h>
#include <stdlib.h>
#include <ctype.h>
#include <iostream>

const size_t kDefaultSize = 8;

template<typename T>
class SmallList {
private:
    T* data_;
    size_t* next_;
    size_t* prev_;

    size_t capacity_;
    size_t size_;
    size_t head_;
    size_t tail_;
    size_t free_;
public:
    SmallList() 
        : data_((T*)_aligned_malloc(sizeof(T) * kDefaultSize, alignof(T))),
          next_((size_t*)malloc(sizeof(size_t) * kDefaultSize)),
          prev_((size_t*)malloc(sizeof(size_t) * kDefaultSize)),
          capacity_(kDefaultSize),
          size_(0),
          head_(0),
          tail_(0),
          free_(1) {
        if (data_ == nullptr || next_ == nullptr || prev_ == nullptr) {
            _aligned_free(data_);
            free(next_);
            free(prev_);
            perror("Error: Failed allocate list");
            return;
        }

        for (size_t i = 1; i < capacity_; ++i) {
            next_[i] = i + 1;
            prev_[i] = size_t(-1);
        }

        next_[0] = prev_[0] = 0;

        next_[capacity_ - 1] = 0;
    }

    ~SmallList() {
        _aligned_free(data_);
        free(next_);
        free(prev_);
    }

    inline size_t Size() const noexcept {
        return size_;
    }

    void Insert(const T& value, size_t pos) noexcept {
        if (size_ == capacity_) {
            capacity_ *= 2;
            data_ = (T*) _aligned_realloc(data_, sizeof(T) * capacity_, alignof(T));
            next_ = (size_t*) realloc(next_, sizeof(size_t) * capacity_);
            prev_ = (size_t*) realloc(prev_, sizeof(size_t) * capacity_);

            if (data_ == nullptr || next_ == nullptr || prev_ == nullptr) {
                perror("Failed insert: could not allocate memory\n");
                return;
            }

            free_ = size_;

            for (size_t i = size_; i < capacity_; ++i) {
                next_[i] = i + 1;
                prev_[i] = size_t(-1);
            }
            next_[capacity_ - 1] = 0;
        }

        if (prev_[pos] == size_t(-1)) {
            std::cerr << "Failed insert: element not created\n";
            return;
        }

        size_t cur = free_;
        data_[cur] = value;
        assert((size_t)&data_[cur] % alignof(T) == 0);
        free_ = next_[cur];

        next_[cur] = (pos ? next_[pos] : head_);
        prev_[cur] = prev_[pos];

        next_[pos] = (pos ? cur : 0ull);
        prev_[next_[cur]] = (next_[cur] ? cur : 0ull);

        if (pos == 0) head_ = cur;
        if (pos == tail_) tail_ = cur;

        ++size_;
    }

    void Erase() {
        assert(0 && "Not implameted");
    }

    size_t Find(const T& elem) noexcept {
        size_t pos = head_;
        while (pos != 0) {
            if (data_[pos] == elem) return pos;
            pos = next_[pos];
        }
        return pos;
        
        for (size_t i = 1; i < capacity_; ++i) {
            if (prev_[i] != size_t(-1) && data_[i] == elem) {
                return i;
            }
        }
        return 0;
    }
};

#endif // TOOLS_SMALL_LIST_HPP_