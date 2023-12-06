#include <iostream>

#include "egDataNodesContainer.h"

int EgDataNodesContainerType::Connect(EgDatabaseType &myDB, std::string serverName = std::string("")) {
    if (LoadLocalLayout())
    {
        std::cout << "ERROR: EgDataNodesContainerType can't open data nodes layout " << dataNodesTypeName << ".dnl" << std::endl;
        return -1;
    }
    return 0;
}

void EgDataNodesContainerType::clear() {
    for (auto nodesIter : dataNodes) // 17 [first, second], <11 = dataFieldsNames.begin(); fieldsIter != dataFieldsNames.end(); ++fieldsIter) {
        delete nodesIter.second;
    dataNodes.clear();
    for (auto nodesIter : deletedDataNodes) // 17 [first, second], <11 = dataFieldsNames.begin(); fieldsIter != dataFieldsNames.end(); ++fieldsIter) {
        delete nodesIter.second;
    deletedDataNodes.clear();
    addedDataNodes.clear();
    updatedDataNodes.clear();
}

int EgDataNodesContainerType::LoadLocalLayout() {
    return dataNodeLayout->LocalLoadLayout();
}

EgDataNodeType *EgDataNodesContainerType::GetNodePtrByID(EgDataNodeIDType nodeID) {
    auto iter = dataNodes.find(nodeID); // search all nodes
    if (iter != dataNodes.end())
        return iter->second;
    return nullptr;
}

int EgDataNodesContainerType::AddDataNode(EgDataNodeType *newNode) {
    newNode-> dataNodeID = dataNodeLayout-> getNextID();
    dataNodes.insert(std::make_pair(newNode-> dataNodeID, newNode));
    addedDataNodes.insert(std::make_pair(newNode-> dataNodeID, newNode));
    return 0;
}

int EgDataNodesContainerType::MarkUpdatedDataNode(EgDataNodeType *theNode) {
    auto iter = dataNodes.find(theNode->dataNodeID); // search all nodes
    if (iter == dataNodes.end())
        return -1;
    updatedDataNodes.insert(std::make_pair(theNode-> dataNodeID, theNode));
    return 0;
}

void EgDataNodesContainerType::DeleteDataNode(const EgDataNodeIDType delID) {
    bool nodeFound{false};
    auto delIter = deletedDataNodes.find(delID); // search deleted nodes
    if (delIter != deletedDataNodes.end())
        return;
    auto iter = dataNodes.find(delID); // search all nodes
    if (iter != dataNodes.end())
    {
        nodeFound = true;
        dataNodes.erase(iter);
    }
    auto addIter = addedDataNodes.find(delID); // search added nodes
    if (addIter != addedDataNodes.end())
    {
        addedDataNodes.erase(addIter);
        return;
    }
    else if (nodeFound)
    {
        deletedDataNodes.insert(std::make_pair(delID, iter->second));
        auto updIter = updatedDataNodes.find(delID); // search updated nodes
        if (updIter != updatedDataNodes.end())
            updatedDataNodes.erase(updIter);
    }
}

int EgDataNodesContainerType::StoreToLocalFile() {
    LocalNodesFile-> OpenFileToUpdate(dataNodesTypeName);
    for (auto delNodesIter : deletedDataNodes) { // 17 [first, second], <11 = dataFieldsNames.begin(); fieldsIter != dataFieldsNames.end(); ++fieldsIter) {
        // PrintEgDataNodeTypeFields(*(delNodesIter.second));
        LocalNodesFile-> DeleteDataNode(delNodesIter.second);
    }
    // std::cout << "===== After DeleteDataNode() =====" << std::endl;
    for (auto updNodesIter : updatedDataNodes)
    {
        // PrintEgDataNodeTypeFields(*(updNodesIter.second));
        LocalNodesFile-> DeleteDataNode(updNodesIter.second);
        LocalNodesFile-> WriteDataNode (updNodesIter.second);
    }
    // std::cout << "===== After WriteDataNode 1 =====" << std::endl;
    for (auto newNodesIter : addedDataNodes) {
        // PrintEgDataNodeTypeFields(*(newNodesIter.second));
        // std::cout << "StoreToLocalFile() container size: " << std::dec << (newNodesIter.second)-> dataFieldsContainer.dataFields.size() << std::endl;
        // std::cout << "newNodesIter.second: " << std::hex << (uint64_t) &((newNodesIter.second)-> dataFieldsContainer.dataFields) << std::endl;
        // std::cout << "* (newNodesIter.second) : " << std::hex << (uint64_t) &((*(newNodesIter.second)).dataFieldsContainer.dataFields) << std::endl;
        LocalNodesFile-> WriteDataNode(newNodesIter.second);
    }
    // std::cout << "===== After WriteDataNode 2 =====" << std::endl;
    LocalNodesFile-> nodesFile.close();
    updatedDataNodes.clear();
    deletedDataNodes.clear();
    addedDataNodes.clear();
    // std::cout << "===== StoreToLocalFile() exit =====" << std::endl;
    return 0;
}

int EgDataNodesContainerType::LoadAllLocalFileNodes() {
    clear();
    EgFileOffsetType nextOffset{0};
    EgDataNodeType *newNode;
    if (!LocalNodesFile->OpenFileToRead(dataNodesTypeName))
    {
        std::cout << "ERROR: loadAllLocalNodes() can't open file " << dataNodesTypeName << ".gdn" << std::endl;
        return -1;
    }
    LocalNodesFile->getFirstNodeOffset(nextOffset); // FIXME check read
    while (nextOffset)
    {
        // std::cout  << "getFirstNodeOffset() nextOffset = " << std::hex << nextOffset << std::endl;
        newNode = new EgDataNodeType(dataNodeLayout);
        newNode->dataFileOffset = nextOffset;
        LocalNodesFile->ReadDataNode(newNode, nextOffset);
        dataNodes.insert(std::make_pair(newNode->dataNodeID, newNode));
    }
    LocalNodesFile->nodesFile.close();
    return 0;
}

// ===================== Operators =======================

EgDataNodesContainerType& operator << (EgDataNodesContainerType& nodesContainer, EgDataNodeType* newNode) { nodesContainer.AddDataNode(newNode); return nodesContainer; }

// ======================== Debug ========================

void PrintDataNodesContainer(EgDataNodesContainerType& container) {
    std::cout << "EgDataNodesContainer nodes: " << std::endl;
    for (auto nodesIter : container.dataNodes) // 17 [first, second], <11 = dataFieldsNames.begin(); fieldsIter != dataFieldsNames.end(); ++fieldsIter) {
        PrintEgDataNodeTypeFields(*(nodesIter.second));
    if (container.addedDataNodes.size()) {
         std::cout << "EgDataNodesContainer added nodes: " << std::endl;
        for (auto nodesIter : container.addedDataNodes)
            PrintEgDataNodeTypeFields(*(nodesIter.second));
    }
    if (container.deletedDataNodes.size()) {
        std::cout << "EgDataNodesContainer deleted nodes: " << std::endl;
        for (auto nodesIter : container.deletedDataNodes)
            PrintEgDataNodeTypeFields(*(nodesIter.second));
    }
    if (container.updatedDataNodes.size()) {    
        std::cout << "EgDataNodesContainer updated nodes: " << std::endl;
        for (auto nodesIter : container.updatedDataNodes)
            PrintEgDataNodeTypeFields(*(nodesIter.second));
    }
}