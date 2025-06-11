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

    EgDataNodeLayoutType* newLayout {nullptr};
    bool CreateNodeTypeFlag {true};

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
    
    void clear();

    int ConnectDataNodesTypeRaw(EgLayoutIDType dntID, EgDataNodesType* dntPtr);
    EgDataNodesType* GetNodeTypePtrByID(EgLayoutIDType nodeTypeID);

    void initLayouts();

    void AddDataNodesTypeInfo(EgLayoutIDType layoutID, std::string& typeName);
    void AddLinksTypeInfo(std::string& linksTypeName);

    int LoadTypesInfo();
    int StoreTypesInfo();

    int LoadLinksInfo();
    int StoreLinksInfo();

    bool checkTypenameExist(std::string& typeName);
    bool checkLinknameExist(std::string& linkName);

    int  CreateNodeLayoutBlueprintClass (std::string& name);
    int  CreateNodeLayoutBlueprintClass (const char* name) { std::string nameStr(name); return CreateNodeLayoutBlueprintClass(nameStr);  } // wrapper
    void AddDataField(std::string& fieldName);
    void AddDataField(const char* typeName)   { std::string name(typeName); return AddDataField(name); } // wrapper
    void CommitNodeLayoutBlueprintClass();

    int AddLinkType(std::string& linkTypeName); // , "locations", "locations");
    int AddLinkType(const char* linkTypeName) { std::string name(linkTypeName); return AddLinkType(name);  } // wrapper

    // delete data nodes layout
};