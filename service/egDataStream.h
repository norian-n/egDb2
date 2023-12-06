#pragma once
#include <cstdint>

typedef uint32_t egMaxStreamSizeType;

class egDataStream {
public:
    egMaxStreamSizeType bufSize  {0};
    egMaxStreamSizeType bufIndex {0};
    uint8_t*   bufData  {nullptr};

    void seek(egMaxStreamSizeType idx) { bufIndex = idx; } 
    inline bool indexOk(egMaxStreamSizeType dataTypeSize) const { return bufIndex + dataTypeSize <= bufSize ;}

    egDataStream(egMaxStreamSizeType buf_size) :
        bufSize(buf_size), bufData(new uint8_t[buf_size]) {}
    ~egDataStream() { delete bufData; }

    template <typename T> egDataStream& operator>>(T&& i) {
        if (indexOk(sizeof(T))) {
            i = reinterpret_cast<T&&>(bufData[bufIndex]);
            bufIndex += sizeof(T);
        }
        return *this;
    }

    template <typename T> egDataStream& operator<< (T&& i) {
        if (indexOk(sizeof(T))) {
            reinterpret_cast<T&&>(bufData[bufIndex]) = i;
            bufIndex += sizeof(T);
        }
        return *this;
    }

    template <typename T> egDataStream& operator<< (const T&& i) {
        if (indexOk(sizeof(T))) {
            reinterpret_cast<T&&>(bufData[bufIndex]) = i;
            bufIndex += sizeof(T);
        }
        return *this;
    }    
};
        // errStatus = indexOk(sizeof(T)) ? Ok : IndexPastEnd; // TODO Debug mode version
        // (errStatus == Ok) {