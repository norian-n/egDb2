#include <iostream>
#include "egDataNodeLayout.h"

EgDataNodeIDType EgDataNodeLayoutType::getNextID() {
    nextIDchanged = true;
    return nextNodeID++;
}

int EgDataNodeLayoutType::AddDataFieldsNames(std::vector<std::string> &fieldsNames, EgNodeLayoutSettingsType &settings) {
    if (LayoutInitStart()) // commit not initiated
        return -1;
    for (auto fieldName : fieldsNames)
        AddDataFieldName(fieldName);
    layoutSettings = settings;
    LayoutInitCommit();
    return 0;
}

int EgDataNodeLayoutType::LayoutInitStart() {
    layoutFile.fileName = layoutName + ".dnl";
    if (layoutFile.checkIfExists())
    {
        std::cout << "ERROR: Can't create DataNodeLayout: \"" << layoutName << "\" - file already exists, use data pumping" << std::endl;
        return -1;
    }
    else
    {
        fieldsCount = 0;
        dataFieldsNames.clear();
        indexedFields.clear();
        layoutMode = egLayoutInit;
    }
    return 0;
}

void EgDataNodeLayoutType::AddDataFieldName(std::string fieldName) {
    if (layoutMode != egLayoutInit)
        std::cout << "ERROR: Can't add field \"" << fieldName << "\" to DataNodeLayout: \"" << layoutName << "\". Call LayoutInitStart() first" << std::endl;
    else
    {
        EgFieldsCountType order = (EgFieldsCountType)dataFieldsNames.size();
        dataFieldsNames.insert(std::make_pair(fieldName, order));
        fieldsCount++;
    }
    // std::cout  << "AddDataField name: " << fieldName << " index: " << order << std::endl;
}

void EgDataNodeLayoutType::LayoutInitCommit() {
    if (layoutMode == egLayoutInit)
    {
        LocalStoreLayout();
        layoutMode = egLayoutActive;
    }
    else
        std::cout << "ERROR: Can't commit fields to DataNodeLayout: \"" << layoutName << "\". Call LayoutInitStart() first" << std::endl;
}

void EgDataNodeLayoutType::AddIndex(std::string fieldName, EgIndexSettingsType &indexSet) {
    auto iter = dataFieldsNames.find(fieldName);
    if (iter != dataFieldsNames.end())
    {
        indexedFields.insert(std::make_pair(iter->second, indexSet));
    }
    else
        std::cout << "ERROR: AddIndex for DataNodeLayout: \"" << layoutName << "\" field name \"" << fieldName << "\" not found" << std::endl;
}

inline void EgDataNodeLayoutType::writeDataFieldsNames() {
    layoutFile.writeType<EgFieldsCountType>((EgFieldsCountType)dataFieldsNames.size());
    for (auto fieldsIter : dataFieldsNames)
    { // 17 [first, second], <11 = dataFieldsNames.begin(); fieldsIter != dataFieldsNames.end(); ++fieldsIter) {
        // layoutFile.writeType<EgFieldNameLengthType>((EgFieldNameLengthType) fieldsIter.first.size());
        layoutFile.writeType<EgStrSizeType>((EgStrSizeType)fieldsIter.first.size());
        layoutFile.fileStream << fieldsIter.first;
        layoutFile.writeType<EgFieldsCountType>((EgFieldsCountType)fieldsIter.second);
    }
}

inline void EgDataNodeLayoutType::readDataFieldsNames() {
    EgFieldsCountType fieldsCountTmp{0};
    EgFieldsCountType order{0};
    std::string fieldName;
    layoutFile.readType<EgFieldsCountType>(fieldsCountTmp);
    // std::cout << "fieldsCountTmp: " << std::dec << (int) fieldsCountTmp << std::endl;
    for (EgFieldsCountType i = 0; i < fieldsCountTmp; i++)
    {
        fieldName.clear();
        layoutFile >> fieldName;
        layoutFile.readType<EgFieldsCountType>(order);
        // std::cout << "fieldName: " << fieldName << std::endl;
        dataFieldsNames.insert(std::make_pair(fieldName, order));
    }
}

inline void EgDataNodeLayoutType::writeIndexedFields() {
    layoutFile.writeType<EgFieldsCountType>((EgFieldsCountType)indexedFields.size());
    for (auto fieldsIter : indexedFields)
    { // [first, second] = dataFieldsNames.begin(); fieldsIter != dataFieldsNames.end(); ++fieldsIter) {
        layoutFile.writeType<EgFieldsCountType>((EgFieldsCountType)fieldsIter.first);
        layoutFile << fieldsIter.second.indexFamilyType;
        layoutFile << fieldsIter.second.indexSizeBits;
        layoutFile << fieldsIter.second.hashFunctionID;
    }
}

inline void EgDataNodeLayoutType::readIndexesFields() {
    EgFieldsCountType fieldsCountTmp{0};
    EgFieldsCountType index{0};
    EgIndexSettingsType indexSettings;
    layoutFile.readType<EgFieldsCountType>(fieldsCountTmp);
    for (EgFieldsCountType i = 0; i < fieldsCountTmp; i++)
    {
        layoutFile.readType<EgFieldsCountType>(index);
        layoutFile >> indexSettings.indexFamilyType;
        layoutFile >> indexSettings.indexSizeBits;
        layoutFile >> indexSettings.hashFunctionID;
        indexedFields.insert(std::make_pair(index, indexSettings));
    }
}

int EgDataNodeLayoutType::LocalStoreLayout() {
    layoutFile.fileName = layoutName + ".dnl";
    layoutFile.openToWrite();

    layoutFile << fieldsCount;
    layoutFile << nodesCount;
    layoutFile << nextNodeID;

    layoutFile << layoutSettings.isServiceType;
    layoutFile << layoutSettings.useLinks;
    layoutFile << layoutSettings.useSubGraph;
    layoutFile << layoutSettings.useEntryNodes;
    layoutFile << layoutSettings.useVisualSpace;
    layoutFile << layoutSettings.useNamedAttributes;
    layoutFile << layoutSettings.useGUIsettings;

    writeDataFieldsNames();
    writeIndexedFields();

    layoutFile.close();
    return 0;
}

int EgDataNodeLayoutType::LocalLoadLayout() {
    clear();

    layoutFile.fileName = layoutName + ".dnl";
    if (!layoutFile.openToRead())
        return -1;

    layoutFile >> fieldsCount;
    layoutFile >> nodesCount;
    layoutFile >> nextNodeID;

    layoutFile >> layoutSettings.isServiceType;
    layoutFile >> layoutSettings.useLinks;
    layoutFile >> layoutSettings.useSubGraph;
    layoutFile >> layoutSettings.useEntryNodes;
    layoutFile >> layoutSettings.useVisualSpace;
    layoutFile >> layoutSettings.useNamedAttributes;
    layoutFile >> layoutSettings.useGUIsettings;

    // std::cout << "Names file pos: " << std::hex << (int) layoutFile.fileStream.tellg() << std::endl;
    readDataFieldsNames();
    readIndexesFields();

    layoutFile.close();
    return 0;
}

// ======================== Debug ========================

void PrintDataNodeLayout(EgDataNodeLayoutType& layout) {
    std::cout << "fieldsCount: " << std::dec << layout.fieldsCount;    
    std::cout << "nodesCount: " << std::dec << layout.nodesCount;
    std::cout << " nextNodeID: " << layout.nextNodeID << std::endl;

    std::cout << "isServiceType: " << layout.layoutSettings.isServiceType;
    std::cout << " useLinks: " << layout.layoutSettings.useLinks;
    std::cout << " useSubGraph: " << layout.layoutSettings.useSubGraph;
    std::cout << " useEntryNodes: " << layout.layoutSettings.useEntryNodes;
    std::cout << " useVisualSpace: " << layout.layoutSettings.useVisualSpace;
    std::cout << " useNamedAttributes: " << layout.layoutSettings.useNamedAttributes;
    std::cout << " useGUIsettings: " << layout.layoutSettings.useGUIsettings << std::endl;
    // fields
    std::cout << "Fields (" << layout.dataFieldsNames.size() << "):" << std::endl;
    for (auto fieldsIter : layout.dataFieldsNames)
    { // [first, second] = dataFieldsNames.begin(); fieldsIter != dataFieldsNames.end(); ++fieldsIter) {
        std::cout << "fieldName: \"" << fieldsIter.first;
        std::cout << "\" fieldIndex: " << (int)fieldsIter.second;
        std::cout << std::endl;
    }
    // indexes
    std::cout << "Indexes (" << layout.indexedFields.size() << "):" << std::endl;
    for (auto indIter : layout.indexedFields)
    { // [first, second] = indexedFields.begin(); indIter != indexedFields.end(); ++indIter) {
        std::cout << "index fieldNum: \"" << (int)indIter.first;
        std::cout << " index indexFamilyType: " << indIter.second.indexFamilyType;
        std::cout << " index indexSizeBits: " << indIter.second.indexSizeBits;
        std::cout << " index hashFunctionID: " << indIter.second.hashFunctionID;
        std::cout << std::endl;
    }
}