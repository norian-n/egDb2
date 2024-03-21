#pragma once
// #include <iostream>
#include "../core/egCoreTypes.h"
#include "egHamSlicer.h"

enum EgByteArrayModeEnum
{
    egNoAlloc,          // external memory provided
    egHamSliceAlloc,    // by egHamSlicer.h
    egSystemHeapAlloc   // usual heap ops
};

class EgByteArrayType {
public:
    EgHamSlicerType* theHamSlicer   {nullptr};
    EgByteArrayModeEnum allocMode   {egNoAlloc};
    EgHamBrickIDType    brickID     {0};
    // bool dynamicDataAlloc   {false}; // support of static strings for testing
    uint64_t  dataSize      {0};
    ByteType* arrayData     {nullptr};

    EgByteArrayType () {} // allocMode == egNoAlloc
    EgByteArrayType (EgHamSlicerType* a_HamSlicer, uint64_t init_size): 
        theHamSlicer(a_HamSlicer), 
        allocMode(egHamSliceAlloc),
        dataSize(init_size)
    { if(init_size) theHamSlicer-> getSlice(dataSize, brickID, arrayData); }
    EgByteArrayType (uint64_t init_size): 
        dataSize(init_size)
    { if(init_size) arrayData = new ByteType[init_size]; allocMode = egSystemHeapAlloc; }
    ~EgByteArrayType() { /*std::cout << "destr. of "; PrintByteArray(*this);*/ 
        if (allocMode == egSystemHeapAlloc) delete arrayData;
        else if (allocMode == egHamSliceAlloc) theHamSlicer-> freeSlice(brickID);
    }

    // void reassignDataArray();
};

void ByteArrayFromCharStr(const char* str, EgByteArrayType& byteArray);
template <typename T> void ByteArrayFromType(T&& value, EgByteArrayType& byteArray) {
    // std::cout << " value: " << value << std::endl;
    byteArray.dataSize  = sizeof(T);
    if (byteArray.allocMode == egHamSliceAlloc) {
        byteArray.theHamSlicer-> getSlice(byteArray.dataSize, byteArray.brickID, byteArray.arrayData);
        memcpy((void*)byteArray.arrayData, (void*) &value, byteArray.dataSize);
    } else if (byteArray.allocMode == egSystemHeapAlloc) {
        byteArray.arrayData = new ByteType[byteArray.dataSize];
        memcpy((void*)byteArray.arrayData, (void*) &value, byteArray.dataSize);
    } else {
        byteArray.arrayData = (ByteType *) &value;
    }
}
void PrintByteArray(EgByteArrayType& bArray, bool isStr = true);