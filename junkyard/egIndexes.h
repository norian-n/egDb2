/*
#pragma once
#include <iostream> // debug
#include <fstream>
#include <set>
#include <vector>
#include <experimental/filesystem> 
#include "egCoreIndexTypes.h"
#include "../service/egByteArray.h"
#include "../service/egFileType.h"
template <typename KeyType> class EgFingers;
template <typename KeyType> class EgIndexes { public: // indexes chain part of indexes-fingers complex
    typedef bool (*CompareFunctionType) (KeyType&, KeyType&);
    const uint64_t            indexHeaderSize       = sizeof(uint64_t);
    const egMaxStreamSizeType oneIndexSize          = sizeof(KeyType) + sizeof(uint64_t); // key and data offset
    const egMaxStreamSizeType fwdPtrPosition        = egIndexesNamespace::egChunkCapacity * oneIndexSize;
    const egMaxStreamSizeType backPtrPosition       = fwdPtrPosition   + sizeof(uint64_t);
    const egMaxStreamSizeType chunkCountPosition    = backPtrPosition  + sizeof(uint64_t);
    const egMaxStreamSizeType fingersChunkOffsetPosition  = chunkCountPosition + sizeof(keysCountType);
    const egMaxStreamSizeType indexChunkSize        =  fingersChunkOffsetPosition + sizeof(uint64_t);
    bool fingersChainFlag {true};
    KeyType  theKey;                // index key to process
    uint64_t theDataOffset;         // offsets in data nodes file
    uint64_t theNewDataOffset;
    uint64_t theOldDataOffset;
    keysCountType theChunkCount;    // indexes count in the chunk for chain opers
    uint64_t theIndexesChunkOffset; // file position for chain connect
    int      theIndexPosition;      // position in the chunk
    uint64_t prevOffsetPtr;         // chunks chain operations
    uint64_t nextOffsetPtr;
    EgFingers<KeyType>*     fingersTree {nullptr};  // fingers tree object ptr
    egDataStream*           localStream {nullptr};  // chunk buffer operations
    EgIndexStruct<KeyType>  indexData;              // index data wrapper for flexibility
    EgFileType              indexFileStream;        // file operations
    std::string             indexFileName;
    EgIndexes(std::string a_indexName);
    ~EgIndexes() { delete localStream; }
        // top API
    bool AddNewIndex(KeyType& key, uint64_t dataOffset);
    bool DeleteIndex(KeyType& key, uint64_t dataOffset);
    bool UpdateDataOffset(KeyType& key, uint64_t oldDataOffset, uint64_t newDataOffset); // UpdateDataOffsetForIndex
        // service ops
    inline void DeleteIndexInChunk();
    inline bool RemoveChunkFromChain();
    inline void DeleteIndexesChunk(uint64_t indexesChunkOffset);    
    inline void ReadIndexAndOffset(KeyType& currentIndex, uint64_t& dataOffset, int indexPosition);
    inline bool FindIndexByDataOffset();
    inline void InitIndexChunk(KeyType& key, uint64_t& dataOffset);
    // int FindIndexByDataOffset(bool isPrimary); // FIXME refactor
    // int InsertToIndexChunk();
    // void UpdateDataOffsetForIndex(bool isPrimary = false);
    // int DeleteIndex(bool isPrimary = false);
        // load data
    void LoadDataUp2(std::set<uint64_t>& index_offsets);

    void LoadAllData(std::set<uint64_t>& index_offsets);
    void LoadDataUp(std::set<uint64_t>& index_offsets);
    int LoadDataByChunkUp(std::set<uint64_t>& index_offsets, CompareFunctionType myCompareFunc);
    int LoadDataByChunkDown(std::set<uint64_t>& index_offsets, CompareFunctionType myCompareFunc);
    int LoadDataByChunkEqual(std::set<uint64_t>& index_offsets);
        // files
    inline void WriteRootHeader(); // meta-info of indexes (first chunk for loadAll, empty chain(TBD)), also for non-zero offset
    inline bool CheckIfIndexFileExists(){ return indexFileStream.checkIfExists(); }
    inline bool OpenIndexFileToRead()   { return indexFileStream.openToRead(); }
    inline bool OpenIndexFileToWrite()  { return indexFileStream.openToWrite(); }
    inline bool OpenIndexFileToUpdate() { return indexFileStream.openToUpdate(); }
    inline bool DeleteIndexesFile();
        // chunk ops
    inline bool StoreIndexChunk(const uint64_t nextChunkOffset);  // from localStream-> bufData
    int LoadIndexChunk(uint64_t chunkOffset);             // to localStream-> bufData
    // inline keysCountType GetCountFromChunk();
    inline void GetCountFromChunk(keysCountType& refCount);
    inline void SetCountOfChunk(keysCountType chunkCount);
    inline uint64_t GetFingersChunkOffset();
    inline void MoveTailToInsert(char* chunkPtr, int indexPosition);

    // int LoadIndexChunk(char *chunkPtr, uint64_t chunkOffset);    // by indexesChunkOffset
    // int LoadIndexChunk();        // default: loads fingersTree-> currentFinger.nextChunkOffset to indexBA.data()
    int StoreIndexChunk(unsigned char* chunkPtr, const uint64_t nextChunkOffset);
    inline int StoreIndexChunk();       // default: indexBA.constData() to fingersTree-> currentFinger.nextChunkOffset
        // service ops
    inline void InitMinMaxFlags();
    inline void SetMinMaxOfFinger(KeyType& key);
    // int StoreFingerOffset(uint64_t fingerOffset);
    int StoreFingerOffset(uint64_t indexesChunkOffset, uint64_t fingersChunkOffset);   // update finger backptr
   
   void GetFingerOffset(uint64_t& fingerOffset);

    inline void GetKeyByFileOffset(uint64_t chunkOffset, int indexPosition, KeyType& key);

    inline void FindIndexPositionToInsert(KeyType& key);

    inline void ReadIndexValues(EgIndexStruct<KeyType>& indexStruct);
    inline void WriteIndexValues(EgIndexStruct<KeyType>& indexStruct, int position);

    inline void ReadIndexOnly(KeyType& currentIndex, int indexPosition);
    inline void ReadIndexOnly(KeyType& currentIndex);    
    inline void UpdateChunkCount(keysCountType newCount);

    inline void InsertInsideIndexChunk(KeyType& key, uint64_t& dataOffset);
    inline void WriteIndexValues(KeyType key, uint64_t dataOffset, int position);    


    // int SplitIndexChunk();

    inline void SaveLastIndex(KeyType& key, uint64_t& dataOffset);
    inline void GetChainPointers(uint64_t& nextPtr, uint64_t& prevPtr);
    inline bool AppendIndexChunk(KeyType& key, uint64_t& dataOffset, uint64_t& fwdPtr, uint64_t& backPtr);
        // load data nodes
    int FindPosByKeyFirst(CompareFunctionType myCompareFunc);
    int FindPosByKeyLast(CompareFunctionType myCompareFunc);

    // int DeleteIndexInChunk_old();
    // void RemoveChunkFromChain_old();
        // compare
    static bool CompareEQ (KeyType& currentIndex, KeyType& key) {return (currentIndex == key);}
    static bool CompareGT (KeyType& currentIndex, KeyType& key) {return (currentIndex > key);}
    static bool CompareGE (KeyType& currentIndex, KeyType& key) {return (currentIndex >= key);}
    static bool CompareLT (KeyType& currentIndex, KeyType& key) {return (currentIndex < key);}
    static bool CompareLE (KeyType& currentIndex, KeyType& key) {return (currentIndex <= key);}

    inline bool checkIndexesChainFwd(uint64_t &doubleSpeedOffset);     // check indexes chain for loops
    inline bool checkIndexesChainBack();

    // bool checkIndexesIntegrity();  // don't use it inside operations
    // void PrintIndexesChunk(char* theChunk, const QString& theMessage);  // debug
};
*/