#pragma once
#include "egDataNode.h"
#include "egDataNodeLayout.h"
#ifdef EG_NODE_OFFSETS_DEBUG
    #include <iostream>
#endif

class EgDataNodesLocalFileType
{
public:
    EgFileType nodesFile;

    // EgDataNodesLocalFileType(): {}
    // ~EgDataNodesLocalFileType() {}

    bool InitFile(std::string layoutName);
    bool OpenFileToRead(std::string layoutName);
    bool OpenFileToUpdate(std::string layoutName);
    bool getFirstNodeOffset(EgFileOffsetType& theOffset);
    bool getLastNodeOffset(EgFileOffsetType& theOffset);
    bool WriteDataNode(EgDataNodeType* theNode);
    bool ReadDataNode(EgDataNodeType* theNode, EgFileOffsetType& nextOffset);
    bool DeleteDataNode(EgDataNodeType* theNode);
};

// ======================== Debug ========================

void ReadDataNodeAndOffsets(EgFileType &theNodesFile, EgDataNodeType *theNode);