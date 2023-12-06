#include "../nodes/egDataNodeLayout.h"
#include <iostream>
#include <cstring>
using namespace std;


bool testDataNodeLayout() {
    EgDataNodeLayoutType testLayout("testNodes");
    cout << "===== Test DataNodeLayout (2 errors must appear in negative tests) " << " =====" << endl;

    EgIndexSettingsType indexSettings;

    std::remove("testNodes.dnl"); // delete file

    testLayout.LayoutInitStart();

    testLayout.AddDataFieldName("field_1");
    testLayout.AddDataFieldName("testField 2");
    testLayout.AddDataFieldName("field 3");

    testLayout.layoutSettings.useEntryNodes         = true;
    testLayout.layoutSettings.useGUIsettings        = true;
    testLayout.layoutSettings.useLinks              = true;
    testLayout.layoutSettings.useNamedAttributes    = true;
    testLayout.layoutSettings.useVisualSpace        = true;

    testLayout.LayoutInitCommit();
    testLayout.AddDataFieldName("try to add field after commit");

    indexSettings.indexFamilyType   = egIntFT;
    indexSettings.indexSizeBits     = 32;
    testLayout.AddIndex("field_1",  indexSettings);

    indexSettings.indexFamilyType   = egHashFT;
    indexSettings.indexSizeBits     = 64;
    indexSettings.hashFunctionID    = 2;
    testLayout.AddIndex("field 3",  indexSettings);

    testLayout.AddIndex("bad index name", indexSettings);
    
    testLayout.LocalStoreLayout();
    int res = testLayout.LocalLoadLayout();
    // PrintDataNodeLayout(testLayout);

    return (res == 0);
}

int main() {
    if (testDataNodeLayout())
        cout << "PASS" << endl;
    else
        cout << "FAIL" << endl;    
}