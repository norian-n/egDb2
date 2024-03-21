// #ifndef EG_BYTE_ARRAY_CPP
// #define EG_BYTE_ARRAY_CPP
#include <iostream>
#include "egByteArray.h"

/*
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
} */

void ByteArrayFromCharStr(const char* str, EgByteArrayType& byteArray) {
    byteArray.dataSize  = strlen(str)+1;
    if (byteArray.allocMode == egHamSliceAlloc) {
        byteArray.theHamSlicer-> getSlice(byteArray.dataSize, byteArray.brickID, byteArray.arrayData);
        memcpy((void*)byteArray.arrayData, (void*) str, byteArray.dataSize);
    } else if (byteArray.allocMode == egSystemHeapAlloc) {
        byteArray.arrayData = new ByteType[byteArray.dataSize];
        memcpy((void*)byteArray.arrayData, (void*) str, byteArray.dataSize);
    } else {
        byteArray.arrayData = (ByteType *) str;
    }
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

void PrintHamSlices(EgHamSlicerType theSlicer) {
    std::cout << "PrintHamSlices hamBricks: " << theSlicer.hamBricks.size() << std::endl;
    for (auto bricsIter : theSlicer.hamBricks) {
        std::cout << std::dec << bricsIter.first
                  << " ID: "                   << bricsIter.second.brickID
                  << " , freeSize: "             << bricsIter.second.freeSize
                  << " , usedSlicesCount: "      << bricsIter.second.usedSlicesCount
                  << " , brickPtr: " << std::hex << (uint64_t) bricsIter.second.brickPtr << std::endl;
    }
    std::cout << "PrintHamSlices hamBricksByFree: " << theSlicer.hamBricksByFree.size() << std::endl;
    for (auto bricsFreeIter : theSlicer.hamBricksByFree) {
        std::cout << std::dec << "freeSize: " << bricsFreeIter.first
                  << " , ID: " << bricsFreeIter.second-> brickID << std::endl;
    }
}

// #endif // EG_BYTE_ARRAY_CPP