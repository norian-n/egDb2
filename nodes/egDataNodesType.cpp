#include <iostream>

#include "egDataNodesType.h"

EgDataNodeType nodeNotFound(nullptr);  // dummy data node for GUI if no data found

int EgDataNodesType::Connect(EgDatabaseType &myDB, std::string serverName = std::string("")) {
    if (OpenLocalLayout())
    {
        std::cout << "ERROR: EgDataNodesContainerType can't open data nodes layout " << dataNodesName << ".dnl" << std::endl;
        return -1;
    }
    return 0;
}

void EgDataNodesType::clear() {
    nodesContainer->clear();
}

int EgDataNodesType::OpenLocalLayout() {
    return dataNodeLayout->LocalLoadLayout();
}

int EgDataNodesType::AddDataNode(EgDataNodeType *newNode) {
    return nodesContainer->AddDataNode(newNode);
}

int EgDataNodesType::MarkUpdatedDataNode(EgDataNodeType *theNode) {
    return nodesContainer->MarkUpdatedDataNode(theNode);
}

void EgDataNodesType::DeleteDataNode(const EgDataNodeIDType delID) {
    return nodesContainer->DeleteDataNode(delID);
}

int EgDataNodesType::Store() {
    return nodesContainer->StoreToLocalFile();
}

int EgDataNodesType::LoadAllNodes() {
    return nodesContainer->LoadAllLocalFileNodes();
}

EgDataNodeType &EgDataNodesType::operator[](EgDataNodeIDType nodeID) {
    EgDataNodeType *nodePtr = nodesContainer->GetNodePtrByID(nodeID);
    if (nodePtr)
        return *nodePtr;
    return nodeNotFound;
}

// ===================== Operators =======================

EgDataNodesType& operator << (EgDataNodesType& nodesServType, EgDataNodeType* newNode) { nodesServType.nodesContainer-> AddDataNode(newNode); return nodesServType; }
