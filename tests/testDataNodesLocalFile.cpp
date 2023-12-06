
#include <iostream>
#include <cstring>
using namespace std;

// !! compiler option required -D EG_NODE_OFFSETS_DEBUG

// #include "../nodes/egDataNodesType.h"
#include "../nodes/egDataNodesLocalFile.h"

EgDataNodeLayoutType testLayout("testLocalFileNodes");

const int TEST_FIELDS_COUNT  {3}; 
const char* field1 = "testField1\0";
const char* field2 = "test some string 2\0";
const char* field3 = "test3\0";

inline void addSampleDataFields(EgDataNodeType& testDataNode) {
    testDataNode << field1;
    testDataNode << field2;
    testDataNode << field3;
}

bool initDataNodeLayout() {
    EgIndexSettingsType indexSettings;
    EgDataNodeLayoutType testLayout("testNodes");
    // cout << "===== Test DataNodeLayout " << " =====" << endl;

    testLayout.LayoutInitStart();

    testLayout.AddDataFieldName("field_1");
    testLayout.AddDataFieldName("testField 2");
    testLayout.AddDataFieldName("my_field 3");

    testLayout.layoutSettings.useEntryNodes         = true;
    testLayout.layoutSettings.useGUIsettings        = true;
    testLayout.layoutSettings.useLinks              = true;
    testLayout.layoutSettings.useNamedAttributes    = true;
    testLayout.layoutSettings.useVisualSpace        = true;

    testLayout.LayoutInitCommit();
    
    return true;
}


int main()
{
    int nodesCount  {0};

    EgDataNodesLocalFileType testLocalFile;
    initDataNodeLayout();

    EgDataNodeType testDataNode(&testLayout);
    EgDataNodeType testNextNode(&testLayout);
    // EgDataNodeLayoutType testLayout("testLocalFile");
    // testDataNode.dataNodeLayout = &testLayout;

    cout << "===== Test egDataNodeLocalFile (\"" << field1 << "\" " << " \""
         << field2 << "\" " << " \""
         << field3 << "\" " << ") =====" << endl;

    testDataNode.dataNodeID = 0x38383838;   // ID 8888 in the file
    testDataNode.dataFieldsContainer.fieldsCount = TEST_FIELDS_COUNT;
    addSampleDataFields(testDataNode);
    PrintEgDataNodeTypeFields(testDataNode);
        // write
    testLocalFile.OpenFileToUpdate("testLocalFile");
    testLocalFile.WriteDataNode(&testDataNode);
    testLocalFile.nodesFile.close();

    // return 0;

    /* testLocalFile.OpenFileToRead("testLocalFile");
    testLocalFile.nodesFile.seekRead(100500);
    cout << "File fail(): " << testLocalFile.nodesFile.fileStream.fail() 
         << " file eof(): " << testLocalFile.nodesFile.fileStream.eof()<< endl;
    testLocalFile.nodesFile.close();
    return 0; */
        // read nodes
    testDataNode.dataFieldsContainer.dataFields.clear();
    testLocalFile.OpenFileToRead("testLocalFile");
    testLocalFile.getFirstNodeOffset(testNextNode.dataFileOffset);
    while (testNextNode.dataFileOffset) {
        ReadDataNodeAndOffsets(testLocalFile.nodesFile, &testNextNode);
        PrintEgDataNodeTypeOffsets(testNextNode);
        testNextNode.dataFileOffset = testNextNode.nextNodeOffset;
        nodesCount++;
    }
    testLocalFile.nodesFile.close();
    // PrintEgDataNodeTypeFields(testNextNode);

        // delete node
    cout << "===== Test DeleteDataNode() - delete second one if nodes count >= 3 =====" << endl;
    if (nodesCount >= 3)
    {
        testLocalFile.OpenFileToUpdate("testLocalFile");
        testLocalFile.getFirstNodeOffset(testNextNode.dataFileOffset);
        ReadDataNodeAndOffsets(testLocalFile.nodesFile, &testNextNode);

        testDataNode.dataFileOffset = testNextNode.nextNodeOffset;
        ReadDataNodeAndOffsets(testLocalFile.nodesFile, &testDataNode);
        testLocalFile.DeleteDataNode(&testDataNode);
        testLocalFile.nodesFile.close();

        testLocalFile.OpenFileToRead("testLocalFile");
        testLocalFile.getFirstNodeOffset(testNextNode.dataFileOffset);
        while (testNextNode.dataFileOffset)
        {
            ReadDataNodeAndOffsets(testLocalFile.nodesFile, &testNextNode);
            PrintEgDataNodeTypeOffsets(testNextNode);
            testNextNode.dataFileOffset = testNextNode.nextNodeOffset;
        }
    }

    testLocalFile.nodesFile.close();

    // std::cout << "nextNodeOffset: 0x" << hex << testDataNode.nextNodeOffset << std::endl;

    /*uint64_t nodeOffset =  testLocalFile.nodesFile.getFileSize();
    std::cout << "WriteDataNode() nodeOffset: " << nodeOffset << std::endl;
    testLocalFile.nodesFile.fileStream.seekp(nodeOffset);
    testLocalFile.nodesFile.writeInt(1); */

    /* testLocalFile.nodesFile.close();
    testLocalFile.nodesFile.openToUpdate();
    testLocalFile.nodesFile.seekWrite(0); */

    return 0;
}