#include "../metainfo/egDatabaseType.h"
#include <iostream>
#include <cstring>

using namespace std;

EgDatabaseType graphDB;

bool initDatabase() {
    // EgNodeTypeSettings typeSettings;
    // typeSettings.useLocation = true;
    // typeSettings.useLinks = true;

    graphDB.CreateNodeType("locations"); // , typeSettings);

    graphDB.AddDataField("name");
    graphDB.AddDataField("status"); // , isIndexed create index
    graphDB.AddDataField("imageType");
    graphDB.AddDataField("x");
    graphDB.AddDataField("y");    

    graphDB.CommitNodeType(); 

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
    dataNodes << newNode;
}

void initData() {
    EgDataNodesType locationsNodesType("locations", &graphDB);

    addSampleDataNode(locationsNodesType);  // nodeID == 1
    addSampleDataNode(locationsNodesType);
    addSampleDataNode(locationsNodesType);

    locationsNodesType.Store();

// QList<QVariant> ins_values;
// #define ADD_RECORD(values,dataNodesType) ins_values.clear(); ins_values << values; dataNodesType.AddDataNode(ins_values);

    // ADD_RECORD("one" << 1, nodes);
    // ADD_RECORD("two" << 2, nodes);

        // add sample data with location coordinates
/*
    nodes.Connect(graphDB, "locations");

    EgDataNodeIdType newID;
    QList<QVariant> addValues;
    QList<QVariant> locValues;

    addValues.clear();
    addValues << "one" << 1;

    nodes.AddDataNode(addValues, newID);

    locValues.clear();
    locValues << 150 << 150 << 1;

    nodes.AddLocation(locValues, newID);

    addValues.clear();
    addValues << "two" << 2;

    nodes.AddDataNode(addValues, newID);

    locValues.clear();
    locValues << 70 << 70 << 2;

    nodes.AddLocation(locValues, newID);

    addValues.clear();
    addValues << "three" << 3;

    nodes.AddDataNode(addValues, newID);

    locValues.clear();
    locValues << 0 << 0 << 3;

    nodes.AddLocation(locValues, newID);

    addValues.clear();
    addValues << "four" << 4;

    nodes.AddDataNode(addValues, newID);

    locValues.clear();
    locValues << 0 << 100 << 4;

    nodes.AddLocation(locValues, newID);

    nodes.StoreData();

        // add links (graph edges)
    // nodes.ConnectLinkType("linktype");

    links.Connect(graphDB, "linktype", nodes, nodes);

    */

    /*
    nodes.AddArrowLink("linktype", 1, nodes, 2);
    nodes.AddArrowLink("linktype", 2, nodes, 3);
    nodes.AddArrowLink("linktype", 2, nodes, 4);
    */
/*
    links.AddArrowLink(1,2);
    links.AddArrowLink(2,3);
    links.AddArrowLink(2,4);

    links.StoreLinks();

    // nodes.StoreAllLinks();

*/
}

/*
void MainWindow::cleanUpFiles()
{
    QDir dir;

    QStringList nameFilters;

    nameFilters  << "*.odf" << "*.odx" << "*.dat" << "*.ddt" << "*.dln" << "*.ent"; // << "test*.*";

        // get filtered filenames
    QStringList ent = dir.entryList(nameFilters);
    // qDebug() << dir.entryList();

        // remove files
    foreach (const QString& str, ent)
        dir.remove(str);

    if (! dir.exists("egdb"))
        return;

    dir = QDir("egdb");

        // get filtered filenames
    ent = dir.entryList(nameFilters);
    // qDebug() << dir.entryList();

        // remove files
    foreach (const QString& str, ent)
        dir.remove(str);
}
*/

/*
void MainWindow::AddImages()
{
    // create node type

    EgNodeTypeSettings typeSettings;

    graphDB.CreateNodeType("images", typeSettings);

    graphDB.AddDataField("name");
    graphDB.AddDataField("bytearray");

    graphDB.CommitNodeType();

    images.Connect(graphDB, "images");

    EgDataNodeIdType newID;
    QList<QVariant> addValues;

        // get all files
    QDir dir;

    QStringList nameFilters;

    nameFilters  << "*.png" << "*.jpg" << "*.bmp" << "*.gif" << "*.ico"; // << "test*.*";

        // get filtered filenames
    QStringList ent = dir.entryList(nameFilters);
    qDebug() << ent;

    foreach (const QString& fileName, ent)
    {
        // QString fileName("test.png");

        QPixmap pixMap(40,40);

        bool loadRes = pixMap.load(fileName);

        if (loadRes)
        {
            pixMap = pixMap.scaled(40,40);

            QByteArray imageData;
            QDataStream dataStream(&imageData, QIODevice::WriteOnly);

            dataStream << pixMap;

            addValues.clear();
            addValues << fileName << imageData;

            images.AddDataNode(addValues, newID);

            qDebug() << fileName;
        }
    }

    images.StoreData();

    /*
    QPixmap pixmap = *child->pixmap();

    QByteArray itemData;
    QDataStream dataStream(&itemData, QIODevice::WriteOnly);
    dataStream << pixmap << QPoint(event->pos() - child->pos()); */ /*
}
*/

int main() {
    // std::remove("locations.dnl"); // delete layout file
    // std::remove("locations.gdn"); // delete data nodes file

    cout << "===== Test basic database ops " << " =====" << endl;

    bool res = initDatabase();
    initData();

    if (res)
        cout << "PASS" << endl;
    else
        cout << "FAIL" << endl;
}