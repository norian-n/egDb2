/*

#include "egIndexes.h"
#include "egFingers.h"

using namespace egIndexesNamespace;

template <typename KeyType> EgIndexes<KeyType>::EgIndexes(std::string a_indexName) :
        fingersTree(new EgFingers<KeyType>(a_indexName, this)),
        indexFileName(a_indexName + ".ind"),
        indexFileStream(a_indexName + ".ind"),
        localStream(new egDataStream(indexChunkSize))
{ }

template <typename KeyType> bool EgIndexes<KeyType>::AddNewIndex(KeyType& key, uint64_t dataOffset) {
    bool res {true};
    if(! CheckIfIndexFileExists()) { // add new index and finger FIXME TODO files
        res = res && OpenIndexFileToWrite();
        if(res) {
            WriteRootHeader();
            InitIndexChunk(key, dataOffset);
            res = res && StoreIndexChunk(indexHeaderSize);
            if(res) {
                if (fingersTree-> CheckIfFingerFileExists()) 
                    fingersTree-> DeleteFingersFile();
                res = res && fingersTree-> AddNewRootFinger(key, indexHeaderSize);
            }
        }
    } else {
        res = res && fingersTree-> FindIndexesChunkToInsert(key);
        if(res) {
            res = res && OpenIndexFileToUpdate();
            // EG_LOG_STUB << "currentFinger.nextChunkOffset(hex) = " << std::hex << (int) fingersTree-> currentFinger.nextChunkOffset << FN;
            LoadIndexChunk(fingersTree-> currentFinger.nextChunkOffset); // sets theChunkCount
            // EG_LOG_STUB << "theChunkCount = " << std::dec << (int) theChunkCount << FN;
            if(theChunkCount < egChunkCapacity) {
                InsertInsideIndexChunk(key, dataOffset);
                theChunkCount++;
                SetCountOfChunk(theChunkCount);
                res = res && StoreIndexChunk(fingersTree-> currentFinger.nextChunkOffset);
                if(res) { // TODO min max
                    SetMinMaxOfFinger(key);
                    res = res && fingersTree-> UpdateFingersChainUp();
                }
            } else {
                KeyType savedKey;
                uint64_t savedDataOffset, savedFwdPtr, savedBackPtr;
                uint64_t newChunkOffset = indexFileStream.getFileSize();
                GetChainPointers(savedFwdPtr, savedBackPtr);
                // EG_LOG_STUB << "fwdPtr = " << std::hex << savedFwdPtr << ", backPtr = " << savedBackPtr << FN;
                if (key >= fingersTree-> currentFinger.maxKey) { // add last
                    localStream->seek(fwdPtrPosition); // update fwdptr to new chunk
                    *localStream << newChunkOffset;
                    res = res && StoreIndexChunk(fingersTree->currentFinger.nextChunkOffset);
                    res = res && AppendIndexChunk(key, dataOffset, savedFwdPtr, fingersTree-> currentFinger.nextChunkOffset);
                } else {
                    SaveLastIndex(savedKey, savedDataOffset);
                    // EG_LOG_STUB << "savedKey = " << std::hex << (int) savedKey << ", savedDataOffset = " << savedDataOffset << FN;
                    InsertInsideIndexChunk(key, dataOffset);
                    localStream->seek(fwdPtrPosition); // update fwdptr to new chunk
                    *localStream << newChunkOffset;
                    ReadIndexOnly(fingersTree->currentFinger.minKey, 0); // update fimger values
                    ReadIndexOnly(fingersTree->currentFinger.maxKey, egChunkCapacity-1);
                    res = res && StoreIndexChunk(fingersTree->currentFinger.nextChunkOffset);
                    fingersTree-> newFinger.nextChunkOffset = newChunkOffset; // moved up from function to optimize calls
                    res = res && AppendIndexChunk(savedKey, savedDataOffset, savedFwdPtr, fingersTree-> currentFinger.nextChunkOffset);
                }
                res = res && fingersTree->AddNewUpdateCurrentFinger();
            }
        }
    }
    indexFileStream.close();
    return res;
}

template <typename KeyType> inline void EgIndexes<KeyType>::DeleteIndexInChunk() {
    if (theIndexPosition < (theChunkCount - 1)) // move tail
        memmove (localStream-> bufData + theIndexPosition*oneIndexSize, localStream-> bufData + (theIndexPosition+1)*oneIndexSize,  
                 oneIndexSize*(theChunkCount - theIndexPosition - 1));
    memset(localStream-> bufData + (theChunkCount - 1)*oneIndexSize, 0, oneIndexSize); // erase last
}

template <typename KeyType> inline bool EgIndexes<KeyType>::RemoveChunkFromChain() {
    uint64_t savedDataOffset, savedFwdPtr, savedBackPtr;
    GetChainPointers(savedFwdPtr, savedBackPtr);
    // EG_LOG_STUB << "savedFwdPtr = " << std::hex << savedFwdPtr << ", savedBackPtr = " << savedBackPtr << FN;
    // if (! savedFwdPtr && ! savedBackPtr) // FIXME check if last chunk, delete files
    if (savedBackPtr) {        
        indexFileStream.seekWrite(savedBackPtr + fwdPtrPosition);
        indexFileStream << savedFwdPtr;
    } else { // update header
        indexFileStream.seekWrite(0);
        indexFileStream << savedFwdPtr;
    }
    if (savedFwdPtr)
    {
        indexFileStream.seekWrite(savedFwdPtr + backPtrPosition);
        indexFileStream << savedBackPtr;
    }
    return indexFileStream.good();
}

template <typename KeyType> inline void EgIndexes<KeyType>::DeleteIndexesChunk(uint64_t indexesChunkOffset) {
    uint64_t fileSize = indexFileStream.getFileSize();
    // EG_LOG_STUB << "fileSize = " << std::dec << (int) fileSize <<"fingersChunkOffset + indexChunkSize = " << fingersChunkOffset + indexChunkSize << FN;
    if (indexesChunkOffset + indexChunkSize == fileSize) { // check if last chunk to delete
        keysCountType chunkCount {0};
        int otherEmptyChunks{0}; // check if other chunks are empty
        int backCount{1};
        while (!chunkCount && (indexChunkSize * backCount < indexesChunkOffset) && indexFileStream.good()) {
            indexFileStream.seekRead(indexesChunkOffset - (indexChunkSize * backCount) + chunkCountPosition);
            indexFileStream >> chunkCount;
            // EG_LOG_STUB << "chunkCount = " << std::dec << (int) chunkCount << FN;
            // EG_LOG_STUB << "offset shift = " << std::hex << indexChunkSize * backCount << FN;
            if (!chunkCount)
                otherEmptyChunks++;
            backCount++;
        }
        // EG_LOG_STUB << "otherEmptyChunks = " << std::dec << otherEmptyChunks << FN;
        indexFileStream.close();
        std::experimental::filesystem::path filename = indexFileName.c_str();
        std::experimental::filesystem::resize_file(filename, indexesChunkOffset - (indexChunkSize * otherEmptyChunks));
        indexFileStream.openToUpdate();
    } else {
        memset(localStream-> bufData, 0, indexChunkSize); // init zero chunk
        StoreIndexChunk(indexesChunkOffset);
    }
}

template <typename KeyType> bool EgIndexes<KeyType>::DeleteIndex(KeyType& key, uint64_t dataOffset) {
    bool res {true};
    theKey = key;
    theDataOffset = dataOffset;
    res = res && fingersTree-> FindIndexChunkEQ(key); // get currentFinger
    if (res) {
        res = res && OpenIndexFileToUpdate();
        if (res) {
            res = res && FindIndexByDataOffset(); // uses currentFinger, theKey and theDataOffset sets theIndexesChunkOffset, theIndexPosition, fingersChainFlag
            if (res) {
                if (! fingersChainFlag) {
                    fingersTree-> currentFinger.myChunkOffset = GetFingersChunkOffset();
                    fingersTree-> currentFinger.nextChunkOffset = theIndexesChunkOffset;
                }
                if (theChunkCount > 1) {  // not last index in the chunk
                    DeleteIndexInChunk(); // set currentFinger for update
                    theChunkCount--;
                    SetCountOfChunk(theChunkCount);
                    res = res && StoreIndexChunk(theIndexesChunkOffset);
                    ReadIndexOnly(fingersTree->currentFinger.minKey, 0); // update fimger values
                    ReadIndexOnly(fingersTree->currentFinger.maxKey, theChunkCount-1);
                    if (fingersChainFlag)
                        fingersTree-> UpdateFingersChainUp();
                    else
                        fingersTree-> UpdateFingersByBackptrs(); // FIXME set minMaxUpdated
                } else {
                    RemoveChunkFromChain();
                    DeleteIndexesChunk(theIndexesChunkOffset);
                    if (fingersChainFlag)
                        res = res && fingersTree-> DeleteCurrentFingerByChain();
                    else
                        res = res && fingersTree-> DeleteFingerByBackptrs();
                }
            }
        }
    }
    indexFileStream.close();
    return res;
}

template <typename KeyType> bool EgIndexes<KeyType>::UpdateDataOffset(KeyType& key, uint64_t oldDataOffset, uint64_t newDataOffset) {
    bool res {true};
    theKey = key;
    theDataOffset = oldDataOffset;
    res = res && fingersTree-> FindIndexChunkEQ(key); // get currentFinger
    if (res) {
        res = res && OpenIndexFileToUpdate();
        if (res) {
            res = res && FindIndexByDataOffset(); // sets theIndexesChunkOffset, theIndexPosition
            if (res) {
                indexFileStream.seekWrite(theIndexesChunkOffset + theIndexPosition*oneIndexSize + sizeof(KeyType));
                indexFileStream << newDataOffset;
                res = res && indexFileStream.good();
            }
        }
    }
    fingersTree-> fingersChain.clear();
    fingersTree-> fingersFileStream.close();
    indexFileStream.close();
    return res;
}

template <typename KeyType> inline void EgIndexes<KeyType>::WriteRootHeader() {
    indexFileStream.seekWrite(0);
    uint64_t localHeaderSize {indexHeaderSize};
    indexFileStream << localHeaderSize;       // first chunk offset
    // uint64_t placeholder { 0xAFAFAFAF };
    // indexFileStream << placeholder; // reserved for chunks recycling list
    // return indexFileStream.good();  // paranoid mode
}

template <typename KeyType> inline void EgIndexes<KeyType>::InitIndexChunk(KeyType& key, uint64_t& dataOffset) {
    keysCountType initCount {1};
    memset(localStream-> bufData, 0, indexChunkSize);
    localStream-> seek(0);          // write first key of all
    *localStream << key;
    *localStream << dataOffset;  // data node offset
    localStream-> seek(chunkCountPosition); // write indexes count
    *localStream << initCount;
}

template <typename KeyType> inline void EgIndexes<KeyType>::InitMinMaxFlags() {
    fingersTree-> anyMinMaxChanged = false;
    // fingersTree-> minValueChanged = false;
    // fingersTree-> maxValueChanged = false;
}

template <typename KeyType> inline void EgIndexes<KeyType>::SetMinMaxOfFinger(KeyType& key) {
    // EG_LOG_STUB << "key = " << std::hex << (int) key << FN;
    // fingersTree-> PrintFingerInfo(fingersTree-> currentFinger, "currentFinger");
    if (key < fingersTree-> currentFinger.minKey)
        fingersTree-> currentFinger.minKey = key;
    else if (key > fingersTree-> currentFinger.maxKey)
        fingersTree-> currentFinger.maxKey = key;
}

template <typename KeyType> int EgIndexes<KeyType>::StoreFingerOffset(uint64_t indexesChunkOffset, uint64_t fingersChunkOffset)
{
    indexFileStream.seekWrite(indexesChunkOffset + fingersChunkOffsetPosition);
    indexFileStream << fingersChunkOffset;

    return 0; // FIXME add paranoid mode
}

template <typename KeyType> void EgIndexes<KeyType>::GetFingerOffset(uint64_t& fingerOffset) {
    localStream-> seek(fingersChunkOffsetPosition);
    *localStream >> fingerOffset;
}

template <typename KeyType> inline void EgIndexes<KeyType>::GetKeyByFileOffset(uint64_t chunkOffset, int indexPosition, KeyType& key)
{
    indexFileStream.seekRead(chunkOffset + indexPosition * oneIndexSize);
    indexFileStream >> key;
}

template <typename KeyType> inline int EgIndexes<KeyType>::LoadIndexChunk(uint64_t chunkOffset) {
    indexFileStream.seekRead(chunkOffset);
    // if (! indexFileStream.seek(fingersTree-> currentFinger.nextChunkOffset)) // FIXME implement paranoid mode
    //    EG_LOG_STUB << "ERROR: bad position of fingersTree-> currentFinger.nextChunkOffset = " << std::hex << fingersTree-> currentFinger.nextChunkOffset << FN;
    indexFileStream.fileStream.read((char *) localStream-> bufData, indexChunkSize);
    GetCountFromChunk(theChunkCount);
    return indexFileStream.good();
}

template <typename KeyType> inline void EgIndexes<KeyType>::GetCountFromChunk(keysCountType& refCount) {
    localStream  -> seek(chunkCountPosition);
    *localStream >> refCount;
}

template <typename KeyType> inline void EgIndexes<KeyType>::SetCountOfChunk(keysCountType chunkCount) {
    localStream  -> seek(chunkCountPosition);
    *localStream << chunkCount;
}


template <typename KeyType> inline uint64_t EgIndexes<KeyType>::GetFingersChunkOffset() {
    uint64_t fingersChunkOffset {0};
    localStream  -> seek(fingersChunkOffsetPosition);
    *localStream >> fingersChunkOffset;
    return fingersChunkOffset;
}

template <typename KeyType> inline bool EgIndexes<KeyType>::StoreIndexChunk(const uint64_t chunkOffset) {
    // PrintIndexesChunk(localStream-> bufData, "chunk to save " + FNS);
    indexFileStream.seekWrite(chunkOffset); // FIXME check result in paranoid mode
    // EG_LOG_STUB << "seek result = " << seekRes << "fingersTree-> currentFinger.nextChunkOffset = " << std::hex << (int) fingersTree-> currentFinger.nextChunkOffset << FN;
    indexFileStream.fileStream.write((char *) localStream-> bufData, indexChunkSize);
    return indexFileStream.good();
}

template <typename KeyType> int EgIndexes<KeyType>::StoreIndexChunk(unsigned char* chunkPtr, const uint64_t chunkOffset)
{
    // PrintIndexesChunk(localStream-> bufData, "chunk to save " + FNS);

    indexFileStream.seekWrite(chunkOffset); // FIXME check result in paranoid mode

    // EG_LOG_STUB << "seek result = " << seekRes << "fingersTree-> currentFinger.nextChunkOffset = " << std::hex << (int) fingersTree-> currentFinger.nextChunkOffset << FN;

    indexFileStream.fileStream.write((char *) chunkPtr, indexChunkSize);
    return indexFileStream.good();
}

template <typename KeyType> inline int EgIndexes<KeyType>::StoreIndexChunk()
{
    indexFileStream.seekWrite(fingersTree-> currentFinger.nextChunkOffset); // FIXME check result in paranoid mode

    indexFileStream.fileStream.write((char *) localStream-> bufData, indexChunkSize);
    return indexFileStream.good();
} */
/*
template <typename KeyType> int EgIndexes<KeyType>::LoadIndexChunk()
{
    indexFileStream.seekRead(fingersTree-> currentFinger.nextChunkOffset);

    // if (! indexFileStream.seek(fingersTree-> currentFinger.nextChunkOffset)) // FIXME implement paranoid mode
    //    EG_LOG_STUB << "ERROR: bad position of fingersTree-> currentFinger.nextChunkOffset = " << std::hex << fingersTree-> currentFinger.nextChunkOffset << FN;
    indexFileStream.fileStream.read((char *) localStream-> bufData, indexChunkSize);
    return indexFileStream.good();
} */
/*
template <typename KeyType> inline int EgIndexes<KeyType>::LoadIndexChunk(char* chunkPtr, uint64_t chunkOffset)
{
    indexFileStream.seekRead(chunkOffset);

    // if (! indexFileStream.seek(fingersTree-> currentFinger.nextChunkOffset)) // FIXME implement paranoid mode
    //    EG_LOG_STUB << "ERROR: bad position of fingersTree-> currentFinger.nextChunkOffset = " << std::hex << fingersTree-> currentFinger.nextChunkOffset << FN;
    indexFileStream.fileStream.read(chunkPtr, indexChunkSize);
    return indexFileStream.good();
}*/
/*
template <typename KeyType> inline void EgIndexes<KeyType>::MoveTailToInsert(char* chunkPtr, int indexPosition)
{
    // EG_LOG_STUB << "chunk before memmove" << QByteArray(chunk, egChunkCapacity*oneIndexSize).toHex() << FN;
    // EG_LOG_STUB << "indexPosition = " << std::dec << indexPosition << FN;
    // EG_LOG_STUB << "theChunkCount = " << std::dec << theChunkCount << FN;
    memmove (chunkPtr + (indexPosition+1)*oneIndexSize, chunkPtr + indexPosition*oneIndexSize,
             oneIndexSize*(theChunkCount - indexPosition));
    // PrintIndexesChunk(chunk, "chunk after memmove " + FNS);
} */
/*
template <typename KeyType> inline void EgIndexes<KeyType>::ReadIndexValues(EgIndexStruct<KeyType>& indexStruct)
{
    localStream->seek(indexPosition*oneIndexSize);
    *localStream >> indexStruct.indexKey;
    *localStream >> indexStruct.dataOffset;
}

template <typename KeyType> inline void EgIndexes<KeyType>::ReadIndexOnly(KeyType& currentIndex, int indexPosition)
{
    localStream->seek(indexPosition*oneIndexSize);
    *localStream >> currentIndex;
}

template <typename KeyType> inline void EgIndexes<KeyType>::ReadIndexOnly(KeyType& currentIndex)
{
    localStream->seek(theIndexPosition*oneIndexSize);
    *localStream >> currentIndex;
}


template <typename KeyType> inline void EgIndexes<KeyType>::WriteIndexValues(EgIndexStruct<KeyType>& indexStruct, int position)
{
    localStream->seek(position*oneIndexSize);
    *localStream << indexStruct.indexKey;
    *localStream << indexStruct.dataOffset;
}

template <typename KeyType> inline void EgIndexes<KeyType>::UpdateChunkCount(keysCountType newCount)
{
    localStream->seek((egChunkCapacity * oneIndexSize) + (sizeof(uint64_t) * 2));
    *localStream << newCount;
}


template <typename KeyType> inline void EgIndexes<KeyType>::InsertInsideIndexChunk(KeyType& key, uint64_t& dataOffset) {
    // bool res {true};
    // theKey = key;
    FindIndexPositionToInsert(key); // sets theIndexPosition
    // EG_LOG_STUB << "theChunkCount = " << std::dec << (int) theChunkCount << FN;
    if (theIndexPosition < theChunkCount) // not last
        MoveTailToInsert((char*) localStream-> bufData, theIndexPosition);
    // EG_LOG_STUB << "theIndexPosition = " << std::dec << (int) theIndexPosition << FN;
    WriteIndexValues(key, dataOffset, theIndexPosition); // write index key and offset
    // return res;
}

template <typename KeyType> inline void EgIndexes<KeyType>::WriteIndexValues(KeyType key, uint64_t dataOffset, int position)
{
    localStream->seek(position*oneIndexSize);
    *localStream << key;
    *localStream << dataOffset;
}

template <typename KeyType> int EgIndexes<KeyType>::InsertToIndexChunk()
{
    InitMinMaxFlags();
    SetMinMaxOfFinger(theKey);

    LoadIndexChunk(); // fingersTree-> currentFinger.nextChunkOffset to localStream-> bufData

    // fingersTree-> PrintFingerInfo(fingersTree-> currentFinger);

    if (fingersTree-> currentFinger.itemsCount < egChunkCapacity) // chunk has empty space
    {
        InsertInsideIndexChunk();
        UpdateChunkCount(fingersTree-> currentFinger.itemsCount + 1);

        StoreIndexChunk(); // localStream-> bufData to fingersTree-> currentFinger.nextChunkOffset

        // PrintIndexesChunk(localStream-> bufData, "chunk after insert " + FNS);

            // update fingers
        fingersTree-> UpdateFingersChainAfterInsert();
    }
    else    // overflow - append or split chunk
    {
        /*if (theKey >= fingersTree-> currentFinger.maxKey)
            AppendIndexChunk();
        else
            SplitIndexChunk(); */

            // update fingers tree
/*        fingersTree-> UpdateFingersChainAfterSplit();
    }

    return 0;
} */
/*
template <typename KeyType> int EgIndexes<KeyType>::SplitIndexChunk()
{
    char new_chunk[indexChunkSize];
    memset(&new_chunk, 0, indexChunkSize);

    localStream-> seek(egChunkCapacity * oneIndexSize);
    *localStream >> prevOffsetPtr; // store for chain update
    *localStream >> nextOffsetPtr;

    fingersTree-> newFinger.nextChunkOffset = indexFileStream.getFileSize(); // append to the end of indexes file
    fingersTree-> newFinger.myLevel = 0;

    // theIndexPosition = FindIndexPosition(*localStream);
    // theIndexPosition = FindPosByKeyLast(EgIndexes<KeyType>::CompareLE) + 1;

    FindIndexPositionToInsert(theKey); // sets theIndexPosition

    // EG_LOG_STUB << "theIndexPosition = " << theIndexPosition << FN;

    // PrintIndexesChunk(localStream-> bufData, "before move" + FNS);

    if (theIndexPosition < fingersTree-> currentFinger.itemsCount ) // not last
        // memmove (localStream-> bufData + (theIndexPosition+1)*oneIndexSize, localStream-> bufData + theIndexPosition*oneIndexSize,
        //         oneIndexSize*(egChunkCapacity - theIndexPosition + 1)); // MoveTail

        MoveTailToInsert((char*) localStream-> bufData, theIndexPosition);

            // write index key and offset
    indexData.indexKey = theKey;
    indexData.dataOffset = theOldDataOffset;

    WriteIndexValues(indexData, theIndexPosition);

    // PrintIndexesChunk(localStream-> bufData, "after insert" + FNS);

        // move second part to new chunk
    memmove(&new_chunk, localStream-> bufData + (egChunkCapacity/2+1)*oneIndexSize, egChunkCapacity/2*oneIndexSize);
    memset(localStream-> bufData + (egChunkCapacity/2+1)*oneIndexSize, 0, (egChunkCapacity/2-1)*oneIndexSize);

    localStream->seek(egChunkCapacity * oneIndexSize);
    *localStream << prevOffsetPtr;                                  // restore prev ptr
    *localStream << fingersTree-> newFinger.nextChunkOffset;        // new next ptr

    fingersTree-> currentFinger.itemsCount = egChunkCapacity/2+1;

    *localStream << fingersTree-> currentFinger.itemsCount;

    localStream->seek(0);
    *localStream >> fingersTree-> currentFinger.minKey;

    localStream->seek(egChunkCapacity/2 * oneIndexSize); // FIXME check pos
    *localStream >> fingersTree-> currentFinger.maxKey;

    StoreIndexChunk(); // old one

    // PrintIndexesChunk(localStream-> bufData, "old chunk updated" + FNS);

    memcpy(localStream-> bufData, &new_chunk, indexChunkSize);

    localStream->seek(egChunkCapacity * oneIndexSize);
    *localStream << fingersTree-> currentFinger.nextChunkOffset;    // prev ptr
    *localStream << nextOffsetPtr;                                  // old next ptr

    if (nextOffsetPtr) // update next chunk in chain
    {
        indexFileStream.seekWrite(nextOffsetPtr + egChunkCapacity * oneIndexSize);
        indexFileStream << fingersTree-> newFinger.nextChunkOffset;
    }

    fingersTree-> newFinger.itemsCount = egChunkCapacity/2;

    *localStream << fingersTree-> newFinger.itemsCount;

    localStream-> seek(0);
    *localStream >> fingersTree-> newFinger.minKey;

    localStream-> seek((egChunkCapacity/2 - 1) * oneIndexSize); // FIXME check pos
    *localStream >> fingersTree-> newFinger.maxKey;

    StoreIndexChunk(localStream-> bufData, fingersTree-> newFinger.nextChunkOffset);

    // PrintIndexesChunk(localStream-> bufData, "new_chunk " + FNS);

    return 0;
} */
/*
template <typename KeyType> inline void EgIndexes<KeyType>::SaveLastIndex(KeyType& key, uint64_t& dataOffset) {
    localStream-> seek((egChunkCapacity-1) * oneIndexSize);
    *localStream >> key;
    *localStream >> dataOffset;
}

template <typename KeyType> inline void EgIndexes<KeyType>::GetChainPointers(uint64_t& nextPtr, uint64_t& prevPtr) {
    localStream-> seek(fwdPtrPosition);
    *localStream >> nextPtr;
    *localStream >> prevPtr;
}

template <typename KeyType> inline bool EgIndexes<KeyType>::AppendIndexChunk(KeyType& key, uint64_t& dataOffset, uint64_t& fwdPtr, uint64_t& backPtr) {
    bool res {true};
    // EG_LOG_STUB << "key = " << std::hex << (int) key << ", dataOffset = " << dataOffset << FN;
    memset(localStream-> bufData, 0, indexChunkSize);
    WriteIndexValues(key, dataOffset, 0);
    SetCountOfChunk(1);
    localStream-> seek(fwdPtrPosition);
    *localStream << fwdPtr;
    *localStream << backPtr;
    fingersTree-> newFinger.minKey     = key;
    fingersTree-> newFinger.maxKey     = key;
    // fingersTree-> newFinger.nextChunkOffset = indexFileStream.getFileSize();
    fingersTree-> newFinger.myChunkIsLeaf = 0xFF; // is leaf
    if(fwdPtr) {
        indexFileStream.seekWrite(fwdPtr + backPtrPosition); // backptr of next chunk
        indexFileStream << fingersTree-> newFinger.nextChunkOffset;
        res = res && indexFileStream.good();
    }   
    res = res && StoreIndexChunk(fingersTree-> newFinger.nextChunkOffset); // save new chunk, fingers already updated to upper level
    return res;
}
/*
template <typename KeyType> int EgIndexes<KeyType>::AppendIndexChunk()
{
        // setup new finger
    fingersTree-> newFinger.nextChunkOffset = indexFileStream.getFileSize(); // append to the end of indexes file

    // fingersTree-> newFinger.myLevel    = 0; FIXME check
    fingersTree-> newFinger.itemsCount = 1;

    fingersTree-> newFinger.minKey     = theKey;
    fingersTree-> newFinger.maxKey     = theKey;

        // get old next ptr
    localStream->seek(egChunkCapacity * oneIndexSize + sizeof(uint64_t));
    *localStream >> nextOffsetPtr;

    // EG_LOG_STUB << "current next ptr =  " << std::hex << (int) fingersTree-> newFinger.nextChunkOffset << " ,prev ptr =  " << std::hex << (int) prevOffsetPtr << FN;

        // write new next ptr
    localStream->seek(egChunkCapacity * oneIndexSize + sizeof(uint64_t));
    *localStream << fingersTree-> newFinger.nextChunkOffset;

    StoreIndexChunk(); // save old chunk by currentFinger

    memset(localStream-> bufData, 0, indexChunkSize); // empty new chunk

        // write index key and offset
    indexData.indexKey   = theKey;
    indexData.dataOffset = theOldDataOffset;

    WriteIndexValues(indexData, 0);

        // update chain pointers & count
    localStream->seek(egChunkCapacity * oneIndexSize);
    *localStream << fingersTree-> currentFinger.nextChunkOffset;        // prev ptr
    *localStream << nextOffsetPtr;                                      // stored next ptr

    UpdateChunkCount(1);

    StoreIndexChunk(localStream-> bufData, fingersTree-> newFinger.nextChunkOffset); // new chunk

    if (nextOffsetPtr) // update next chunk in chain
    {
        indexFileStream.seekWrite(nextOffsetPtr + egChunkCapacity * oneIndexSize);
        indexFileStream << fingersTree-> newFinger.nextChunkOffset;
    }

    return 0;
}
*/
/*
template <typename KeyType> inline void EgIndexes<KeyType>::FindIndexPositionToInsert(KeyType& key)
{
    KeyType currentIndex;
    theIndexPosition = theChunkCount;
    while (theIndexPosition > 0) {
        ReadIndexOnly(currentIndex, theIndexPosition-1);
        if (currentIndex > key)
           theIndexPosition--;
        else
            break;
    }
    return;

/*        // find position to insert
    while (theIndexPosition >= 0)
    {
        EG_LOG_STUB << "theIndexPosition = " << std::dec << (int) theIndexPosition << FN;
           // load index key from chunk
        ReadIndexOnly(currentIndex);
        EG_LOG_STUB << "currentIndex = " << std::hex << (int) currentIndex << FN;
        if(currentIndex <= key)
        {
            theIndexPosition++; // insert here
            break;
        }

        theIndexPosition--;
    }

    if (theIndexPosition < 0)
        theIndexPosition = 0; */
/*}

template <typename KeyType> int EgIndexes<KeyType>::FindPosByKeyFirst(CompareFunctionType myCompareFunc)
{
    KeyType currentIndex;

        // min/max lookup
    if ((theKey >  fingersTree-> currentFinger.maxKey)|| (fingersTree-> currentFinger.itemsCount <= 0))
    {
        EG_LOG_STUB << "ERROR bad finger of " << fingersTree-> fingersFileName << " for Key = " << theKey << FN;

        fingersTree-> PrintFingerInfo(fingersTree-> currentFinger, "currentFinger");
        return -1; // error
    }

    if ((theKey < fingersTree-> currentFinger.minKey) || (fingersTree-> currentFinger.itemsCount == 1))
    {
            // load key
        theIndexPosition = 0;
        ReadIndexOnly(currentIndex);

        if (myCompareFunc(currentIndex,theKey))
            return 0; // got it
        else
            return -1;
    }

        // proportional index lookup
    if (fingersTree-> currentFinger.maxKey > fingersTree-> currentFinger.minKey)
        theIndexPosition = (theKey - fingersTree-> currentFinger.minKey)*(fingersTree-> currentFinger.itemsCount - 1) /
                        (fingersTree-> currentFinger.maxKey - fingersTree-> currentFinger.minKey);
    else
    {
        if (myCompareFunc(fingersTree-> currentFinger.minKey, theKey))
            return 0; // got it
        else
            return -1;
    }

    // EG_LOG_STUB << "theIndexPosition =  " << theIndexPosition << FN;

    ReadIndexOnly(currentIndex);

    // EG_LOG_STUB << "theIndexPosition =  " << theIndexPosition << " ,currentIndex = " << std::hex << (int) currentIndex << FN;

        // compare
    if (myCompareFunc(currentIndex,theKey) && (theIndexPosition > 0)) // (currentIndex >= theKey)
    {
            // move down until currentIndex <
        do
        {
            theIndexPosition--;
            ReadIndexOnly(currentIndex);
        }
        while (myCompareFunc(currentIndex,theKey) && (theIndexPosition > 0)); // (currentIndex >= theKey)

        if (! myCompareFunc(currentIndex,theKey) && (theIndexPosition < (fingersTree-> currentFinger.itemsCount-1))) // (currentIndex < theKey)
            theIndexPosition++; // correction
    }
    else if (! myCompareFunc(currentIndex,theKey) && (theIndexPosition < (fingersTree-> currentFinger.itemsCount-1))) // (currentIndex < theKey)
    {
            // move up until currentIndex >=
        do
        {
            theIndexPosition++;
            ReadIndexOnly(currentIndex);
        }
        while (! myCompareFunc(currentIndex,theKey) && (theIndexPosition < (fingersTree-> currentFinger.itemsCount-1))); // (currentIndex < theKey)
    }

    return theIndexPosition;
}

template <typename KeyType> int EgIndexes<KeyType>::FindPosByKeyLast(CompareFunctionType myCompareFunc)
{
    KeyType currentIndex;

        // min/max check
    if ((theKey < fingersTree-> currentFinger.minKey) || (fingersTree-> currentFinger.itemsCount <= 0))
    {
        // EG_LOG_STUB << "ERROR bad finger for Key = " << theKey << FN;

        // fingersTree->PrintFingerInfo(fingersTree-> currentFinger, "currentFinger");
        return -1; // error or between fingers
    }

    if ((theKey >  fingersTree-> currentFinger.maxKey) || (fingersTree-> currentFinger.itemsCount == 1))
    {
            // load key
        theIndexPosition = fingersTree-> currentFinger.itemsCount - 1;
        ReadIndexOnly(currentIndex);

        /*
        EG_LOG_STUB << "theIndexPosition 0 =  " << theIndexPosition
                 << " ,myCompareFunc =  " << myCompareFunc(currentIndex,theKey)
                 << " ,currentIndex = " << (int) currentIndex << " ,theKey = " << (int) theKey << FN;
        */
/*
        if (myCompareFunc(currentIndex,theKey))
            return (fingersTree-> currentFinger.itemsCount - 1); // got it
        else
            return -1;
    }

        // proportional index lookup
    if (fingersTree-> currentFinger.maxKey > fingersTree-> currentFinger.minKey)
        theIndexPosition = (theKey - fingersTree-> currentFinger.minKey)*(fingersTree-> currentFinger.itemsCount - 1)/(fingersTree-> currentFinger.maxKey - fingersTree-> currentFinger.minKey);
    else
    {
        if (myCompareFunc(fingersTree-> currentFinger.maxKey, theKey))
            return (fingersTree-> currentFinger.itemsCount - 1); // got it
        else
            return -1;
    }

    ReadIndexOnly(currentIndex);

    /*
    EG_LOG_STUB << "theIndexPosition 1 =  " << theIndexPosition
             << " ,myCompareFunc =  " << myCompareFunc(currentIndex,theKey)
             << " ,currentIndex = " << (int) currentIndex << " ,theKey = " << (int) theKey << FN;
    */

        // compare
/*    if (myCompareFunc(currentIndex,theKey) && (theIndexPosition < (fingersTree-> currentFinger.itemsCount-1))) // (currentIndex <= theKey)
    {
            // move up until currentIndex >
        do
        {
            theIndexPosition++;
            ReadIndexOnly(currentIndex);

            // EG_LOG_STUB << "theIndexPosition 2 =  " << theIndexPosition << " ,currentIndex = " << std::hex << (int) currentIndex << FN;
        }
        while (myCompareFunc(currentIndex,theKey) && (theIndexPosition < (fingersTree-> currentFinger.itemsCount-1))); // (currentIndex <= theKey)

        if (! myCompareFunc(currentIndex,theKey)  && (theIndexPosition > 0)) // (currentIndex > theKey)
            theIndexPosition--; // correction
    }
    else if (! myCompareFunc(currentIndex,theKey)  && (theIndexPosition > 0)) // (currentIndex > theKey)
    {
            // move down until currentIndex <=
        do
        {
            theIndexPosition--;
            ReadIndexOnly(currentIndex);
        }
        while (! myCompareFunc(currentIndex,theKey)  && (theIndexPosition > 0)); // (currentIndex > theKey)
    }

    /*
    EG_LOG_STUB << "theIndexPosition 3 =  " << theIndexPosition
             << " ,myCompareFunc =  " << myCompareFunc(currentIndex,theKey)
             << " ,currentIndex = " << (int) currentIndex << " ,theKey = " << (int) theKey << FN;
    */

/*    return theIndexPosition;
}


template <typename KeyType> void EgIndexes<KeyType>::LoadAllData(std::set<uint64_t>& index_offsets)
{
        // get first index offset from header
    indexFileStream.seekRead(0);
    indexFileStream >> fingersTree-> currentFinger.nextChunkOffset;

    LoadIndexChunk(fingersTree-> currentFinger.nextChunkOffset); // fingersTree-> currentFinger.nextChunkOffset to localStream-> bufData

    theIndexPosition = 0;

    LoadDataUp(index_offsets);
}

template <typename KeyType> int EgIndexes<KeyType>::LoadDataByChunkUp(std::set<uint64_t>& index_offsets, CompareFunctionType myCompareFunc)
{
    LoadIndexChunk(fingersTree-> currentFinger.nextChunkOffset); // localStream-> bufData

    theIndexPosition = FindPosByKeyFirst(myCompareFunc);

    // EG_LOG_STUB << "pos =  " << theIndexPosition << FN;

    if (theIndexPosition >= 0)
        LoadDataUp(index_offsets);

    return 0;
}

template <typename KeyType> void EgIndexes<KeyType>::LoadDataUp2(std::set<uint64_t>& index_offsets) {

}

template <typename KeyType> void EgIndexes<KeyType>::LoadDataUp(std::set<uint64_t>& index_offsets)
{
    uint64_t dataOffset;

    localStream->seek((egChunkCapacity * oneIndexSize) + (sizeof(uint64_t) * 2));
    *localStream >> theChunkCount;

    // EG_LOG_STUB << "theIndexPosition =  " << theIndexPosition  << " , theChunkCount =  " << theChunkCount << FN;

    while (theIndexPosition < theChunkCount)
    {
        localStream->seek(theIndexPosition*oneIndexSize + sizeof(KeyType));
        // *localStream >> currentIndex;
        *localStream >> dataOffset;

        index_offsets.insert(dataOffset);

        theIndexPosition++;
    }

        // get next chunk
    localStream->seek((egChunkCapacity * oneIndexSize) + (sizeof(uint64_t)));
    *localStream >> nextOffsetPtr;

    while (nextOffsetPtr)
    {
        LoadIndexChunk(nextOffsetPtr);

            // get chunk items count
        localStream->seek((egChunkCapacity * oneIndexSize) + (sizeof(uint64_t) * 2));
        *localStream >> theChunkCount;

        // EG_LOG_STUB << "nextChunkPtr =  " << std::hex << (int) nextChunkPtr  << " , theChunkCount =  " << std::hex << (int) theChunkCount << FN;

        theIndexPosition = 0;

        while (theIndexPosition < theChunkCount)
        {
            localStream->seek(theIndexPosition*oneIndexSize + sizeof(KeyType));
            // *localStream >> currentIndex;
            *localStream >> dataOffset;

            index_offsets.insert(dataOffset);

            theIndexPosition++;
        }

            // get next chunk ptr
        localStream->seek((egChunkCapacity * oneIndexSize) + (sizeof(uint64_t)));
        *localStream >> nextOffsetPtr;
    }
}


template <typename KeyType> int EgIndexes<KeyType>::LoadDataByChunkDown(std::set<uint64_t>& index_offsets, CompareFunctionType myCompareFunc)
{
    uint64_t dataOffset;

    LoadIndexChunk(fingersTree-> currentFinger.nextChunkOffset); // localStream-> bufData

    // EG_LOG_STUB << "indexBA =  " << indexBA.toHex() << FN;

    theIndexPosition = FindPosByKeyLast(myCompareFunc);

    // EG_LOG_STUB << "pos =  " << theIndexPosition << FN;

    if (theIndexPosition < 0)
        return theIndexPosition; // not found

    while (theIndexPosition >= 0)
    {
        localStream-> seek(theIndexPosition*oneIndexSize + sizeof(KeyType));
        // *localStream >> currentIndex;
        *localStream >> dataOffset;

        index_offsets.insert(dataOffset);

        theIndexPosition--;
    }

        // get next chunk
    localStream-> seek(egChunkCapacity * oneIndexSize);
    *localStream >> prevOffsetPtr;

    // EG_LOG_STUB << "prevOffsetPtr =  " << std::hex << (int) prevOffsetPtr  << " , theChunkCount =  " << std::hex << (int) theChunkCount << FN;

    while (prevOffsetPtr)
    {
        LoadIndexChunk(prevOffsetPtr);

            // get chunk items count
        localStream-> seek((egChunkCapacity * oneIndexSize) + (sizeof(uint64_t) * 2));
        *localStream >> theChunkCount;

        theIndexPosition = theChunkCount - 1;

        while (theIndexPosition >= 0)
        {
            localStream-> seek(theIndexPosition*oneIndexSize + sizeof(KeyType));
            // *localStream >> currentIndex;
            *localStream >> dataOffset;

            index_offsets.insert(dataOffset);

            theIndexPosition--;
        }

            // get next chunk ptr
        localStream-> seek(egChunkCapacity * oneIndexSize);
        *localStream >> prevOffsetPtr;
    }

    return 0;
}

template <typename KeyType> int EgIndexes<KeyType>::LoadDataByChunkEqual(std::set<uint64_t>& index_offsets)
{
    KeyType currentIndex;
    uint64_t dataOffset;

    uint64_t nextChunkPtr;

    bool keyOutOfRange = false;
    bool firstChunk = true;

    nextChunkPtr =  fingersTree-> currentFinger.nextChunkOffset;

    // EG_LOG_STUB << "nextChunkPtr =  " << std::hex << (int) nextChunkPtr  << " , theChunkCount =  " << std::hex << (int) theChunkCount << FN;

    while (nextChunkPtr && (! keyOutOfRange))
    {
        LoadIndexChunk(nextChunkPtr);

        if (firstChunk)
        {
           theIndexPosition = FindPosByKeyFirst(CompareGE);  // Index >= Key

           if (theIndexPosition < 0)
              theIndexPosition = 0;    // FIXME

           firstChunk = false;

           theChunkCount = fingersTree-> currentFinger.itemsCount;
        }
        else
        {
            theIndexPosition = 0;

                // get chunk items count
            localStream->seek((egChunkCapacity * oneIndexSize) + (sizeof(uint64_t) * 2));
            *localStream >> theChunkCount;
        }

        // EG_LOG_STUB << "nextChunkPtr =  " << std::hex << (int) nextChunkPtr  << " , theChunkCount =  " << std::hex << (int) theChunkCount << FN;

        // theIndexPosition = 0;

        do
        {
            localStream-> seek(theIndexPosition*oneIndexSize);
            *localStream >> currentIndex;
            *localStream >> dataOffset;

            // EG_LOG_STUB << "currentIndex =  " << std::hex << (int) currentIndex  << " , dataOffset =  " << std::hex << (int) dataOffset << FN;

            if (currentIndex == theKey)
                index_offsets.insert(dataOffset);
            else
                keyOutOfRange = true;

            theIndexPosition++;
        }
        while ((! keyOutOfRange) && (theIndexPosition < theChunkCount));

            // get next chunk ptr
        localStream-> seek((egChunkCapacity * oneIndexSize) + (sizeof(uint64_t)));
        *localStream >> nextChunkPtr;
    }

    return 0;
}


template <typename KeyType> inline void EgIndexes<KeyType>::ReadIndexAndOffset(KeyType& currentIndex, uint64_t& dataOffset, int indexPosition)
{
    localStream  -> seek(indexPosition*oneIndexSize);
    *localStream >> currentIndex;
    *localStream >> dataOffset; 
}

template <typename KeyType> inline bool EgIndexes<KeyType>::FindIndexByDataOffset() // uses theKey, theDataOffset sets theIndexesChunkOffset, theIndexPosition, fingersChainFlag
{
    KeyType currentIndex;
    uint64_t dataOffset;
    bool res {true};
    fingersChainFlag = true;
    theIndexesChunkOffset = fingersTree-> currentFinger.nextChunkOffset;
    while (theIndexesChunkOffset && res) {
        res = res && LoadIndexChunk(theIndexesChunkOffset); // sets theChunkCount
        theIndexPosition = 0;
        while (theIndexPosition < theChunkCount) { // look for offset in the chunk
            ReadIndexAndOffset(currentIndex, dataOffset, theIndexPosition);
            if (currentIndex == theKey && dataOffset == theDataOffset) // got it
                return true;
            if (currentIndex > theKey) // out of range
                return false;
            theIndexPosition++;
        }
        fingersChainFlag = false;               // chunk moved to next, use backptrs
        localStream  -> seek(fwdPtrPosition);   // go to next chunk by chain ptr
        *localStream << theIndexesChunkOffset;
    }
    return res;
}
/*
template <typename KeyType> int EgIndexes<KeyType>::FindIndexByDataOffset(bool isPrimary)
{
    KeyType currentIndex;
    uint64_t dataOffset;

    // uint64_t nextChunkPtr;

    bool keyOutOfRange = false;
    bool firstChunk = true;

    // fingersTree-> fingerIsMoved = false; // moved flag setup

    LoadIndexChunk(); // localStream-> bufData by finger

    theIndexesChunkOffset = fingersTree-> currentFinger.nextChunkOffset;

    while (theIndexesChunkOffset && (! keyOutOfRange))
    {
        if (firstChunk)
        {
           theIndexPosition = FindPosByKeyFirst(CompareGE);
           if (theIndexPosition < 0)
              theIndexPosition = 0;    // FIXME

           firstChunk = false;

           theChunkCount = fingersTree-> currentFinger.itemsCount;
        }
        else
        {
            theIndexPosition = 0;

                // get chunk items count
            localStream->seek((egChunkCapacity * oneIndexSize) + (sizeof(uint64_t) * 2));
            *localStream >> theChunkCount;
        }

        // EG_LOG_STUB << "theIndexPosition =  " << theIndexPosition;

        do
        {
            localStream->seek(theIndexPosition*oneIndexSize);
            *localStream >> currentIndex;
            *localStream >> dataOffset;

            // EG_LOG_STUB << "currentIndex =  " << std::hex << (int) currentIndex  << " , dataOffset =  " << std::hex << (int) dataOffset << FN;

            if (currentIndex == theKey)
            {
                if (! isPrimary)
                {
                    if (dataOffset == theOldDataOffset)
                        return 0; // found theIndexPosition
                }
                else    // found ID, store offset
                {
                    theOldDataOffset = dataOffset;
                    return 0;
                }
            }
            else
                keyOutOfRange = true;

            theIndexPosition++;
        }
        while ((! keyOutOfRange) && (theIndexPosition < theChunkCount));

            // get next chunk ptr
        localStream->seek((egChunkCapacity * oneIndexSize) + (sizeof(uint64_t)));
        *localStream >> theIndexesChunkOffset;

        if (theIndexesChunkOffset)
        {
            // fingersTree-> fingerIsMoved = true;
            LoadIndexChunk(localStream-> bufData, theIndexesChunkOffset);
        }
    }

    return 1; // not found
}
*/

/*
template <typename KeyType> int EgIndexes<KeyType>::DeleteIndexInChunk_old()
{

        // get count
    localStream->seek((egChunkCapacity * oneIndexSize) + (sizeof(uint64_t) * 2));
    *localStream >> theChunkCount;

    // EG_LOG_STUB << "theIndexPosition =  " << std::hex << (int) theIndexPosition << " ,theChunkCount =  " << std::hex << (int) theChunkCount << FN;

        // if not last, move tail
    if (theIndexPosition < (theChunkCount - 1)) // FIXME move tail
    {
        // EG_LOG_STUB << "theIndexPosition = " << theIndexPosition << ", itemsCount = " << theChunkCount << FN;
        // EG_LOG_STUB << "chunk before memmove" << indexBA.toHex() << FN;
        memmove (localStream-> bufData + theIndexPosition*oneIndexSize, localStream-> bufData + (theIndexPosition+1)*oneIndexSize,  oneIndexSize*(theChunkCount - theIndexPosition - 1));
    }

        // write zeroes
    memset(localStream-> bufData + (theChunkCount - 1)*oneIndexSize, 0, oneIndexSize);

        // reset flags
    fingersTree-> minValueChanged = false;
    fingersTree-> maxValueChanged = false;

        // get new min/max FIXME check
    if (theIndexPosition == 0)
    {
        localStream->seek(0);
        *localStream >> fingersTree-> newMinValue;

        if (theKey < fingersTree-> newMinValue)
            fingersTree-> minValueChanged = true;
    }
    else if ((theIndexPosition == theChunkCount-1) && (theChunkCount > 1))
    {
        localStream->seek((theChunkCount-2)*oneIndexSize);
        *localStream >> fingersTree->newMaxValue;

        if (theKey > fingersTree-> newMaxValue)
            fingersTree-> maxValueChanged = true;
    }

    --theChunkCount; // theChunkCount DECREMENT here

        // write decreased count
    localStream->seek((egChunkCapacity * oneIndexSize) + (sizeof(uint64_t) * 2));
    *localStream << theChunkCount;

    // EG_LOG_STUB << "chunk after memmove " << indexBA.toHex() << FN;

        // save
    StoreIndexChunk();

    return 0;
} */
/*
template <typename KeyType> void EgIndexes<KeyType>::UpdateDataOffsetForIndex(bool isPrimary)
{
    if (FindIndexByDataOffset(isPrimary) == 0) // index found
    {
        indexFileStream.seekWrite(fingersTree-> currentFinger.nextChunkOffset + theIndexPosition*oneIndexSize + sizeof(KeyType));
        indexFileStream << theNewDataOffset;
    }
    else
        EG_LOG_STUB << "Index to update not found, theKey = " << theKey << ", IndexFileName = " << fingersTree-> IndexFileName << FN;
}
*/ /*
template <typename KeyType> int EgIndexes<KeyType>::DeleteIndex(bool isPrimary)
{
    // EG_LOG_STUB << "theKey = " << theKey << ", theOldDataOffset = " << std::hex << (int) theOldDataOffset << ", IndexFileName = " << fingersTree-> IndexFileName << FN;

    // fingersTree-> PrintFingerInfo(fingersTree-> currentFinger, "DeleteIndex");

    if (FindIndexByDataOffset(isPrimary) == 0) // index found
    {
        // EG_LOG_STUB << "theChunkCount 1 =  " << (int) theChunkCount << FN;

            // get finger ptr
        localStream-> seek((egChunkCapacity * oneIndexSize) + (sizeof(uint64_t) * 2) + sizeof(keysCountType));
        *localStream >> fingersTree-> currentFingerOffset;

        // EG_LOG_STUB << "theChunkCount =  " << (int) theChunkCount << "currentFingerOffset =  " << std::hex << (int) fingersTree-> currentFingerOffset << FN;

        if (theChunkCount > 1) // not last index
        {
            DeleteIndexInChunk_old(); // theChunkCount decrement here

            fingersTree-> UpdateFingerCountAfterDelete(theChunkCount); // by fingersTree-> currentFingerOffset
        }
        else if (theChunkCount == 1) // last index, delete finger by fingersTree-> currentFingerOffset
        {
            if (fingersTree-> DeleteParentFinger() == 1) // last index
                return 1; // delete files

            RemoveChunkFromChain_old();
        }
        else
            EG_LOG_STUB << "Bad indexes count at " << fingersTree-> IndexFileName << " Key = " << std::hex << (int) theKey << " Offset = " << (int) theOldDataOffset << FN;
    }
    else
        EG_LOG_STUB << "Indexes chunk not found " << fingersTree-> IndexFileName << " Key = " << std::hex << (int) theKey << " Offset = "  << (int) theOldDataOffset << FN;

    return 0;
}
*/

/*
template <typename KeyType> void EgIndexes<KeyType>::RemoveChunkFromChain_old()
{
    // uint64_t prevChunkPtr, nextChunkPtr;

        // get pointers
    indexFileStream.seekRead(theIndexesChunkOffset + egChunkCapacity * oneIndexSize);
    indexFileStream >> prevOffsetPtr;
    indexFileStream >> nextOffsetPtr; */
/*
    EG_LOG_STUB << "theIndexesChunkOffset = " << std::hex << (int) theIndexesChunkOffset <<
                ", nextOffsetPtr = " << std::hex << (int) nextOffsetPtr <<
                ", prevOffsetPtr = " << std::hex << (int) prevOffsetPtr << FN;
*/
        // update backlinks
  /*  if (prevOffsetPtr)
    {        
        indexFileStream.seekWrite(prevOffsetPtr + egChunkCapacity * oneIndexSize  + sizeof(uint64_t));
        indexFileStream << nextOffsetPtr;
    }
    else // update header
    {
        indexFileStream.seekWrite(0);
        indexFileStream << nextOffsetPtr;
    }

    if (nextOffsetPtr)
    {
        indexFileStream.seekWrite(nextOffsetPtr + egChunkCapacity * oneIndexSize );
        indexFileStream << prevOffsetPtr;
    }

    // StoreIndexChunk(zero_chunk, theIndexesChunkOffset);

    // TODO add to vacant chunks chain

} */
/*
template <typename KeyType> bool EgIndexes<KeyType>::checkIndexesChainFwd(uint64_t &doubleSpeedOffset)
{
    if (doubleSpeedOffset)
    {
        indexFileStream.seekRead(doubleSpeedOffset + (egChunkCapacity * oneIndexSize + sizeof(uint64_t)));
        indexFileStream >> doubleSpeedOffset;

        if (doubleSpeedOffset == nextOffsetPtr)
            return false;

        if (doubleSpeedOffset)
        {
            indexFileStream.seekRead(doubleSpeedOffset + (egChunkCapacity * oneIndexSize + sizeof(uint64_t)));
            indexFileStream >> doubleSpeedOffset;

            if (doubleSpeedOffset == nextOffsetPtr)
                return false;
        }
    }

    return true;
}

template <typename KeyType> bool EgIndexes<KeyType>::checkIndexesChainBack()
{
    uint64_t doubleSpeedOffset = prevOffsetPtr;

    while (prevOffsetPtr && doubleSpeedOffset)
    {
        indexFileStream.seekRead(doubleSpeedOffset + (egChunkCapacity * oneIndexSize));
        indexFileStream >> doubleSpeedOffset;

        if (doubleSpeedOffset == prevOffsetPtr)
            return false;

        if (doubleSpeedOffset)
        {
            indexFileStream.seekRead(doubleSpeedOffset + (egChunkCapacity * oneIndexSize));
            indexFileStream >> doubleSpeedOffset;

            if (doubleSpeedOffset == prevOffsetPtr)
                return false;
        }

        indexFileStream.seekRead(prevOffsetPtr + (egChunkCapacity * oneIndexSize));
        indexFileStream >> prevOffsetPtr;
    }

    return true;
}
/*
template <typename KeyType> bool EgIndexes<KeyType>::checkIndexesIntegrity()
{
    KeyType currentKey, chunkMin, chunkMax, nextMin, prevMax;
    uint64_t dataOffset, fingerOffset;

    keysCountType prevChunkCount;

        // get first index offset from header
    indexFileStream.seekRead(0);
    indexFileStream >> nextOffsetPtr;

    uint64_t doubleSpeedOffset = nextOffsetPtr;

    while (nextOffsetPtr)
    {
        LoadIndexChunk(localStream-> bufData, nextOffsetPtr); // fingersTree-> currentFinger.nextChunkOffset to localStream-> bufData

        localStream->seek(egChunkCapacity * oneIndexSize + sizeof(uint64_t) * 2);
        *localStream >> theChunkCount;

        // EG_LOG_STUB << "theIndexPosition =  " << theIndexPosition  << " , theChunkCount =  " << theChunkCount << FN;

        theIndexPosition = 0;

        while (theIndexPosition < theChunkCount)
        {
            localStream->seek(theIndexPosition * oneIndexSize);
            *localStream >> currentKey;
            *localStream >> dataOffset;

            // check for both zero index key and data offset
            if (! static_cast<int> (currentKey) && ! dataOffset)
            {
                EG_LOG_STUB << "Possible structure corrupted: zero index key and data offset " << FN;
                return false;
            }

            if (! theIndexPosition)
                chunkMin = currentKey;

            if (theIndexPosition == theChunkCount-1)
                chunkMax = currentKey;

            theIndexPosition++;
        }

            // check for parent finger count and downlink
        GetFingerOffset(fingerOffset);
        fingersTree->LoadFingerDirect(fingersTree->currentFinger, fingerOffset);

        if ((fingersTree->currentFinger.minKey != chunkMin) ||
            (fingersTree->currentFinger.maxKey != chunkMax) ||
            (fingersTree->currentFinger.itemsCount != theChunkCount) ||
             fingersTree->currentFinger.nextChunkOffset != nextOffsetPtr)
        {
            EG_LOG_STUB << "Possible bad finger for index chunk " << FN;

            EG_LOG_STUB << "Min: " << static_cast<int> (chunkMin)
                        << " Max: " << static_cast<int> (chunkMax)
                        << " Chunk offset: " << nextOffsetPtr
                        << " Chunk count: "  << theChunkCount;
            fingersTree->PrintFingerInfo(fingersTree->currentFinger, "Finger by chunk uplink");

            return false;
        }

            // check indexes chain for loops:
        if (doubleSpeedOffset && ! checkIndexesChainFwd(doubleSpeedOffset))
        {
            EG_LOG_STUB << "Possible indexes chain loop detected on fwd links" << FN;
            return false;
        }

            // get next chunk
        GetChainPointers(nextOffsetPtr, prevOffsetPtr);

            // check for min/max issues with prev/next chunk
        if (nextOffsetPtr)
        {
            GetKeyByFileOffset(nextOffsetPtr, 0, nextMin);

            if (nextMin < chunkMax)
            {
                EG_LOG_STUB << "Possible indexes chain min/max failure, max = " << (int) chunkMax << " , next min = " << (int) nextMin  << FN;
                EG_LOG_STUB << "nextOffsetPtr = " << std::hex << nextOffsetPtr << FN;
                EG_LOG_STUB << "prevOffsetPtr = " << std::hex << prevOffsetPtr << FN;
                return false;
            }
        }

        if (prevOffsetPtr)
        {
            indexFileStream.seekRead(prevOffsetPtr + egChunkCapacity * oneIndexSize + sizeof(uint64_t) * 2);
            indexFileStream >> prevChunkCount;

            GetKeyByFileOffset(prevOffsetPtr, prevChunkCount - 1, prevMax);

            if (prevMax > chunkMin)
            {
                EG_LOG_STUB << "Possible indexes chain min/max failure, prev max = " << (int) prevMax << " , min = " << (int) chunkMin  << FN;
                EG_LOG_STUB << "nextOffsetPtr = " << std::hex << nextOffsetPtr << FN;
                EG_LOG_STUB << "prevOffsetPtr = " << std::hex << prevOffsetPtr << FN;

                return false;
            }
        }

    }
        // check indexes chain backlinks
    localStream->seek(egChunkCapacity * oneIndexSize);
    *localStream >> prevOffsetPtr;

    if (! checkIndexesChainBack())
    {
        EG_LOG_STUB << "Possible indexes chain loop detected on backward links" << FN;
        return false;
    }

   return true; // ok
} */

/*
EgIndexes<qint32> EgIndexesqint32;
EgIndexes<quint32> EgIndexesquint32;
EgIndexes<qint64> EgIndexesqint64;
EgIndexes<uint64_t> EgIndexesuint64_t;
EgIndexes<float> EgIndexesfloat;
EgIndexes<double> EgIndexesdouble;
*/

/*
template <typename KeyType> void EgIndexes<KeyType>::PrintIndexesChunk(char* theChunk, const QString &theMessage)
{
    EG_LOG_STUB << QByteArray(theChunk, indexChunkSize).toHex() << theMessage;
}
*/

// template class EgIndexes<uint32_t>;

