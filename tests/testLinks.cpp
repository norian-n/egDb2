#include "../links/egLinksType.h"
#include "../nodes/egDataNodesType.h"
#include <iostream>
#include <cstring>

using namespace std;

EgDatabaseType theDatabase;

string field1 = "111111\0";
const char* field2 = "test some string 2\0";
string field3("just test 3");

inline void addSampleDataNode(EgDataNodesType& dataNodes) {
    EgDataNodeType* newNode = new EgDataNodeType(dataNodes.dataNodeLayout);
    *newNode << field1;
    *newNode << field2;
    *newNode << field3;
    dataNodes << newNode;
}

bool testLinksResolving(EgLinksType& testLinks) {
    EgDataNodesType fromType("testLinksFrom", &theDatabase);
    EgDataNodesType toType  ("testLinksTo",   &theDatabase);

    addSampleDataNode(fromType);  // nodeID == 1
    addSampleDataNode(fromType);
    addSampleDataNode(fromType);
    addSampleDataNode(fromType);

    addSampleDataNode(toType);
    addSampleDataNode(toType);    // nodeID == 2
    addSampleDataNode(toType);
    addSampleDataNode(toType);

    testLinks.ConnectDataNodesTypes(fromType, toType);
    testLinks.ResolveNodesIDsToPtrs();

    /* for (auto iter : fromType.nodesContainer-> dataNodes)
        PrintEgDataNodeTypeFields(fromType[iter.first]);

    for (auto iter : fromType.nodesContainer-> dataNodes)
        PrintResolvedLinks(*(iter.second));

    for (auto iter : toType.nodesContainer-> dataNodes)
        PrintResolvedLinks(*(iter.second));
    */

    // std::map < EgLinkLayoutIDType, std::vector<EgDataNodeType*> >  inLinks;
    // std::map < EgLinkLayoutIDType, std::vector<EgDataNodeType*> >  outLinks;

    auto linksIterFrom = fromType[1].outLinks.begin();  // nodeID == 1, outLinks[0], 3 items
    auto linksIterTo   = toType[2].inLinks.begin();     // nodeID == 2, inLinks[0], 1 item

    // cout << linksIterFrom->second.size() << endl;
    // cout << linksIterTo->second.size() << endl;

    return ((linksIterFrom->second.size() == 3) && (linksIterTo->second.size() == 1));
}

bool testLinksStorage(EgLinksType& testLinks) {

    testLinks.AddRawLink(1, 2); // nodeID == 1, outLinks[0], 3 items
    testLinks.AddRawLink(1, 3);
    testLinks.AddRawLink(1, 4);

    testLinks.AddRawLink(3, 4);

    testLinks.StoreLinks();
    testLinks.LoadLinks();

    // PrintDataNodesContainer(*(testLinks.linksStorage));

    int count = testLinks.linksStorage-> dataNodes.size();

    return (count == 4);
}

int main() {

    cout << "===== Test EgLinksType =====" << endl;
    
    std::remove("testLinks.gdn");   // delete file
    EgLinksType testLinks("testLinks", &theDatabase);
    // std::remove("testNodes.gdn");  // delete data nodes file

    bool testStor   = testLinksStorage(testLinks);
    bool testResolv = testLinksResolving(testLinks);

    if (testStor && testResolv)
        cout << "PASS" << endl;
    else
        cout << "FAIL" << endl;
}