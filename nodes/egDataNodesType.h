#pragma once
#include <iostream>

#include "egDataNodesContainer.h"
// #include "egDataNodeLayout.h"

//  ============================================================================
//          EXTERNAL TYPES
//  ============================================================================

class EgLinksType;

//  ============================================================================

class EgDataNodesType { // DataNodesAmbience
public:
    bool                        isConnected     { false };      // ? TODO nodes data status ( connected, no file, no server)
    std::string                 dataNodesName;
    EgDatabaseType*             metaInfoDatabase;
    EgDataNodeLayoutType*       dataNodeLayout  { nullptr };    // layout == class == type of data nodes
    EgDataNodesContainerType*   nodesContainer  { nullptr };

    std::map < std::string, EgLinksType* >    linksNames;

    // EgIndexConditionsTree* index_tree       {nullptr};       // indexed fields operations
    // std::vector <uint64_t> IndexOffsets;                     // offsets returned by index, for index-based operations (AND, OR)

    EgDataNodesType(std::string a_dataNodesName, EgDatabaseType* aDatabase):
            dataNodesName(a_dataNodesName),
            metaInfoDatabase(aDatabase),
            dataNodeLayout (new EgDataNodeLayoutType(a_dataNodesName)),
            nodesContainer (new EgDataNodesContainerType(a_dataNodesName, dataNodeLayout)) {}
    ~EgDataNodesType() { clear(); delete dataNodeLayout; delete nodesContainer; }

    int Connect(EgDatabaseType& myDB, std::string serverName);
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