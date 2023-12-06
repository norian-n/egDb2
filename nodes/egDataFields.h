#pragma once
#include <vector>
#include "../service/egByteArray.h"
// #include "../service/egFileType.h"

//  ============================================================================
//          EXTERNAL TYPES
//  ============================================================================
class EgDataNodeLayoutType;
EgByteArrayType egNotFound;
const char* egNotFoundStr = "<Data Not Found>";
//  ============================================================================

const int DATA_CONVERT_MAX_BYTES_COUNT  {10};   // 64 bits to up to 10 bytes 
StaticLengthType    egMask7f               {0x7f}; // get 7 bits to next byte
ByteType            egMask80               {0x80}; // not last byte flag
/*
struct EgDataFieldsType
{
    EgFieldsCountType               fieldsCount {0};
    uint64_t                        dataFieldSizeTmp;
    std::vector<EgByteArrayType*>   dataFields;
};
*/