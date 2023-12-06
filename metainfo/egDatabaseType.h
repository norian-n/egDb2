#pragma once
#include <iostream>

#include "../nodes/egDataNodesType.h"

//  ============================================================================
//          EXTERNAL TYPES
//  ============================================================================
    const std::string nodesTypesStorageName("nodesTypesStorageLayout");
    const std::string linksTypesStorageName("linksTypesStorageLayout");
//  ============================================================================

// egdb/meta/....

class EgDatabaseType {
public:
    EgDataNodeLayoutType* nodesTypesStorageLayout;
    EgDataNodeLayoutType* linksTypesStorageLayout;

    EgDataNodesContainerType* nodesTypesStorage;
    EgDataNodesContainerType* linksTypesStorage;

    std::map <EgLayoutIDType, EgDataNodesType*>  dataNodesTypes; 

    EgDatabaseType():   nodesTypesStorageLayout (new EgDataNodeLayoutType(nodesTypesStorageName)),
                        linksTypesStorageLayout (new EgDataNodeLayoutType(linksTypesStorageName)),
                        nodesTypesStorage (new EgDataNodesContainerType(nodesTypesStorageName, nodesTypesStorageLayout)),
                        linksTypesStorage (new EgDataNodesContainerType(linksTypesStorageName, linksTypesStorageLayout)) 
    { initLayouts(); }
    
    ~EgDatabaseType() { 
        clear(); 
        delete nodesTypesStorageLayout; delete linksTypesStorageLayout; delete nodesTypesStorage; delete linksTypesStorage;
    }
    
    void clear() {
        nodesTypesStorage-> clear();
        linksTypesStorage-> clear();
    }

    int ConnectDataNodesTypeRaw(EgLayoutIDType dntID, EgDataNodesType* dntPtr) {
        auto iter = dataNodesTypes.find(dntID); // search if already connected
        if (iter != dataNodesTypes.end())
            return -1;
        dataNodesTypes.insert(std::pair<EgLayoutIDType, EgDataNodesType* > (dntID, dntPtr));
        return 0;
    }

    EgDataNodesType* GetNodeTypePtrByID(EgLayoutIDType nodeTypeID) {
        auto iter = dataNodesTypes.find(nodeTypeID); // search all nodes
        if (iter != dataNodesTypes.end())
            return iter->second;
        return nullptr;
    }


    void initLayouts() {
        nodesTypesStorageLayout-> LayoutInitStart();
        nodesTypesStorageLayout-> AddDataFieldName("nodesLayoutID");
        nodesTypesStorageLayout-> AddDataFieldName("nodesTypeName");
        nodesTypesStorageLayout-> layoutSettings.isServiceType = true;
        nodesTypesStorageLayout-> layoutMode = egLayoutActive;// virtual, do NOT commit to db

        linksTypesStorageLayout-> LayoutInitStart();
        linksTypesStorageLayout-> AddDataFieldName("linksTypeName");
        linksTypesStorageLayout-> layoutSettings.isServiceType = true;
        linksTypesStorageLayout-> layoutMode = egLayoutActive;// virtual, do NOT commit to db
    }

    void AddDataNodesTypeInfo(EgLayoutIDType layoutID, std::string& typeName) {
        EgDataNodeType *newNode = new EgDataNodeType(nodesTypesStorageLayout);
        *newNode << layoutID;
        *newNode << typeName;
        *nodesTypesStorage << newNode;
    }

    void AddLinksTypeInfo(std::string& linksTypeName) {
        EgDataNodeType* newNode = new EgDataNodeType(linksTypesStorageLayout);
        *newNode << linksTypeName;
        *linksTypesStorage << newNode;
    }

    int LoadTypesInfo() {
        nodesTypesStorage-> clear();
        linksTypesStorage-> clear();
        int res1 = nodesTypesStorage-> LoadAllLocalFileNodes();
        int res2 = linksTypesStorage-> LoadAllLocalFileNodes();
        return res1+res2;
    }

    int StoreTypesInfo() {
        int res1 = nodesTypesStorage-> StoreToLocalFile();
        int res2 = linksTypesStorage-> StoreToLocalFile();
        return res1+res2;
    }
};