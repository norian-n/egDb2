#pragma once
#include "../metainfo/egDatabaseType.h"

//  ============================================================================
//          EXTERNAL TYPES
//  ============================================================================

// class EgDataNodeLinksType;
// EgDataNodeType notFound;  // dummy data node for GUI if no data found

//  ============================================================================

class EgLinksType {
public:
    // bool                        isConnected         { false };      // ? TODO nodes data status ( connected, no file, no server)
    EgLayoutIDType              linkTypeID          { 0 };
    std::string                 linkTypeName;
    EgDatabaseType*             metaInfoDatabase    { nullptr };    
    EgDataNodeLayoutType*       linksStorageLayout  { nullptr };
    EgDataNodesContainerType*   linksStorage        { nullptr };
    // EgDataNodesContainerType* linkDataStorage;
    EgDataNodesContainerType* fromDataNodes         { nullptr };
    EgDataNodesContainerType* toDataNodes           { nullptr };

    EgLinksType(std::string a_Name, EgDatabaseType* a_Database):
            linkTypeName(a_Name),
             metaInfoDatabase(a_Database),           
            linksStorageLayout(new EgDataNodeLayoutType(a_Name)),
            linksStorage (new EgDataNodesContainerType(a_Name, linksStorageLayout)) { initLinkLayout(linksStorageLayout); }

    ~EgLinksType() { clear();  delete linksStorage; delete linksStorageLayout; }
    
    void clear();
    void initLinkLayout(EgDataNodeLayoutType* linkLayout);
    int ConnectToDatabase();
    void AddRawLink(EgDataNodeIDType fromID, EgDataNodeIDType toID);
    int AddContainersLink(EgDataNodeIDType fromID, EgDataNodeIDType toID);
    int LoadLinks();
    int StoreLinks();
    void ConnectDataNodesTypes(EgDataNodesType& from, EgDataNodesType& to);
    int ResolveNodesIDsToPtrs();
};

// ======================== Debug ========================

void PrintResolvedLinks(const EgDataNodeType& node);