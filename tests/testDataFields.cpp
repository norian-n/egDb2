#include "../nodes/egDataNode.h"

#include <iostream>
#include <cstring>
using namespace std;

const int TEST_FIELDS_COUNT  {3}; 
const string fileName("test_data_fields.tdf");
const char* field1 = "testField1\0";
const char* field2 = "test some string 2\0";
const char* field3 = "test3\0";

const int DATA_CONVERT_MAX_BYTES_COUNT_COPY  {10}; 

ByteType            lengthRawData[DATA_CONVERT_MAX_BYTES_COUNT_COPY];
StaticLengthType    inputLength {123456789};
StaticLengthType    testLength;

EgFileType          testFile(fileName);
EgDataFieldsType    testDataFields;

void addSampleDataFields()
{
    testDataFields.fieldsCount = TEST_FIELDS_COUNT;
    EgByteArrayType* byteArray = new EgByteArrayType();

    byteArray-> arrayData = (ByteType*) field1;
    byteArray-> dataSize  = strlen(field1)+1;
    testDataFields.dataFields.push_back(byteArray);

    byteArray = new EgByteArrayType();
    byteArray-> arrayData = (ByteType*) field2;
    byteArray-> dataSize  = strlen(field2)+1;
    testDataFields.dataFields.push_back(byteArray);

    byteArray = new EgByteArrayType();
    byteArray-> arrayData = (ByteType*) field3;
    byteArray-> dataSize  = strlen(field3)+1;
    testDataFields.dataFields.push_back(byteArray);
}

int main()
{
    std::remove(fileName.c_str()); // delete file
    addSampleDataFields();

    cout << "===== Test length convertor ( " << inputLength << " ) =====" << endl;

    int byteCountIn  = egConvertStaticToFlex(inputLength, lengthRawData); // up to 0xffffffffffffffff
    int byteCountOut = egConvertFlexToStatic(lengthRawData, testLength);

    cout << "byteCountIn: " << byteCountIn << " byteCountOut: " << byteCountOut << " testLength: " << testLength << endl;

    cout << "===== Test egDataFields (" << field1 << " " << strlen(field1) << " " 
                                                << field2 << " " << strlen(field2) << " "
                                                << field3 << " " << strlen(field3) <<  ") =====" << endl;

    testFile.openToWrite();
    writeDataFieldsToFile(testDataFields, testFile);
    testFile.close();

    testFile.openToRead();
    readDataFieldsFromFile(testDataFields, testFile);
    testFile.close();   

    for (const auto& field : testDataFields.dataFields)
        PrintByteArray(*field) ;

    // cout << endl << endl;

    testDataFields.dataFields.clear();

    // cout << "===== Tests end =====" << endl << endl;

    return 0;
}
