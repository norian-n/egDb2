// #ifndef EG_BYTE_ARRAY_CPP
// #define EG_BYTE_ARRAY_CPP
#include <iostream>
#include "egByteArray.h"

void EgByteArrayType::reassignDataArray() {
    // std::cout << "reassign dynamicDataAlloc: " << dynamicDataAlloc << std::endl;
    if (dynamicDataAlloc)
        delete arrayData;
    if (dataSize) {
        arrayData = new ByteType[dataSize]; // FIXME TODO paged allocator
        dynamicDataAlloc = true;
    } else {
        arrayData = nullptr;
        dynamicDataAlloc = false;
    }
}

void ByteArrayFromCharStr(const char* str, EgByteArrayType& byteArray) {
    byteArray.dataSize  = strlen(str)+1;
    byteArray.reassignDataArray();
    memcpy((void*)byteArray.arrayData, (void*) str, byteArray.dataSize);
}
/*
template <typename T> void ByteArrayFromType(T&& value, EgByteArrayType& byteArray) {
    // std::cout << " value: " << value << std::endl;
    byteArray.dataSize  = sizeof(T);
    byteArray.reassignDataArray();
    memcpy((void*)byteArray.arrayData, (void*) &value, byteArray.dataSize);
}
*/
void PrintByteArray(EgByteArrayType& bArray, bool isStr) {
    std::cout << " size: " << std::dec << bArray.dataSize;
    if (bArray.dataSize) {
        // if ((bArray.dataSize > 1) && !bArray.arrayData[bArray.dataSize - 1] && bArray.arrayData[0] > 0x29)
        if (isStr)
            std::cout << " Str: \"" << (char *)bArray.arrayData << "\"";
        else
            std::cout << " Int: " << (int)*(bArray.arrayData);
        std::cout << " Hex: " << std::hex;
        for (int i = 0; i < bArray.dataSize; i++)
            // std::cout << " \"" << std::hex << (int) field.arrayData[i] << "\"";
            std::cout << (int)bArray.arrayData[i] << " ";
        std::cout << std::endl;
    }
}

// #endif // EG_BYTE_ARRAY_CPP