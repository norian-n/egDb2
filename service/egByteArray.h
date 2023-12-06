#pragma once
// #include <iostream>
#include "../core/egCoreTypes.h"

class EgByteArrayType {
public:
    bool dynamicDataAlloc   {false}; // support of static strings for testing
    uint64_t  dataSize      {0};
    ByteType* arrayData     {nullptr};

    EgByteArrayType () {}
    EgByteArrayType (uint64_t init_size): 
        dataSize(init_size) 
    { if(init_size) arrayData = new ByteType[init_size]; dynamicDataAlloc = true; }
    ~EgByteArrayType() { /*std::cout << "destr. of "; PrintByteArray(*this);*/ if (dynamicDataAlloc) { delete arrayData; } }

    void reassignDataArray();
};

void ByteArrayFromCharStr(const char* str, EgByteArrayType& byteArray);
template <typename T> void ByteArrayFromType(T&& value, EgByteArrayType& byteArray) {
    // std::cout << " value: " << value << std::endl;
    byteArray.dataSize  = sizeof(T);
    byteArray.reassignDataArray();
    memcpy((void*)byteArray.arrayData, (void*) &value, byteArray.dataSize);
}
void PrintByteArray(EgByteArrayType& bArray, bool isStr = true);