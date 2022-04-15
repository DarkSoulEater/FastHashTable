#include <stdio.h>
#include <assert.h>
#include <iostream>
#include <fstream>
#include <ctime>
#include <chrono>
#include <string.h>
#include <assert.h>

#include "Buffer.hpp"
#include "ChunkedArray_.hpp"
#include "HashTable.hpp"
#include "List.hpp"

// Сreates a set of row distribution data in CSV format for all standard hash-function
void CreateHashTableStatCSV(const char* file_name) {
    Buffer buffer(file_name);
    char* data = (char*) buffer.GetData();
    if (data == nullptr) {
        std::cerr << "It was not possible to read the data from the file\n";
        return;
    }

    HashTable tables[] = {
        HashTable(HashFunction::HashZero),
        HashTable(HashFunction::HashOneASCII),
        HashTable(HashFunction::HashSumASCII),
        HashTable(HashFunction::HashLenght),
        HashTable(HashFunction::HashSumRoll),
        HashTable(HashFunction::HashPolynom),
        HashTable(HashFunction::HashCRC32)
    };
    
    for (size_t len = 0; *data != '\0'; ++len) {
        if (std::ispunct(data[len]) || std::isspace(data[len])) {
            if (len > 0) {
                for (size_t i = 0; i < sizeof(tables) / sizeof(HashTable); ++i) {
                    tables[i].Insert(data, len);
                }
            }

            data += len + 1;
            len = -1; // All OK
        }
    }

    tables[0].CreateOccupancyStateCSV("doc/stat_hash_zero.csv", ';');
    tables[1].CreateOccupancyStateCSV("doc/stat_hash_one_ascii.csv", ';');
    tables[2].CreateOccupancyStateCSV("doc/stat_hash_sum_ascii.csv", ';');
    tables[3].CreateOccupancyStateCSV("doc/stat_hash_lenght.csv", ';');
    tables[4].CreateOccupancyStateCSV("doc/stat_hash_sum_roll.csv", ';');
    tables[5].CreateOccupancyStateCSV("doc/stat_hash_polynom.csv", ';');
    tables[6].CreateOccupancyStateCSV("doc/stat_hash_crc32.csv", ';');
}

static size_t HashSumRoll(const char* val) {
    assert(val);
    size_t ans = val[0];
    for (size_t i = 0; i < 32 && val[i]; ++i) {
        ans = ((ans >> 1) | (ans << 63)) ^ val[i];
    }
    return ans;
}

uint64_t rollHashCode(char* str) {
  uint64_t hash = str[0];
  while (*str) {
      hash = ((hash >> 1) | (hash << 63)) ^ *(str++);
  }
  return hash;
}

int main() {
    CreateHashTableStatCSV("assets/text/romeo-i-dzhuletta.txt");
/*
    HashTable ht;
    std::string s;
    size_t cnt = 0;
    for (file >> s; !file.eof(); file >> s, cnt++) {
        if (s.size() <= 32) {
            ht.Insert(s.data(), s.size());
        } else {
            std::cout << s << " " << cnt << " - Size > 32\n";
        }
    }
    std::cout << "Count = " << cnt << "\n";

    ht.PrintStatistics();
    //ht.Dump();
    //int a = 15;
    //int b = 10;
    //std::cout << &b << "\n";
    //b = std::move(a);
    //std::cout << &b << "\n";
    //return 0;
    //List<int> k;
    //std::cout << sizeof(String) << "\n\n";

    /*ChunkedArray_<int> arr;
    arr.Dump();
    arr.ReserveChunk(2);
    for (size_t i = 0; i < 100000; ++i) {
        const char* t = "312";
        arr[i % 100] = i;
        //memcpy(&arr[i].str[0], t, strlen(t) + 1);
        //assert((size_t)arr[i].str % 32 == 0);
        //arr[i].str[0] = '1';
    }
    arr[0];

    //std::cout << arr[2];
    arr.Dump();/*/

    /*const char* str = "0123456789ABCDEF";

    List<int> list;
    for (size_t i = 0; i < 100; ++i) {
        list.Insert(i, i);
    }

    list.Dump();

    return 0;
    /*const size_t kSize = 1000000;
    const size_t kN = 1000000;
    void* buf1 = malloc(kSize);
    void* buf2 = malloc(kSize);

    std::chrono::high_resolution_clock::time_point now_time_ = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < kN * 0; ++i) {
        memcpy(buf1, buf2, kSize);
        ++*(int*)buf2;
    }
    std::chrono::high_resolution_clock::time_point time = std::chrono::high_resolution_clock::now();
    auto delta_time_ = std::chrono::duration_cast<std::chrono::nanoseconds>(time - now_time_).count();
    std::cout << "memcpy = " << delta_time_<< "\n";

    now_time_ = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < kN; ++i) {
        Memcpy(buf1, buf2, kSize);
        ++*(int*)buf2;
    }
    time = std::chrono::high_resolution_clock::now();
    delta_time_ = std::chrono::duration_cast<std::chrono::nanoseconds>(time - now_time_).count();
    std::cout << "Memcpy = " << delta_time_ << "\n";

    std::cout << ++*(int*)buf1;
    return 0;

/*
    PageArray_ arr(1, 16, 16);
    arr.ReservePage(2);
    arr.Dump(Print);
    for (size_t i = 0; i < 64; ++i) {
        char* ptr = (char*)arr.At(i);
        if (ptr) *ptr = rand() % 20 + 'A';
    }
    arr.Dump(Print);
    return 0;*/
}