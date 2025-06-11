#include <iostream>
#include "egDataNode.h"
#include "egDataNodeLayout.h"

//  ============================================================================
EgByteArrayType egNotFound(0);
const char* egNotFoundStr = "<Data Not Found>";

const int DATA_CONVERT_MAX_BYTES_COUNT      {10};   // 64 bits to up to 10 bytes 
StaticLengthType    egMask7f                {0x7f}; // get 7 bits to next byte
ByteType            egMask80                {0x80}; // not last byte flag
//  ============================================================================


EgDataNodeType::EgDataNodeType(EgDataNodeLayoutType* a_dataNodeLayout):
    dataNodeLayout(a_dataNodeLayout) 
{
    if (a_dataNodeLayout) { 
        // dataFieldsContainer.fieldsCount = a_dataNodeLayout-> dataFieldsNames.size();
        dataFieldsContainer.dataFields.reserve(a_dataNodeLayout-> fieldsCount);
    }
    ByteArrayFromCharStr(egNotFoundStr, egNotFound); // FIXME STUB move to database level
}

void EgDataNodeType::clear() {
    for (auto fieldsIter : dataFieldsContainer.dataFields) // 17 [first, second], <11 = dataFieldsNames.begin(); fieldsIter != dataFieldsNames.end(); ++fieldsIter) {
        delete fieldsIter;
    dataFieldsContainer.dataFields.clear();
}

EgByteArrayType& EgDataNodeType::operator[](std::string fieldStrName) { // field value by name as stg::string
    if (dataNodeLayout) {
        auto iter = dataNodeLayout->dataFieldsNames.find(fieldStrName);
        if (iter != dataNodeLayout->dataFieldsNames.end())
            return *dataFieldsContainer.dataFields[iter->second];
    }
    return egNotFound;
}

EgByteArrayType& EgDataNodeType::operator[](const char *fieldCharName) { // field value by name as char* literal
    if (dataNodeLayout) {
        auto iter = dataNodeLayout->dataFieldsNames.find(std::string(fieldCharName));
        if (iter != dataNodeLayout->dataFieldsNames.end())
            return *dataFieldsContainer.dataFields[iter->second];
    }
    return egNotFound;
}

// ======================== Debug ========================

void PrintEgDataNodeTypeOffsets(const EgDataNodeType& dataNode){
    std::cout << "NodeID: " << std::dec << dataNode.dataNodeID
              << ", nodeOffset: 0x" << std::hex << dataNode.dataFileOffset
#ifdef EG_NODE_OFFSETS_DEBUG
              << ", next: 0x" << dataNode.nextNodeOffset
              << ", prev: 0x" << dataNode.prevNodeOffset 
#endif
              << std::endl;
}

void PrintEgDataNodeTypeFields(const EgDataNodeType& dataNode){
    std::cout << "NodeID: " << std::dec << dataNode.dataNodeID << " Fields: " << std::endl;
    for (const auto &field : dataNode.dataFieldsContainer.dataFields)
        PrintByteArray(*field);
}

// ======================== DataFields ========================

void AddNextDataFieldFromCharStr(const char* str, EgDataNodeType& theNode) {
    // EgByteArrayType* byteArray = new EgByteArrayType();
    // ByteArrayFromCharStr(str, *byteArray);
    // std::cout << "AddNextDataFieldFromCharStr() in: " << str << std::endl;
    EgByteArrayType* byteArray = new EgByteArrayType(&(theNode.dataNodeLayout-> theHamSlicer), strlen(str)+1);  // use ham slicer allocator
    // EgByteArrayType* byteArray = new EgByteArrayType(strlen(str)+1);  // use ham slicer allocator
    memcpy((void*)byteArray-> arrayData, (void*) str, byteArray-> dataSize);
    theNode.dataFieldsContainer.dataFields.push_back(byteArray);
}

void AddNextDataFieldFromByteArray(EgByteArrayType& ba, EgDataNodeType& theNode) {
    EgByteArrayType* byteArray = new EgByteArrayType(&(theNode.dataNodeLayout-> theHamSlicer), ba.dataSize);  // use ham slicer allocator
    // EgByteArrayType* byteArray = new EgByteArrayType(strlen(str)+1);
    memcpy((void*)byteArray-> arrayData, (void*) ba.arrayData, byteArray-> dataSize);
    theNode.dataFieldsContainer.dataFields.push_back(byteArray);
}

/*
template <typename T> void AddNextDataFieldFromType(T&& value, EgDataNodeType& theNode) {
    // EgByteArrayType* byteArray = new EgByteArrayType();
    EgByteArrayType* byteArray = new EgByteArrayType(&(theNode.dataNodeLayout-> theHamSlicer), 0);  // use ham slicer allocator
    ByteArrayFromType<T> (value, *byteArray);
    theNode.dataFieldsContainer.dataFields.push_back(byteArray);    
} */

/*
template <typename T> void AddNextDataFieldFromType(T&& value, EgDataNodeType& theNode) {
    EgByteArrayType* byteArray = new EgByteArrayType();
    ByteArrayFromType<T> (value, *byteArray);
    theNode.dataFieldsContainer.dataFields.push_back(byteArray);    
}
*/
// convert fixed length dataset size to variable length one to save file space 
uint8_t egConvertStaticToFlex(StaticLengthType staticVal, ByteType* flexibleVal)
{
    StaticLengthType    buf         {0};
    int                 byteCount   {1};

    while (staticVal && (byteCount < (DATA_CONVERT_MAX_BYTES_COUNT+1))) {
        buf = staticVal & egMask7f; // get 7 bits to next byte
        flexibleVal[byteCount-1] = static_cast<ByteType>(buf);
            
        // std::cout << "staticVal out: " << std::dec << staticVal << std::endl;
        staticVal = staticVal >> 7; // shift static counter to get next 7 bits
        if (staticVal) {
            flexibleVal[byteCount-1] |=  egMask80; // not last byte
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
        buf = static_cast<ByteType> (flexibleVal[byteCount-1]) & egMask7f; // get 7 bits to next byte
        // std::cout << "byte in: " << hex << (int) flexibleVal[byteCount-1] << endl;        
        staticVal = (buf << 7 * (byteCount-1)) | staticVal;
        // std::cout << "buf in: " << std::dec << buf << std::endl;
        // std::cout << "staticVal in: " << std::dec << staticVal << std::endl;
            // check "continue" bit
        if ( !(flexibleVal[byteCount-1] & egMask80) ) // last byte
            break;
        byteCount++;
    }
    return byteCount;
}

void EgDataNodeType::writeDataFieldsToFile(EgDataFieldsType& df, EgFileType &theFile) {
    ByteType lengthRawData[DATA_CONVERT_MAX_BYTES_COUNT]; // flex size buffer, see egDataConvert.h
    // std::cout << "writeDataFieldsToFile() start, " << std::dec << df.dataFields.size() << std::endl;
    std::vector<EgByteArrayType*>::iterator field;
    // for (const auto &field : df.dataFields) {
    for(field = df.dataFields.begin(); field != df.dataFields.end(); ++field ) {
        if ( !(*field)) {
            std::cout  << "DEBUG: writeDataFieldsToFile() field ptr is NULL " << std::endl;
            return;
        } else {
            // PrintByteArray(*field);
            uint8_t lenSize = egConvertStaticToFlex((*field)->dataSize, lengthRawData);
            // std::cout << "lenSize: " << (int) lenSize << " lengthRawData[0]: " << (int) lengthRawData[0] << std::endl;
            theFile.fileStream.write((const char *)lengthRawData, lenSize);                    // write size
            theFile.fileStream.write((const char *)((*field)->arrayData), (*field)->dataSize); // write data
        }
    }
    // std::cout << "writeDataFieldsToFile() exit " << std::endl;
    theFile.fileStream.flush();
}

void EgDataNodeType::readDataFieldsFromFile(EgDataFieldsType& df, EgFileType& theFile) {
    ByteType lengthRawData[DATA_CONVERT_MAX_BYTES_COUNT]; // flex size buffer, see egDataConvert.h
    EgByteArrayType* newField;
    df.dataFields.clear();
    df.dataFields.resize(dataNodeLayout-> fieldsCount);
    // std::cout << "readDataFieldsFromFile() fieldsCount: " << std::dec << (int) dataNodeLayout-> fieldsCount << std::endl;
    for (EgFieldsCountType i = 0; i < dataNodeLayout-> fieldsCount; i++) {
        uint64_t savePos = static_cast<uint64_t>(theFile.fileStream.tellg());
        uint64_t fileTailSize = theFile.getFileSize() - savePos;
        theFile.seekRead(savePos);
        theFile.fileStream.read((char *)lengthRawData, std::min((uint64_t)DATA_CONVERT_MAX_BYTES_COUNT, fileTailSize)); // read size
        uint64_t dataFieldSizeTmp;
        uint8_t lenSize = egConvertFlexToStatic(lengthRawData, dataFieldSizeTmp);
        // std::cout << "newField.dataSize: " << std::dec << (int) df.dataSize;
        newField = new EgByteArrayType(dataFieldSizeTmp); // +1
        theFile.seekRead(savePos + lenSize);
        theFile.fileStream.read((char *)(newField->arrayData), dataFieldSizeTmp); // read data
        // newField->arrayData[dataFieldSizeTmp] = 0;
        // std::cout << " newField.arrayData: " << (char *)(newField->arrayData) << std::endl;
        df.dataFields[i] = newField;
    }
}

// ===================== Operators =======================

EgDataNodeType& operator << (EgDataNodeType& egNode, const char* str) { AddNextDataFieldFromCharStr(str, egNode); return egNode; }
EgDataNodeType& operator << (EgDataNodeType& egNode, std::string& s)  { AddNextDataFieldFromCharStr(s.c_str(), egNode); return egNode; }
EgDataNodeType& operator << (EgDataNodeType& egNode, EgByteArrayType& ba) { AddNextDataFieldFromByteArray(ba, egNode); return egNode; }

// template <typename T> EgDataNodeType& operator << (EgDataNodeType& egNode, T&& i) { AddNextDataFieldFromType<T>(i, egNode); return egNode; }

