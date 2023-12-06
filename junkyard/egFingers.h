    // uint64_t directFileOffset;  // == myChunkOffset + myPosInChunk
    // uint64_t parentPosInChunk;  // parent finger position in the chunk
    // fingersLevelType myLevel;   // (FIXME obsolete)
    // KeyType newMinValue;
    // KeyType newMaxValue;
    // uint64_t parentFingerOffset;
    // char* fingersChunk;     // current fingers chunk buffer
    // std::string IndexFileName; // copy for debug messages
    // uint64_t currentFingerOffset;
    // keysCountType currentKeysCount;

    // StoreFingerOffset(newFinger.nextChunkOffset, newFinger.nextChunkOffset);
    // bool LoadFingerDirect   (egFinger<KeyType>& theFinger, const uint64_t fingerOffset);
    // bool StoreFingerMinMaxOnly (egFinger<KeyType>& theFinger);
    // bool LoadParentChunkOffsetDirect(egFinger<KeyType>& theFinger);
    // inline void UpdateBackptrOffsets(uint64_t myChunkOffset,  int posToInsert, int itemsCount); // based on currentFinger
    // int  DeleteCurrentFinger();       // uses current // ex UpdateFingersChainAfterDelete
    // int  DeleteParentFinger(); // FIXME check

    // int UpdateFingersChainAfterInsert(); // moved to top API 
    // int UpdateFingersChain();  // FIXME level num
    // int UpdateFingersChainAfterDelete(); // FIXME check if needed & refactor
    // bool UpdateFingerCountAfterDelete(keysCountType newKeysCount); // FIXME root level check if required
    // int UpdateMinValueUp(); // FIXME  root level
    // int UpdateMaxValueUp(); // FIXME  root level
    // inline void UpdateMinValueInFile(uint64_t fingerOffset, KeyType& newValue);
    // inline void UpdateMaxValueInFile(uint64_t fingerOffset, KeyType& newValue);
    // inline void UpdateTheFingerMinMax(egFinger<KeyType>& theFinger);

    // void PrintChunkInfo();
    // void PrintFingersChunk(char* theFingersChunk, const QString& theMessage);
    // bool checkFingersTreeIntegrity();  // don't use it inside operations
    // void PrintAllChunksInfo(const QString& theMessage);

// JUNK TODO clean
    // void LoadRootFinger();
    // void StoreRootFinger();
    // void StoreRootFingerMinMaxOnly();
    // void InitFinger (egFinger<KeyType>& theFinger);

    // template <typename KeyType> class EgIndexes;
/*
template <typename KeyType> class EgIndexes {
public:
    void StoreFingerOffset(uint64_t Offset1, uint64_t Offset2) {}
};
*/