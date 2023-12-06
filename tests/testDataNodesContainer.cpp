#include "../nodes/egDataNodesContainer.h"
#include <iostream>
#include <cstring>

using namespace std;

string field1 = "111111\0";
const char* field2 = "test some string 2\0";
string field3("just test 3");

inline void addSampleDataNode(EgDataNodesContainerType& container) {
    EgDataNodeType* newNode = new EgDataNodeType(container.dataNodeLayout);
    *newNode << field1;
    *newNode << field2;
    *newNode << field3;
    container << newNode;
    // cout << "addSampleDataNode() : node added" << endl;
}

bool testDataNodeLayout() {
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

    indexSettings.indexFamilyType   = egIntFT;
    indexSettings.indexSizeBits     = 32;
    testLayout.AddIndex("field_1",  indexSettings);

    indexSettings.indexFamilyType   = egHashFT;
    indexSettings.indexSizeBits     = 64;
    indexSettings.hashFunctionID    = 2;
    testLayout.AddIndex("my_field 3",  indexSettings);
    
    testLayout.LocalStoreLayout();
    testLayout.LocalLoadLayout();
    // PrintDataNodeLayout(testLayout);
    return true;
}

bool testDataNodesContainer() {
    EgDataNodesContainerType nodesContainer("testNodes");

    cout << "===== Test NodesContainerType (store 3 generated nodes to testNodes.gdn, delete #2, reload and print) =====" << endl;
    if(nodesContainer.LoadLocalLayout()) {
        cout << "Error: can't open data nodes layout testNodes.dnl" << endl;
        return false;
    }
        // 3 generated nodes
    addSampleDataNode(nodesContainer);
    addSampleDataNode(nodesContainer);
    addSampleDataNode(nodesContainer);
    addSampleDataNode(nodesContainer);
    // PrintDataNodesContainer(nodesContainer);
        // check delete
    nodesContainer.DeleteDataNode(4);
    // cout << endl << "===== After DeleteDataNode(4) =====" << endl;
    // PrintDataNodesContainer(nodesContainer);
        // store to testNodes.gdn, load and print
    nodesContainer.StoreToLocalFile();          // write to file
    // cout << endl << "===== After StoreToLocalFile() =====" << endl;
    nodesContainer.LoadAllLocalFileNodes();         // load back
    // cout << endl << "===== After LoadAllLocalFileNodes() =====" << endl;
    // cout << endl << "===== After loadAllLocalNodes() =====" << endl;
    // PrintDataNodesContainer(nodesContainer);
    // return true;
        // check delete
    nodesContainer.DeleteDataNode(2);
    // EgDataNodeType* updNode = nodesContainer.dataNodes[1];
    // cout << "updNode-> dataNodeID : " << (int) updNode-> dataNodeID << endl;
    nodesContainer.MarkUpdatedDataNode(nodesContainer.dataNodes[1]);
    // cout << endl << "===== After DeleteDataNode(2), UpdateDataNode =====" << endl;
    // PrintDataNodesContainer(nodesContainer);
        // store to testNodes.gdn, load and print
    /*EgDataNodesLocalFileType testLocalFile;
    EgDataNodeType testNextNode;
    testLocalFile.OpenFileToRead("testNodes");
    testLocalFile.getFirstNodeOffset(testNextNode.dataFileOffset);
    while (testNextNode.dataFileOffset) {
        testLocalFile.ReadDataNode(&testNextNode);
        PrintEgDataNodeTypeOffsets(testNextNode);
        testNextNode.dataFileOffset = testNextNode.nextNodeOffset;
    }
    testLocalFile.nodesFile.close();*/

    nodesContainer.StoreToLocalFile();          // write to file
    nodesContainer.LoadAllLocalFileNodes();         // load back
    // cout << endl << "===== After store/load with deleted & updated nodes =====" << endl;
    // PrintDataNodesContainer(nodesContainer);

    /* testLocalFile.OpenFileToRead("testNodes");
    testLocalFile.getFirstNodeOffset(testNextNode.dataFileOffset);
    while (testNextNode.dataFileOffset) {
        testLocalFile.ReadDataNode(&testNextNode);
        PrintEgDataNodeTypeOffsets(testNextNode);
        testNextNode.dataFileOffset = testNextNode.nextNodeOffset;
    }
    testLocalFile.getLastNodeOffset(testNextNode.dataFileOffset);
    cout << "LastNodeOffset : 0x" << hex << testNextNode.dataFileOffset << endl;    
    testLocalFile.nodesFile.close(); */

    int count = nodesContainer.dataNodes.size();
    nodesContainer.clear();
    return (count == 2);
}

int main() {
    std::remove("testNodes.dnl"); // delete layout file
    std::remove("testNodes.gdn"); // delete data nodes file

    testDataNodeLayout(); // create layout file

    if (testDataNodesContainer())
        cout << "PASS" << endl;
    else
        cout << "FAIL" << endl;
}