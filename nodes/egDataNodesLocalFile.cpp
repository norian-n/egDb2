#include "egDataNodesLocalFile.h"
#include <iostream>

bool EgDataNodesLocalFileType::InitFile(std::string layoutName) { // tests support
    nodesFile.fileName = layoutName + ".gdn";
    bool isOk = nodesFile.openToWrite();
    if (isOk)
    {
        nodesFile.writeType<EgFileOffsetType>(0);
        nodesFile.writeType<EgFileOffsetType>(0);
        nodesFile.close();
    }
    return isOk;
}

bool EgDataNodesLocalFileType::OpenFileToRead(std::string layoutName) { // tests support
    nodesFile.fileName = layoutName + ".gdn";
    return nodesFile.openToRead();
}

bool EgDataNodesLocalFileType::OpenFileToUpdate(std::string layoutName) { // tests support
    nodesFile.fileName = layoutName + ".gdn";
    bool isOk = nodesFile.openToUpdate();
    if (!isOk)
    {
        isOk = InitFile(layoutName);
        if (isOk)
            isOk = nodesFile.openToUpdate();
    }
    return isOk;
}

bool EgDataNodesLocalFileType::getFirstNodeOffset(EgFileOffsetType &theOffset) {
    nodesFile.seekRead(0);
    nodesFile >> theOffset;
    return nodesFile.fileStream.good();
}

bool EgDataNodesLocalFileType::getLastNodeOffset(EgFileOffsetType &theOffset) {
    nodesFile.seekRead(sizeof(EgFileOffsetType));
    nodesFile >> theOffset;
    return nodesFile.fileStream.good();
}

bool EgDataNodesLocalFileType::WriteDataNode(EgDataNodeType* theNode) { // write to end, update header
    /* if ( !theNode) {
        std::cout  << "DEBUG: WriteDataNode() theNode ptr is NULL " << std::endl;
    } */
    // std::cout << "WriteDataNode() container fieldsCount: " << std::dec << (int) theNode-> dataFieldsContainer.fieldsCount << std::endl;
    // std::cout << "WriteDataNode() container size: " << std::dec << theNode-> dataFieldsContainer.dataFields.size() << std::endl;
    // std::cout << "WriteDataNode() ptr " << std::hex << (uint64_t) theNode-> dataFieldsContainer << std::endl;
    EgFileOffsetType nodeOffset = nodesFile.getFileSize();
    theNode-> dataFileOffset = nodeOffset;

    EgFileOffsetType headerLastNodeOffset{0};
    nodesFile.seekRead(sizeof(EgFileOffsetType)); // save previous last node in chain
    nodesFile >> headerLastNodeOffset;
    // std::cout << "nodeOffset: " <<  std::hex << nodeOffset << std::endl;
    nodesFile.seekWrite(sizeof(EgFileOffsetType)); // update last node in chain ptr
    nodesFile << nodeOffset;                       // to this node offset

    if (headerLastNodeOffset)
    {
        nodesFile.seekWrite(headerLastNodeOffset + sizeof(EgDataNodeIDType));
        nodesFile << nodeOffset; // update "next" pointer of prev node
    }

    EgFileOffsetType headerFirstNodeOffset{0};
    nodesFile.seekRead(0);
    nodesFile >> headerFirstNodeOffset;
    if (!headerFirstNodeOffset)
    {
        nodesFile.seekWrite(0);
        nodesFile << nodeOffset; // update "next" pointer of prev node
    }
    // std::cout << "WriteDataNode() nodeOffset: " <<  std::hex << nodeOffset << std::endl;
    nodesFile.seekWrite(nodeOffset);
    nodesFile << theNode-> dataNodeID;
    nodesFile.writeType<EgFileOffsetType>(0); // pointer to next node = nullptr
    nodesFile << headerLastNodeOffset;        // pointer to prev node
    // std::cout << "ID: " << std::dec << theNode-> dataNodeID << ", headerLastNodeOffset: " <<  std::hex << headerLastNodeOffset << std::endl;
    // std::cout << "container size: " << std::dec << (int) theNode-> dataFieldsContainer.fieldsCount << " " << theNode-> dataFieldsContainer.dataFields.size() << std::endl;
    writeDataFieldsToFile(theNode-> dataFieldsContainer, nodesFile);
    return nodesFile.fileStream.good();
}

bool EgDataNodesLocalFileType::ReadDataNode(EgDataNodeType *theNode, EgFileOffsetType &nextOffset) {
    nodesFile.seekRead(theNode->dataFileOffset);
    nodesFile >> theNode->dataNodeID;
    nodesFile >> nextOffset;
    // std::cout  << "theNode-> dataFileOffset = " << std::hex << theNode-> dataFileOffset;
    // std::cout  << " , dataNodeID = " << std::dec << theNode-> dataNodeID;
    // std::cout  << " , nextOffset = " << std::hex << nextOffset << std::endl;
    nodesFile.seekRead(theNode->dataFileOffset + sizeof(EgDataNodeIDType) + 2 * sizeof(EgFileOffsetType));
    readDataFieldsFromFile(theNode->dataFieldsContainer, nodesFile);
    return nodesFile.fileStream.good();
}

bool EgDataNodesLocalFileType::DeleteDataNode(EgDataNodeType *theNode) {
    EgFileOffsetType nextNodeOffset{0};
    EgFileOffsetType prevNodeOffset{0};
    nodesFile.seekRead(theNode->dataFileOffset + sizeof(EgDataNodeIDType));
    nodesFile >> nextNodeOffset;
    nodesFile >> prevNodeOffset;
    /* std::cout << "DeleteDataNode() dataFileOffset: " <<  std::hex << theNode-> dataFileOffset;
    std::cout << " , prevNodeOffset: " <<  std::hex << theNode-> prevNodeOffset;
    std::cout << " , nextNodeOffset: " <<  std::hex << theNode-> nextNodeOffset << std::endl; */
    if (prevNodeOffset)
        nodesFile.seekWrite(prevNodeOffset + sizeof(EgDataNodeIDType));
    else
        nodesFile.seekWrite(0); // update first node of chain ptr
    nodesFile << nextNodeOffset;

    if (nextNodeOffset)
        nodesFile.seekWrite(nextNodeOffset + sizeof(EgDataNodeIDType) + sizeof(EgFileOffsetType));
    else
        nodesFile.seekWrite(sizeof(EgFileOffsetType)); // update last node in chain ptr
    nodesFile << prevNodeOffset;

    nodesFile.seekWrite(theNode->dataFileOffset + sizeof(EgDataNodeIDType)); // FIXME TODO - history chain
    nodesFile.writeType<EgFileOffsetType>(0);
    nodesFile.writeType<EgFileOffsetType>(0);
    return nodesFile.fileStream.good();
}

// ======================== Debug ========================

void ReadDataNodeAndOffsets(EgFileType &theNodesFile, EgDataNodeType *theNode) {
    theNodesFile.seekRead(theNode->dataFileOffset);
    theNodesFile >> theNode->dataNodeID;
#ifdef EG_NODE_OFFSETS_DEBUG
    theNodesFile >> theNode->nextNodeOffset;
    theNodesFile >> theNode->prevNodeOffset;
#endif
    readDataFieldsFromFile(theNode->dataFieldsContainer, theNodesFile);
}