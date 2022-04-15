#ifndef TOOLS_LIST_HPP
#define TOOLS_LIST_HPP

#include <inttypes.h>
#include "ChunkedArray_.hpp"

// Prev not implementation
template<typename T>
class List{
private:
    ChunkedArray_<T> data_;
    ChunkedArray_<size_t> next_;
    ChunkedArray_<size_t> prev_;

    const size_t& capacity_;
    size_t size_;
    size_t head_;
    size_t tail_;
    size_t free_;

public:
    List() : size_(0), capacity_(data_.Capacity_()), head_(0), tail_(0), free_(1) {
        data_.ReserveChunk(1);
        next_.ReserveChunk(1);
        prev_.ReserveChunk(1);

        next_[0] = prev_[0] = 0ull;

        for (size_t i = 1; i < capacity_; ++i) {
            next_[i] = i + 1;
            prev_[i] = size_t(-1);
        }
        next_[capacity_ - 1] = 0;
    }

    ~List() {}


    inline size_t Size() const noexcept {
        return size_;
    }

    void Insert(const T& val, size_t pos) noexcept {
        if (size_ == capacity_) {
            data_.ReserveNewChunk(1);
            next_.ReserveNewChunk(1);
            prev_.ReserveNewChunk(1);

            free_ = size_;
            
            for (size_t i = size_; i < capacity_; ++i) {
                next_[i] = i + 1;
                prev_[i] = size_t(-1);
            }
            next_[capacity_ - 1] = 0;
        }

        if (prev_[pos] == size_t(-1)) {
            std::cerr << "FAIL\n";
            return; // TODO:
        }

        size_t cur = free_;
        data_[cur] = val;
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

    /*void Insert(const T&& val, size_t pos) noexcept { // TODO:
        if (size_ == capacity_) {
            data_.ReserveNewChunk(1);
            next_.ReserveNewChunk(1);
            prev_.ReserveNewChunk(1);

            free_ = size_;
            
            for (size_t i = size_; i < capacity_; ++i) {
                next_[i] = i + 1;
                prev_[i] = size_t(-1);
            }
            next_[capacity_ - 1] = 0;
        }

        if (prev_[pos] == size_t(-1)) {
            std::cerr << "FAIL\n";
            return; // TODO:
        }

        size_t cur = free_;
        data_[cur] = val;
        assert((size_t)&data_[cur] % alignof(T) == 0);
        free_ = next_[cur];

        next_[cur] = (pos ? next_[pos] : head_);
        prev_[cur] = prev_[pos];

        next_[pos] = (pos ? cur : 0ull);
        prev_[next_[cur]] = (next_[cur] ? cur : 0ull);

        if (pos == 0) head_ = cur;
        if (pos == tail_) tail_ = cur;

        ++size_;
    }*/

    void Erase(size_t pos) noexcept {}

    int Find(const T& elem) const noexcept {
        for (size_t i = 1; i < capacity_; ++i) {
            if (data_[i] == elem) return i;
        }
        return 0;
    }

    void Dump() const noexcept {
        std::cerr << "List DUMP:\n";
        std::cerr << "Capacity = " << capacity_ << "\n";
        std::cerr << "Size = " << size_ << "\n";
        std::cerr << "Head = " << head_ << "\n";
        std::cerr << "Tail = " << tail_ << "\n";
        std::cerr << "Free = " << free_ << "\n\n";

        std::cerr << "DATA:\n";
        data_.Dump();

        std::cerr << "NEXT:\n";
        next_.Dump();

        std::cerr << "PREV:\n";
        prev_.Dump();
    }

    friend std::ostream& operator<< (std::ostream &out, const List &list) {
        list.Dump();
        return out;
    }

private:

};

#endif // TOOLS_LIST_HPP