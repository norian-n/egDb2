#pragma once
#include <cstdint>
#include "egDataNodesLocalFile.h"

//  ============================================================================
//          EXTERNAL TYPES
//  ============================================================================

// EgDataNodeType notFound;  // dummy data node for GUI if no data found

//  ============================================================================

class EgDataNodesContainerType {
public:
    std::string                 dataNodesTypeName;
    EgDataNodeLayoutType*       dataNodeLayout  { nullptr };    // layout == class == type of data nodes
    EgDataNodesLocalFileType*   LocalNodesFile  { nullptr };    // data files *.gdn load/store support
        // data nodes content and changes tracking
    std::map <EgDataNodeIDType, EgDataNodeType*>  dataNodes;        // active nodes container
    std::map <EgDataNodeIDType, EgDataNodeType*>  addedDataNodes;
    std::map <EgDataNodeIDType, EgDataNodeType*>  updatedDataNodes;
    std::map <EgDataNodeIDType, EgDataNodeType*>  deletedDataNodes; // TODO : clear all addons on node delete

    EgDataNodesContainerType(std::string a_dataNodesName): // DEBUG - standalone layout
            dataNodesTypeName  (a_dataNodesName),
            LocalNodesFile (new EgDataNodesLocalFileType()),
            dataNodeLayout (new EgDataNodeLayoutType(a_dataNodesName))  {}
    EgDataNodesContainerType(std::string a_dataNodesName, EgDataNodeLayoutType* a_dataNodeLayout): // layout from upper layer
            dataNodesTypeName  (a_dataNodesName),
            LocalNodesFile (new EgDataNodesLocalFileType()),
            dataNodeLayout (a_dataNodeLayout)  {}
    ~EgDataNodesContainerType() { clear(); /* delete dataNodeLayout;*/ delete LocalNodesFile; } // FIXME check dynamic layout

    int Connect(EgDatabaseType& myDB, std::string serverName);
    void clear();
    int LoadLocalLayout();
    EgDataNodeType* GetNodePtrByID(EgDataNodeIDType nodeID);
    int AddDataNode(EgDataNodeType* newNode);
    int MarkUpdatedDataNode(EgDataNodeType* theNode);
    void DeleteDataNode(const EgDataNodeIDType delID);
    int StoreToLocalFile();
    int LoadAllLocalFileNodes();
};

// ===================== Operators =======================

EgDataNodesContainerType& operator << (EgDataNodesContainerType& nodesContainer, EgDataNodeType* newNode);

// ======================== Debug ========================

void PrintDataNodesContainer(EgDataNodesContainerType& container);