// fixed length dataset size <=> variable length one
#pragma once
// #include <iostream> // debug mode
// #include "egCoreTypes.h" // typedef unsigned char ByteType;

typedef uint64_t StaticLengthType;

const int DATA_CONVERT_MAX_BYTES_COUNT  {10};   // 64 bits to up to 10 bytes 
StaticLengthType    mask7               {0x7f}; // get 7 bits to next byte
ByteType            mask8               {0x80}; // not last byte flag

// convert fixed length dataset size to variable length one to save file space 
uint8_t egConvertStaticToFlex(StaticLengthType staticVal, ByteType* flexibleVal)
{
    StaticLengthType    buf         {0};
    int                 byteCount   {1};

    while (staticVal && (byteCount < (DATA_CONVERT_MAX_BYTES_COUNT+1))) {
        buf = staticVal & mask7; // get 7 bits to next byte
        flexibleVal[byteCount-1] = static_cast<ByteType>(buf);
            
        // std::cout << "staticVal out: " << std::dec << staticVal << std::endl;
        staticVal = staticVal >> 7; // shift static counter to get next 7 bits
        if (staticVal) {
            flexibleVal[byteCount-1] |=  mask8; // not last byte
            // std::cout << "byte out: " << std::hex << (int) flexibleVal[byteCount-1] << std::endl; 
            byteCount++;
        }
        // else // last byte
            // std::cout << "last byte out: " << std::hex << (int) flexibleVal[byteCount-1] << std::endl;   
    }
    return byteCount;
}

// reverse convert variable length dataset size to fixed length one for faster processing
uint8_t egConvertFlexToStatic(ByteType* flexibleVal, StaticLengthType& staticVal)
{
    staticVal = 0;
    StaticLengthType    buf         {0};
    int                 byteCount   {1};

    while ( byteCount < (DATA_CONVERT_MAX_BYTES_COUNT+1) ) {
        buf = static_cast<ByteType> (flexibleVal[byteCount-1]) & mask7; // get 7 bits to next byte
        // std::cout << "byte in: " << hex << (int) flexibleVal[byteCount-1] << endl;        
        staticVal = (buf << 7 * (byteCount-1)) | staticVal;
        // std::cout << "buf in: " << std::dec << buf << std::endl;
        // std::cout << "staticVal in: " << std::dec << staticVal << std::endl;
            // check "continue" bit
        if ( !(flexibleVal[byteCount-1] & mask8) ) // last byte
            break;
        byteCount++;
    }
    return byteCount;
}