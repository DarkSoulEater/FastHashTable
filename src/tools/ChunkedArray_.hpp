#ifndef TOOLS_CHUNKED_ARRAY_HPP_
#define TOOLS_CHUNKED_ARRAY_HPP_

#include <assert.h>
#include <iostream>

static constexpr size_t ToUpDegree2_(size_t digit) {
    size_t ans = 1;
    while (digit > ans) {
        ans <<= 1ull;
    }
    return ans;
}

static constexpr size_t Log2_(size_t digit) {
    size_t i = 0;
    while (digit > (1ull << i)) {
        ++i;
    }
    return i;
}

template<typename T>
class ChunkedArray_ {
    using Chunk = T*;
private:
    Chunk* chunks_;     // Chuks array
    size_t chunk_cnt_;  // Count chunk
    size_t capacity_;       // Total capacity of the structure

    static constexpr size_t kTypeSize = sizeof(T);
    //static constexpr size_t kTypeSizeLog2 = Log2_(kTypeSize);
    static constexpr size_t kChunkSize = 64; //4096;
    static constexpr size_t kChunkSizeLog2 = Log2_(kChunkSize);

public:
    ChunkedArray_() : chunks_(nullptr), chunk_cnt_(0), capacity_(0) {}
    ~ChunkedArray_() {
        for (size_t i = 0; i < chunk_cnt_; ++i) {
            delete[] chunks_[i];
        }
        free(chunks_);
    }

    void ReserveChunk(size_t chunk_cnt) noexcept {
        if (chunk_cnt <= chunk_cnt_) return;
        ReallocateChunk(chunk_cnt);
    }

    void ReserveNewChunk(size_t new_chunk_cnt) noexcept {
        ReallocateChunk(chunk_cnt_ + new_chunk_cnt);
    }

    T& operator[](size_t i) {
        if (chunks_[i >> kChunkSizeLog2] == nullptr) {
            AllocateChunk(i >> kChunkSizeLog2);
            //if (!AllocateChunk(i >> kChunkSizeLog2)) return nullptr;
            // TODO: exeptions
        }
        return chunks_[i >> kChunkSizeLog2][i & ((1ull << kChunkSizeLog2) - 1ull)];
    }

    const T& operator[](size_t i) const {
        if (chunks_[i >> kChunkSizeLog2] == nullptr) {
            AllocateChunk(i >> kChunkSizeLog2);
            //if (!AllocateChunk(i >> kChunkSizeLog2)) return nullptr;
            // TODO: exeptions
        }
        return chunks_[i >> kChunkSizeLog2][i & ((1ull << kChunkSizeLog2) - 1ull)];
    }

    inline size_t GetCapacity() const noexcept { return capacity_; }
    
    inline const size_t& Capacity_() const noexcept { return capacity_; }

    void Dump() const noexcept {
        std::cerr << "ChunkedArray_ DUMP:\n";
        std::cerr << "Type size = " << kTypeSize << "\n";
        //std::cerr << "Type size log2 = " << kTypeSizeLog2 << "\n";
        std::cerr << "Chunk size = " << kChunkSize << "\n";
        std::cerr << "Chunk size log2 = " << kChunkSizeLog2 << "\n\n";

        std::cerr << "Size = " << capacity_ << "\n";
        std::cerr << "Chunk count = " << chunk_cnt_ << "\n";
        if (chunks_) {
            for (size_t i = 0; i < chunk_cnt_; ++i) {
                std::cerr << "chunks[" << i << "](" << (void*)chunks_[i]  << ")";
                if (chunks_[i]) {
                    std::cerr << " = ";
                    for (size_t j = 0; j < kChunkSize; ++j) {
                        if (j % 16 == 0) std::cerr << "\n";
                        std::cerr << " " << chunks_[i][j];
                    }
                }
                std::cerr << "\n\n";
            }
        }
        std::cerr << "--------------END-------------\n\n";
    }

private:
    bool AllocateChunk(size_t i) const noexcept {
        assert(chunks_ != nullptr);

        chunks_[i] = new T[kChunkSize];

        if (chunks_[i] == nullptr) {
            // TODO: exeptions
            return false;
        }

        return true;
    }

    void ReallocateChunk(size_t chunk_cnt) noexcept {
        assert(chunk_cnt_ <= chunk_cnt);

        Chunk* new_chunks = (Chunk*) realloc(chunks_, sizeof(Chunk) * chunk_cnt);
        if (new_chunks == nullptr) {
            // TODO: exeptions
        }

        chunks_ = new_chunks;
        for (size_t i = chunk_cnt_; i < chunk_cnt; ++i) {
            chunks_[i] = nullptr;
        }

        chunk_cnt_ = chunk_cnt; 
        capacity_ = chunk_cnt_ * kChunkSize;
    }
};

#endif // TOOLS_CHUNKED_ARRAY_HPP_