#include <iostream>

#include "egDatabaseType.h"

void EgDatabaseType::clear() {
    nodesTypesStorage->clear();
    linksTypesStorage->clear();
}

int EgDatabaseType::ConnectDataNodesTypeRaw(EgLayoutIDType dntID, EgDataNodesType *dntPtr) {
    auto iter = dataNodesTypes.find(dntID); // search if already connected
    if (iter != dataNodesTypes.end())
        return -1;
    dataNodesTypes.insert(std::pair<EgLayoutIDType, EgDataNodesType *>(dntID, dntPtr));
    return 0;
}

EgDataNodesType *EgDatabaseType::GetNodeTypePtrByID(EgLayoutIDType nodeTypeID) {
    auto iter = dataNodesTypes.find(nodeTypeID); // search all nodes
    if (iter != dataNodesTypes.end())
        return iter->second;
    return nullptr;
}

void EgDatabaseType::initLayouts() {
    nodesTypesStorageLayout->LayoutInitStart();
    nodesTypesStorageLayout->AddDataFieldName("nodesLayoutID");
    nodesTypesStorageLayout->AddDataFieldName("nodesTypeName");
    nodesTypesStorageLayout->layoutSettings.isServiceType = true;
    nodesTypesStorageLayout->layoutMode = egLayoutActive; // virtual, do NOT commit to db

    linksTypesStorageLayout->LayoutInitStart();
    linksTypesStorageLayout->AddDataFieldName("linksTypeName");
    linksTypesStorageLayout->layoutSettings.isServiceType = true;
    linksTypesStorageLayout->layoutMode = egLayoutActive; // virtual, do NOT commit to db
}

void EgDatabaseType::AddDataNodesTypeInfo(EgLayoutIDType layoutID, std::string &typeName) {
    EgDataNodeType *newNode = new EgDataNodeType(nodesTypesStorageLayout);
    *newNode << layoutID;
    *newNode << typeName;
    *nodesTypesStorage << newNode;
}

void EgDatabaseType::AddLinksTypeInfo(std::string &linksTypeName) {
    EgDataNodeType *newNode = new EgDataNodeType(linksTypesStorageLayout);
    *newNode << linksTypeName;
    *linksTypesStorage << newNode;
}

int EgDatabaseType::LoadTypesInfo() {
    nodesTypesStorage->clear();
    return nodesTypesStorage-> LoadAllLocalFileNodes();
}

int EgDatabaseType::StoreTypesInfo() {
    return nodesTypesStorage-> StoreToLocalFile();
}

int EgDatabaseType::LoadLinksInfo() {
    linksTypesStorage->clear();
    return linksTypesStorage-> LoadAllLocalFileNodes();
}

int EgDatabaseType::StoreLinksInfo() {
    return linksTypesStorage-> StoreToLocalFile();
}

bool EgDatabaseType::checkTypenameExist(std::string& typeName) {
    if (LoadTypesInfo() == 0) { // files exist
        for (auto nodesIter : nodesTypesStorage-> dataNodes) {// 17 [first, second], <11 = dataFieldsNames.begin(); fieldsIter != dataFieldsNames.end(); ++fieldsIter) {
            std::string currentName ((char *)(*(nodesIter.second))["nodesTypeName"].arrayData);
            if (currentName == typeName) {
                // std::cout << "checkTypenameExist() node type found " << std::endl;
                return false; // typename exists
            }
        }
    }
    return true;
}

bool EgDatabaseType::checkLinknameExist(std::string& linkName) {
    if (LoadLinksInfo() == 0) { // files exist
        for (auto nodesIter : linksTypesStorage-> dataNodes) {// 17 [first, second], <11 = dataFieldsNames.begin(); fieldsIter != dataFieldsNames.end(); ++fieldsIter) {
            std::string currentName ((char *)(*(nodesIter.second))["linksTypeName"].arrayData);
            if (currentName == linkName) {
                std::cout << "checkLinknameExist() link type found " << std::endl;
                return false; // typename exists
            }
        }
    }
    return true;
}

int EgDatabaseType::CreateNodeLayoutBlueprintClass(std::string& name) { //,  EgNodeTypeSettings& typeSettings)
    CreateNodeTypeFlag = true; // ok to add fields
    if (! checkTypenameExist(name)) { // check nodestype/layout name
        std::cout << "ERROR: Data nodes type/layout name already exists, cant CreateNodeType()" << std::endl;
        CreateNodeTypeFlag = false; // abort next ops
        return -1;
    }

    newLayout = new EgDataNodeLayoutType(name);
    newLayout-> LayoutInitStart();
    // std::cout << "newLayout before: " << newLayout->layoutName << " " << (int) newLayout-> layoutID << std::endl;
    AddDataNodesTypeInfo(newLayout->layoutID, name); // FIXME STUB

    return 0;
}

void EgDatabaseType::AddDataField(std::string& fieldName) {
    if (CreateNodeTypeFlag)
        newLayout-> AddDataFieldName(fieldName);
}

void EgDatabaseType::CommitNodeLayoutBlueprintClass() {
    if (CreateNodeTypeFlag)
    {
        newLayout-> LayoutInitCommit();
        std::cout << "newLayout after: " << newLayout->layoutName << " " << (int) newLayout-> layoutID << std::endl;
        StoreTypesInfo();
        delete newLayout;
        newLayout = nullptr;
    }
}

int EgDatabaseType::AddLinkType(std::string& linkTypeName) { //,  EgNodeTypeSettings& typeSettings)
    if (! checkLinknameExist(linkTypeName)) { // check nodestype/layout name
        std::cout << "ERROR: Links type/layout name already exists, cant AddLinkType()" << std::endl;
        return -1;
    }

    AddLinksTypeInfo(linkTypeName); // FIXME STUB
    StoreLinksInfo();

    return 0;
}