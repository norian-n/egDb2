#pragma once
#include <vector>
#include <map>
#include <iostream>

#include "../core/egCoreTypes.h"
#include "../indexes/egCoreIndexTypes.h"
#include "../service/egFileType.h"
#include "../service/egHamSlicer.h"

//  ============================================================================
//          EXTERNAL TYPES
//  ============================================================================

class EgDatabaseType;          // peer database (graph)

//  ============================================================================

enum EgLayoutModeEnum
{
    egLayoutInit,
    egLayoutActive,
    egLayoutDataPump    // TBD: special data copy procedure with parameters "from field => to field", indexes rebuilding
};

struct EgNodeLayoutSettingsType
{
    bool isServiceType      { false };  // hardlinked by ID to primary data nodes container
    bool useLinks           { false };  // some service node layouts don't use links so it's optional
    bool useSubGraph        { false };  // add detail nodes inside this node
    bool useEntryNodes      { false };  // start points for custom graph operations
    bool useVisualSpace     { false };  // visual space node apperiance info 
    bool useGUIsettings     { false };  // store GUI elements features for this nodes layout
    bool useNamedAttributes { false };  // flexible named attributes/properties/tags
};

class EgDataNodeLayoutType
{
public:
    EgLayoutModeEnum layoutMode {egLayoutActive};   // layout can't be edited freely, data pumping on changes is required

    EgLayoutIDType      layoutID;                  // Data Nodes Layout (DNL) ID for graph DB
    std::string         layoutName;                // == dataNodesTypeName

    EgDataNodeIDType    nodesCount      {0};        // data nodes count
    EgDataNodeIDType    nextNodeID      {1};        // next available data node ID
    bool                nextIDchanged   {false};    // write to file on commit

    EgHamSlicerType     theHamSlicer;    // {nullptr};  // FIXME STUB move to data nodes type handler

    // EgDatabaseType*         myECoGraphDB        {nullptr};      // peer database (graph)
    // EgServerConnection*     serverConnection    {nullptr};      // server load & store support ( if data on server )
    EgFileType                  layoutFile;                        // local files load & store support (if local files used)    

    EgNodeLayoutSettingsType    layoutSettings;                    // add-ons for layout type

    std::map < std::string, EgFieldsCountType >           dataFieldsNames;    // map  data node field names to fields order
    std::map < EgFieldsCountType, EgIndexSettingsType >   indexedFields;      // map indexed fields names to structure

    EgDataNodeLayoutType(std::string a_layoutName): layoutName(a_layoutName), layoutFile(a_layoutName) {}
    ~EgDataNodeLayoutType() { clear(); }

    void clear() { dataFieldsNames.clear(); indexedFields.clear(); }

    EgDataNodeIDType getNextID();
    int AddDataFieldsNames(std::vector<std::string>& fieldsNames, EgNodeLayoutSettingsType& settings);
    int LayoutInitStart();
    void AddDataFieldName(std::string fieldName);
    void LayoutInitCommit();
    void AddIndex(std::string fieldName, EgIndexSettingsType& indexSet);
    inline void writeDataFieldsNames();
    inline void readDataFieldsNames();
    inline void writeIndexedFields();
    inline void readIndexesFields();
    int  LocalStoreLayout();
    int  LocalLoadLayout();
};

// ======================== Debug ========================

void PrintDataNodeLayout(EgDataNodeLayoutType& layout);