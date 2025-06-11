#include "../metainfo/egDatabaseType.h"
#include <iostream>
#include <cstring>

using namespace std;

bool initDatabase(EgDatabaseType& graphDB) {
    // EgNodeTypeSettings typeSettings;
    // typeSettings.useLocation = true;
    // typeSettings.useLinks = true;

    graphDB.CreateNodeLayoutBlueprintClass("locations"); // , typeSettings);

    graphDB.AddDataField("name");
    graphDB.AddDataField("status"); // , isIndexed create index
    graphDB.AddDataField("imageType");
    graphDB.AddDataField("x");
    graphDB.AddDataField("y");    

    graphDB.CommitNodeLayoutBlueprintClass();

    graphDB.AddLinkType("linktype"); // , "locations", "locations"); // create link type

    return ((graphDB.nodesTypesStorage-> dataNodes.size() == 1) 
        &&  (graphDB.linksTypesStorage-> dataNodes.size() == 1));
}

string field1 = "name\0";
const char* field2 = "status\0";
string field3("image type");
string field4("100");
string field5("200");
int num1 = 100;
int num2 = 200;

inline void addSampleDataNode(EgDataNodesType& dataNodes) {
    EgDataNodeType* newNode = new EgDataNodeType(dataNodes.dataNodeLayout);
    *newNode << field1;
    *newNode << field2;
    *newNode << field3;
    *newNode << num1;
    *newNode << num2;
    // dataNodes.AddDataNode(newNode);
    dataNodes << newNode;
}

bool testEgDataNodesTypeBasicNodeOps(EgDataNodesType& testDataNodes)
{
    addSampleDataNode(testDataNodes);
    addSampleDataNode(testDataNodes);
    addSampleDataNode(testDataNodes);
    
    addSampleDataNode(testDataNodes);
    addSampleDataNode(testDataNodes);
    addSampleDataNode(testDataNodes);
    
    addSampleDataNode(testDataNodes);
    addSampleDataNode(testDataNodes);
    addSampleDataNode(testDataNodes);

    testDataNodes.DeleteDataNode(2);

    testDataNodes.MarkUpdatedDataNode(&(testDataNodes[3]));
/*
    cout << "dataNodes: " << testDataNodes.nodesContainer-> dataNodes.size() << endl;
    cout << "addedDataNodes: " << testDataNodes.nodesContainer-> addedDataNodes.size() << endl;
    cout << "deletedDataNodes: " << testDataNodes.nodesContainer-> deletedDataNodes.size() << endl;
    cout << "updatedDataNodes: " << testDataNodes.nodesContainer-> updatedDataNodes.size() << endl;
*/
    bool res = ((testDataNodes.nodesContainer-> addedDataNodes.size() == 8)
            && (testDataNodes.nodesContainer-> dataNodes.size() == 8)
            && (testDataNodes.nodesContainer-> deletedDataNodes.size() == 0)
            && (testDataNodes.nodesContainer-> updatedDataNodes.size() == 0));

    testDataNodes.Store();

    // testShowResult(res, FNS);

    return res;
}

bool reloadData(EgDataNodesType& testDataNodes) {
    // cout << "===== reloadData() " << " =====" << endl;

    testDataNodes.LoadAllNodes();

    testDataNodes.DeleteDataNode(2);
    testDataNodes.MarkUpdatedDataNode(&(testDataNodes[4]));
/*
    cout << "dataNodes: " << testDataNodes.nodesContainer-> dataNodes.size() << endl;
    cout << "addedDataNodes: " << testDataNodes.nodesContainer-> addedDataNodes.size() << endl;
    cout << "deletedDataNodes: " << testDataNodes.nodesContainer-> deletedDataNodes.size() << endl;
    cout << "updatedDataNodes: " << testDataNodes.nodesContainer-> updatedDataNodes.size() << endl;
*/
    bool res = ((testDataNodes.nodesContainer-> addedDataNodes.size() == 0)
            && (testDataNodes.nodesContainer-> dataNodes.size() == 8)
            && (testDataNodes.nodesContainer-> deletedDataNodes.size() == 0)
            && (testDataNodes.nodesContainer-> updatedDataNodes.size() == 1));

    return res;
}


int main() {
    // std::remove("locations.dnl"); // delete layout file
    // std::remove("locations.gdn"); // delete data nodes file

    EgDatabaseType graphDB;
    EgDataNodesType locationsNodesType("locations", &graphDB);

    cout << "===== Test basic database ops " << " =====" << endl;

    bool res = initDatabase(graphDB);
    locationsNodesType.Connect(graphDB);
    // PrintDataNodeLayout(*(locationsNodesType.dataNodeLayout));
    bool res1 = testEgDataNodesTypeBasicNodeOps(locationsNodesType);
    bool res2 = reloadData(locationsNodesType);

    if (res && res1 && res2)
        cout << "PASS" << endl;
    else
        cout << "FAIL" << endl;
}