#include <stdio.h>
#include <assert.h>
#include <iostream>
#include <fstream>
#include <ctime>
#include <chrono>
#include <string.h>
#include <nmmintrin.h>

#include "Buffer.hpp"
#include "ChunkedArray_.hpp"
#include "HashTable.hpp"
#include "List.hpp"

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

size_t HashCRC32Asm(const char* val) {
    size_t hash = 0;
    __asm__ (".intel_syntax noprefix\n\t"
        //"mov rcx, 25\n\t"
        "mov rcx, 4\n\t"
        "0:\n\t"
        "mov rax, [%[arg_val]]\n\t"
        "crc32 %[ret_val], rax\n\t"
        //"inc %[arg_val]\n\t"
        "add %[arg_val], 8\n\t"
        "loop 0b\n\t"
        "1:\n\t"
        ".att_syntax prefix\n\t"
        : [ret_val]"=S"(hash)
        : [arg_val]"D"(val)
        :"%rax", "%rcx", "cc"
    );
    return hash;
}

struct HashFunc {
    size_t(*f)(const char*);
    const char* path;
};

#define PREF_PATH "doc/stat_exel/stat_hash_"
static const HashFunc AllowedFunction[] = {
    {HashZero,     PREF_PATH"zero.csv"},
    {HashOneASCII, PREF_PATH"one_ascii.csv"},
    {HashSumASCII, PREF_PATH"sum_ascii.csv"},
    {HashLenght,   PREF_PATH"lenght.csv"},
    {HashSumRoll,  PREF_PATH"sum_roll.csv"},
    {HashPolynom,  PREF_PATH"polynom.csv"},
    {HashCRC32,    PREF_PATH"crc32.csv"}
};

// ------------------------------------------------------------------

struct StringView {
    char* data = nullptr;
    size_t size = 0;
};

void TestHashFunctionCSV(size_t (*func)(const char*), const ChunkedArray_<StringView>& views, const char* out_file) {
    assert(func);
    assert(out_file);

    HashTable table(func);

    for (size_t i = 0; i < views.GetCapacity(); ++i) {
        if (views[i].data == nullptr) break;
        table.Insert(views[i].data, views[i].size);
    }

    table.CreateOccupancyStateCSV(out_file, ';');
}

ChunkedArray_<StringView> CreateStringViews(Buffer& buffer, const char* delim) {
    assert(delim);

    char* data = (char*) buffer.GetData();

    ChunkedArray_<StringView> views;
    views.ReserveChunk(1);
    size_t size_views = 0;

    char* pch = strtok(data, delim);
    while (pch != nullptr) {
        if (size_views == views.GetCapacity()) {
            views.ReserveNewChunk(1);
        }

        views[size_views++] = {pch, strlen(pch)};
        pch = strtok(nullptr, delim);
    }

    return views;
}

// Сreates a set of row distribution data in CSV format for all standard hash-function
void CreateHashTablesStatCSV(const char* file_name) {
    assert(file_name);
    Buffer buffer(file_name);
    char* data = (char*) buffer.GetData();
    if (data == nullptr) {
        std::cerr << "It was not possible to read the data from the file\n";
        return;
    }

    ChunkedArray_<StringView> views = CreateStringViews(buffer, " []\n\r\t\,.;=-?!\"\'");
    
    for (size_t i = 0; i < sizeof(AllowedFunction) / sizeof(HashFunc); ++i) {
        TestHashFunctionCSV(AllowedFunction[i].f, views, AllowedFunction[i].path);
    }
}

// ------------------------------------------------------------------

void ForProfilerTest(const char* file_name) {
    assert(file_name);
    Buffer buffer(file_name);
    char* data = (char*) buffer.GetData();
    if (data == nullptr) {
        std::cerr << "It was not possible to read the data from the file\n";
        return;
    }

    ChunkedArray_<StringView> views = CreateStringViews(buffer, " []\n\r\t\,.;=-?!\"\'");
    //HashTable table(HashPolynom);
    //HashTable table(HashCRC32);
    HashTable table(HashCRC32Asm);

    size_t views_size = 0;
    for (size_t& i = views_size; i < views.GetCapacity(); ++i) {
        if (views[i].data == nullptr) break;
        if (views[i].size > 32) {
            std::cout << "Big str: " << views[i].data << " " << views[i].size << "\n";
        } else {
            table.Insert(views[i].data, views[i].size);
        }
    }

    std::cout << "Number of rows in the dataset: " << views_size   << "\n";
    std::cout << "Number of unique rows: "         << table.Size() << "\n";

    for (size_t i = 0; i < 10000000; ++i) {
        //size_t rand_dig = rand() % views_size;
        //table.Find(views[rand_dig].data, views[rand_dig].size);

        const size_t len = 16;
        char src[len] = "1";
        for (size_t j = 0; j < len / 4; j += 4) {
            ((int*)src)[j] = rand() + 1;
        }
        table.Find(src, len);
    }

    //table.CreateOccupancyStateCSV(PREF_PATH"big_text.csv", ';');
}

int main() {
    char text[32] = "dawfwawdwadaw";
    std::cout << (size_t)(text+1) << "\n";
    std::cout << (int)*(char*)(text+2) << "\n";
    //std::cout << HashCRC32Asm(text);
    //return 0;
    //CreateHashTablesStatCSV("assets/text/romeo-i-dzhuletta.txt");
    ForProfilerTest("C:/Users/eleno/C++/FastHashTable/assets/text/big.txt");
    //ForProfilerTest("assets/text/romeo-i-dzhuletta.txt");
    return 0;
}

