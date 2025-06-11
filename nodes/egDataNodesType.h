#pragma once
#include <iostream>

#include "egDataNodesContainer.h"
// #include "egDataNodeLayout.h"

//  ============================================================================
//          EXTERNAL TYPES
//  ============================================================================

class EgLinksType;

/*
enum EgNodesListModeEnum
{
    egLayoutInit,
    egLayoutActive,
    egLayoutDataPump    // TBD: special data copy procedure with parameters "from field => to field", indexes rebuilding
};
*/

//  ============================================================================

class EgDataNodesType { // DataNodesAmbience
public:
    bool                        isConnected      { false };     // ? TODO nodes data status ( connected, no file, no server)
    std::string                 dataNodesName;
    EgDatabaseType*             metaInfoDatabase { nullptr };   // nodes and links layout == blueprint == class == type info
    EgDataNodeLayoutType*       dataNodeLayout   { nullptr };   // layout == blueprint == class == type of these data nodes
    EgDataNodesContainerType*   nodesContainer   { nullptr };   // data storage of all these nodes

    std::map < std::string, EgLinksType* >    linksNames;

    // EgIndexConditionsTree* index_tree       {nullptr};       // indexed fields operations
    // std::vector <uint64_t> IndexOffsets;                     // offsets returned by index, for index-based operations (AND, OR)

    EgDataNodesType(std::string a_dataNodesName, EgDatabaseType* aDatabase);
    ~EgDataNodesType() { clear(); delete dataNodeLayout; delete nodesContainer; }

    int Connect(EgDatabaseType& myDB); // , std::string serverName);
    void clear();
    int OpenLocalLayout();

    int AddDataNode(EgDataNodeType* newNode);
    int MarkUpdatedDataNode(EgDataNodeType* theNode);
    void DeleteDataNode(const EgDataNodeIDType delID);
    int Store();
    int LoadAllNodes();
    EgDataNodeType& operator[](EgDataNodeIDType nodeID);
};

// ===================== Operators =======================

EgDataNodesType& operator << (EgDataNodesType& nodesServType, EgDataNodeType* newNode);