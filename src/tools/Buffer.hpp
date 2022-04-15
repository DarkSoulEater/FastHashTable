#ifndef TOOLS_BUFFER_HPP_
#define TOOLS_BUFFER_HPP_

#include <assert.h>

class Buffer {
private:
    void* data_;
    size_t size_;

public:
    Buffer(size_t size); // Size in bytes
    Buffer(size_t cnt_elem, size_t sizeof_elem);
    Buffer(const char* file_name); // Reads all data from the file to the buffer
    ~Buffer();

    inline void* GetData() { return data_; }

private:
};


#endif // TOOLS_BUFFER_HPP_