#pragma once
// #include <iostream>
#include <map>
// #include "egDataFields.h"
// #include "egDataNodeLayout.h"
// #include "../service/egByteArray.h"

// #define EG_NODE_OFFSETS_DEBUG

#include <vector>
#include "../service/egByteArray.h"
#include "../service/egFileType.h"

class EgDataNodeLayoutType;

struct EgDataFieldsType {
    EgFieldsCountType               fieldsCount {0};
    uint64_t                        dataFieldSizeTmp;
    std::vector<EgByteArrayType*>   dataFields;
};

class EgDataNodeType {
public:
    EgDataNodeIDType        dataNodeID      { 0 };
    EgFileOffsetType        dataFileOffset  { 0 };          // stored offsets for local file operations speedup
#ifdef EG_NODE_OFFSETS_DEBUG
    EgFileOffsetType        nextNodeOffset  { 0 };
    EgFileOffsetType        prevNodeOffset  { 0 };
#endif
    EgDataNodeLayoutType*   dataNodeLayout  { nullptr };    // layout == class == type of data nodes
    EgDataFieldsType        dataFieldsContainer;            // vector of egByteArrays

    std::map < EgLayoutIDType, std::vector<EgDataNodeType*> >  inLinks;
    std::map < EgLayoutIDType, std::vector<EgDataNodeType*> >  outLinks;
#ifdef EG_NODE_OFFSETS_DEBUG
    EgDataNodeType() {} // for debug only
#endif    
    EgDataNodeType(EgDataNodeLayoutType* a_dataNodeLayout);
    ~EgDataNodeType() { /*std::cout << "EgDataNodeType destructor, ID = " << dataNodeID << std::endl;*/ clear(); }

    void clear();
    EgByteArrayType& operator[](std::string fieldStrName);
    EgByteArrayType& operator[](const char* fieldCharName);
};

// ======================== DataFields ========================

void AddNextDataFieldFromCharStr(const char* str, EgDataNodeType& theNode);
// template <typename T> void AddNextDataFieldFromType(T&& value, EgDataNodeType& theNode);
template <typename T> void AddNextDataFieldFromType(T&& value, EgDataNodeType& theNode) {
    EgByteArrayType* byteArray = new EgByteArrayType();
    ByteArrayFromType<T> (value, *byteArray);
    theNode.dataFieldsContainer.dataFields.push_back(byteArray);    
}
// convert fixed length dataset size to variable length one to save file space 
uint8_t egConvertStaticToFlex(StaticLengthType staticVal, ByteType* flexibleVal);

// reverse convert variable length dataset size to fixed length one for faster processing
uint8_t egConvertFlexToStatic(ByteType* flexibleVal, StaticLengthType& staticVal);

// file operations
void writeDataFieldsToFile(EgDataFieldsType& df, EgFileType &theFile);
void readDataFieldsFromFile(EgDataFieldsType& df, EgFileType& theFile);

// ======================== Debug ========================

void PrintEgDataNodeTypeOffsets(const EgDataNodeType& dataNode);
void PrintEgDataNodeTypeFields (const EgDataNodeType& dataNode);

// ===================== Operators =======================

EgDataNodeType& operator << (EgDataNodeType& egNode, const char* str); // { AddNextDataFieldFromCharStr(str, egNode); return egNode; }
EgDataNodeType& operator << (EgDataNodeType& egNode, std::string& s); //  { AddNextDataFieldFromCharStr(s.c_str(), egNode); return egNode; }

template <typename T> EgDataNodeType& operator << (EgDataNodeType& egNode, T&& i) { AddNextDataFieldFromType<T>(i, egNode); return egNode; }
