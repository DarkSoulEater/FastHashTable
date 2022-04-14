#include <stdio.h>
#include <assert.h>
#include <iostream>
#include <fstream>
#include <ctime>
#include <chrono>
#include <string.h>
#include <assert.h>

#include "ChunkedArray_.hpp"
#include "HashTable.hpp"
#include "List.hpp"

static size_t Logi2(size_t digit) {
    assert(digit != 0 && "The number must be an exact power of two");

    size_t i = 0;
    while (!(digit & 1)) {
        ++i;
        digit >>= 1;
    }

    assert(digit == 1 && "The number must be an exact power of two");

    return i;
}

inline void Memcpy(void* dst, const void* src, size_t size) {
    while (size > sizeof(size_t)) {
        *(size_t*)dst = *(const size_t*)src;
        size -= sizeof(size_t);
    }
    
    while (size > sizeof(unsigned)) {
        *(unsigned*)dst = *(const unsigned*)src;
        size -= sizeof(unsigned);
    }
    
    while (size > sizeof(char)) {
        *(char*)dst = *(const char*)src;
        size -= sizeof(char);
    }
}

struct alignas(32) String {
    char str[16] = "123123";
    //int a;

    //String() : str("1231") {}

    friend std::ostream& operator<< (std::ostream &out, const String &str) {
        out << str.str;
        //out << "OK";
        return out;
    }
};


int main() {
    // std::ifstream file("assets/text/0.txt");
    std::ifstream file("C:/Users/eleno/C++/ToolsLib/assets/text/romeo-i-dzhuletta.txt");
    if (!file.is_open()) {
        return 0;
    }

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