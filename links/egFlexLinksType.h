#pragma once
#include "../metainfo/egDatabaseType.h"

//  ============================================================================
//          EXTERNAL TYPES
//  ============================================================================

// class EgDataNodeLinksType;
// EgDataNodeType notFound;  // dummy data node for GUI if no data found

//  ============================================================================

// egdb/meta/....

class EgFlexLinksType {
public:
    // bool                        isConnected         { false };      // ? TODO nodes data status ( connected, no file, no server)
    EgLayoutIDType              linkTypeID          { 0 };
    std::string                 linkTypeName;
    EgDatabaseType*             metaInfoDatabase    { nullptr };    
    EgDataNodeLayoutType*       linksStorageLayout  { nullptr };
    EgDataNodesContainerType*   linksStorage        { nullptr };
    // EgDataNodesContainerType* linkDataStorage;

    EgFlexLinksType(std::string a_Name, EgDatabaseType* a_Database):
            linkTypeName(a_Name),
             metaInfoDatabase(a_Database),           
            linksStorageLayout(new EgDataNodeLayoutType(a_Name)),
            linksStorage (new EgDataNodesContainerType(a_Name, linksStorageLayout)) { initFlexLinkLayout(linksStorageLayout); }

    ~EgFlexLinksType() { clear(); delete linksStorage; delete linksStorageLayout; }
    
    void clear();
    void initFlexLinkLayout(EgDataNodeLayoutType* linkLayout);
    void AddRawLink(EgLayoutIDType fromLayID, EgDataNodeIDType fromID, EgLayoutIDType toLayID, EgDataNodeIDType toID);
    void AddContainersLink(EgLayoutIDType fromLayID, EgDataNodeIDType fromID, EgLayoutIDType toLayID, EgDataNodeIDType toID);
    int LoadLinks();
    int StoreLinks();
    int ResolveNodesIDsToPtrs();
};

// ======================== Debug ========================

void PrintResolvedLinksFlex(const EgDataNodeType& node);
