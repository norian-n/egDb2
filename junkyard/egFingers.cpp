// #include "../service/egFileType.cpp"
// #include "egIndexes.h"


 /* PrintFingerInfo(parentFinger, "parentFinger " + FNS);
    PrintFingerInfo(currentFinger, "currentFinger " + FNS);
    PrintFingerInfo(newFinger, "newFinger " + FNS); */

/*
template <typename KeyType> int EgFingers<KeyType>::DeleteCurrentFinger() { // ex UpdateFingersChainAfterDelete
    KeyType oldValue;
    keysCountType fingersCount;
    bool isFirstFinger;
    bool isLastFinger;
    if(! parentFingerOffset) { // check / update root header
        if (maxValueChanged || minValueChanged) {
            if (maxValueChanged)
                rootFinger.maxKey =  newMaxValue;
            if (minValueChanged)
                rootFinger.minKey =  newMinValue;
            StoreFingerMinMaxOnly(rootFinger);
        }
        return 0;
    }
    while (parentFingerOffset && (maxValueChanged || minValueChanged)) {
        currentFingerOffset = parentFingerOffset; // up
        uint64_t fingersChunkOffset = currentFingerOffset - ( currentFingerOffset % fingersChunkSize ); // calc this chunk offset
        fingersFileStream.seekRead(fingersChunkOffset + egChunkCapacity * oneFingerSize); // get next parent finger offset
        fingersFileStream >> parentFingerOffset;
        keysCountType fingerNum = (currentFingerOffset - fingersChunkOffset) / oneFingerSize; // check first/last finger for min/max update of root finger
        if (parentFingerOffset) {
            fingersFileStream.seekRead(parentFingerOffset + sizeof(KeyType)*2);
            fingersFileStream >> fingersCount;
        } else
            fingersCount = rootFinger.itemsCount;
        isFirstFinger = (fingerNum == 0);
        isLastFinger  = (fingerNum == fingersCount-1);
        if (maxValueChanged) {
            fingersFileStream.seekRead(parentFingerOffset + sizeof(KeyType));
            fingersFileStream >> oldValue;
            if (newMaxValue < oldValue) {
                fingersFileStream.seekWrite(parentFingerOffset + sizeof(KeyType));
                fingersFileStream << newMaxValue;
            }
            if ((newMaxValue >= oldValue) || (! isLastFinger)) {
                maxValueChanged = false;
                break;
            }
            // EG_LOG_STUB << "newMaxValue = " << newMaxValue << FN;
        } else if (minValueChanged) {
            fingersFileStream.seekRead(parentFingerOffset);
            fingersFileStream >> oldValue;
            if (newMinValue > oldValue) {
                fingersFileStream.seekWrite(parentFingerOffset);
                fingersFileStream << newMinValue;
            }
            if ((newMinValue <= oldValue) || (! isFirstFinger)) {
                minValueChanged = false;
                break;
            }
            // EG_LOG_STUB << "newMinValue = " << newMinValue << FN;
        } else {
            maxValueChanged = false;
            minValueChanged = false;
            break;
        }
    }   // while
        // EG_LOG_STUB << "parentFingerOffset = " << parentFingerOffset << FN;
    if(! parentFingerOffset) { // check / update root header
        if (maxValueChanged || minValueChanged) {
            if (maxValueChanged)
                rootFinger.maxKey =  newMaxValue;
            else if (minValueChanged)
                rootFinger.minKey =  newMinValue;
            StoreFingerMinMaxOnly(rootFinger);
        }
    }
    maxValueChanged = false; // reset
    minValueChanged = false;
    return 0;
}
*/
/*
template <typename KeyType> bool EgFingers<KeyType>::LoadFingerDirect(egFinger<KeyType>& theFinger, const uint64_t fingerOffset) {
    theFinger.myPosInChunkBytes = (fingerOffset - rootHeaderSize) % fingersChunkSize;
    theFinger.myChunkOffset   =  fingerOffset - currentFinger.myPosInChunkBytes;
    fingersFileStream.seekRead(fingerOffset);
    fingersFileStream >> theFinger.minKey;
    fingersFileStream >> theFinger.maxKey;
    fingersFileStream >> theFinger.itemsCount;
    fingersFileStream >> theFinger.nextChunkOffset;
    // fingersFileStream >> theFinger.chunkIsLeaf;
    return fingersFileStream.good();
}
*/
/*
template <typename KeyType> bool EgFingers<KeyType>::StoreFingerMinMaxOnly(egFinger<KeyType>& theFinger) {
    fingersFileStream.seekWrite(theFinger.myChunkOffset+theFinger.myPosInChunkBytes);
    fingersFileStream << theFinger.minKey;
    fingersFileStream << theFinger.maxKey;
    return fingersFileStream.good();
}
*/
/*
template <typename KeyType> bool EgFingers<KeyType>::LoadParentChunkOffsetDirect(egFinger<KeyType>& theFinger) {
    fingersFileStream.seekRead(theFinger.myChunkOffset + parentChunkOffsetPosition); // parentOffsetPosition is const in this header
    fingersFileStream >> theFinger.parentChunkOffset;
    // EG_LOG_STUB << "fingersChunkOffset" << hex << (int) fingersChunkOffset << ", parentFingerOffset" << hex << (int) parentFingerOffset << FN;
    return fingersFileStream.good();
}
*/
/*
template <typename KeyType> inline void EgFingers<KeyType>::ReadChunkMinMaxToParentFinger(egFinger<KeyType>& theParentFinger) {
    localStream  -> seek(0);
    *localStream >> theParentFinger.minKey;
    localStream  -> seek((theParentFinger.itemsCount - 1) * oneFingerSize + sizeof(KeyType));
    *localStream >> theParentFinger.maxKey;
} */

//template <typename KeyType> int EgFingers<KeyType>::SplitFingersChunk() {
//    uint64_t parentOffset;
/*  PrintFingerInfo(parentFinger, "parentFinger " + FNS);
    PrintFingerInfo(currentFinger, "currentFinger " + FNS);
    PrintFingerInfo(newFinger, "newFinger " + FNS); */
/*    char new_chunk[fingersChunkSize];
    memset(&new_chunk, 0, fingersChunkSize);
    uint64_t newChunkOffset = fingersFileStream.getFileSize();
    localStream  -> seek(egChunkCapacity * oneFingerSize);
    *localStream >> parentOffset; // store current chunk backptr as parent's
    posToInsert = currentFinger.myPosInChunkBytes/oneFingerSize + 1; // insert new finger next to current one
    // EG_LOG_STUB << "posToInsert = " << posToInsert << ", fingersCount = " << parentFinger.itemsCount << FN;
    if (posToInsert < parentFinger.itemsCount) // not last
        MoveTailToInsert(localStream-> bufData, posToInsert, parentFinger.itemsCount-posToInsert);
    // localStream -> seek(currentFinger.myPosInChunkBytes); // write updated current and new fingers
    WriteFingerToChunk(currentFinger);
    WriteFingerToChunk(newFinger);
    memmove(&new_chunk, localStream-> bufData + (egChunkCapacity/2+1)*oneFingerSize, egChunkCapacity/2*oneFingerSize); // move second part to new chunk
    memset(localStream-> bufData + (egChunkCapacity/2+1)*oneFingerSize, 0, egChunkCapacity/2*oneFingerSize);
    localStream  -> seek(egChunkCapacity * oneFingerSize); // restore parent back ptr & chunk level
    *localStream << parentOffset;
    *localStream << currentFinger.myLevel;
    StoreFingersChunk(parentFinger.nextChunkOffset);
    if (posToInsert < egChunkCapacity/2+1) // update parent backlinks for shifted fingers
        UpdateBackptrOffsets(parentFinger.nextChunkOffset, posToInsert, egChunkCapacity/2+1, currentFinger.myLevel); // FIXME check
    // PrintFingersChunk(localStream-> bufData,, "old chunk after split up " + FNS);
    currentFinger.itemsCount = egChunkCapacity/2+1; // level up
    ReadChunkMinMaxToParentFinger(currentFinger);

    memcpy(localStream-> bufData, &new_chunk, fingersChunkSize);
    UpdateChunkLevel(currentFinger.myLevel);
    StoreFingersChunk(newChunkOffset);
    // PrintFingersChunk(localStream-> bufData,, "new chunk after split up " + FNS);
    UpdateBackptrOffsets(newChunkOffset, 0, egChunkCapacity/2, currentFinger.myLevel); // update parent backlinks for shifted fingers
    newFinger.itemsCount = egChunkCapacity/2; // update parent fingers, go level up
    ReadChunkMinMaxToParentFinger(newFinger);
    currentFinger.nextChunkOffset = parentFinger.nextChunkOffset;
    currentFinger.myPosInChunkBytes = parentFinger.myPosInChunkBytes;
    currentFinger.myChunkOffset = parentFinger.myChunkOffset;
    currentFinger.myLevel = parentFinger.myLevel;
    // fingersFileStream.seekWrite(parentFinger.myChunkOffset + parentFinger.myPosInChunk);
    StoreFingerToFile(currentFinger);
    newFinger.myLevel = parentFinger.myLevel;
    newFinger.nextChunkOffset = newChunkOffset; */
/*  PrintFingerInfo(parentFinger, "parentFinger " + FNS);
    PrintFingerInfo(currentFinger, "currentFinger " + FNS);
    PrintFingerInfo(newFinger, "newFinger " + FNS); */
//     return 0;
//}

/*
template <typename KeyType> inline void EgFingers<KeyType>::UpdateBackptrOffsets( uint64_t myChunkOffset, int posToInsert, int itemsCount) {
    uint64_t nextLevelOffset;
    // EG_LOG_STUB << "posToInsert = " << posToInsert << ", myLocalLevel = " << myLocalLevel << ", itemsCount = " << itemsCount << FN;
    for (int i = posToInsert; i < itemsCount; i++) { // update parent backlinks for shifted fingers
        localStream  -> seek(i * oneFingerSize + nextChunkOffsetPosition); // get next level chunk offset
        *localStream >> nextLevelOffset;
            // EG_LOG_STUB << "i = " << i << ", nextLevelOffset = " << hex << (int) nextLevelOffset
            //         << ", myChunkOffset = " << hex << (int) myChunkOffset << FN;
        if (currentFinger.chunkIsLeaf) // write actual backlink  FIXME level num
            indexChunks-> StoreFingerOffset(nextLevelOffset, myChunkOffset + i * oneFingerSize);
        else
            StoreParentOffsetDirect(nextLevelOffset, myChunkOffset + i * oneFingerSize);
    }
    // return 0;
} */

/*
template <typename KeyType> int EgFingers<KeyType>::UpdateFingersChainAfterInsert() {
    if (anyMinMaxChanged)
        UpdateMinMaxByFlags(currentFinger, theKey);
    currentFinger.itemsCount++;
    fingersFileStream.seekWrite(currentFinger.myChunkOffset + currentFinger.myPosInChunkBytes + sizeof(KeyType)*2);
    fingersFileStream << currentFinger.itemsCount;
    if ( ! currentFinger.myChunkOffset )  // is root finger, FIXME check if header size not zero
        return 0;
    while (! fingersChain.empty()()) { // go all chain up
        // PrintFingerInfo(fingersChain.last(), "fingersChain.last() " + FNS);
        maxValueChanged  = theKey > fingersChain.back().maxKey;
        minValueChanged  = theKey < fingersChain.back().minKey;
        anyMinMaxChanged = minValueChanged || maxValueChanged;
        if (anyMinMaxChanged)
            UpdateMinMaxByFlags(fingersChain.back(), theKey);
        else
            break; // no sense to go up further
        fingersChain.pop_back();
    }
    fingersChain.clear();
    maxValueChanged  = theKey > rootFinger.maxKey; // update root finger FIXME check logic
    minValueChanged  = theKey < rootFinger.minKey;
    anyMinMaxChanged = minValueChanged || maxValueChanged;
    if (anyMinMaxChanged)
       UpdateMinMaxByFlags(rootFinger, theKey);
    return 0;
} */
/*
template <typename KeyType> int EgFingers<KeyType>::UpdateFingersChain() {
    bool splitParentChunk = false;
    // bool appendMode = true;
    if (rootFinger.chunkIsLeaf) { // no fingers chunks yet FIXME level num
        AddNewTopLevelChunk(); // add new chunk under root with currentFinger and newFinger
        return 0;
    }
    while (! fingersChain.empty()()) {
        parentFinger = fingersChain.back(); // get parent from chain
        fingersChain.pop_back();
        // appendMode = (theKey >= parentFinger.maxKey); // FIXME check
        LoadFingersChunk();
        if (parentFinger.itemsCount < egChunkCapacity) {
            InsertNewFingerToChunk();     // add new item  to localStream and recalc min/max
            splitParentChunk = false;
            break;  // get out
        } else {
            // if (appendMode)
                AppendNewFingersChunk();
            // else
            //    SplitFingersChunk();
            splitParentChunk = true;
                // FIXME refactor
            if (currentFinger.chunkIsLeaf)
                indexChunks->StoreFingerOffset(newFinger.nextChunkOffset, newFinger.nextChunkOffset);
            else
                StoreParentOffsetDirect(newFinger.nextChunkOffset, newFinger.nextChunkOffset); // FIXME check

            currentFinger = parentFinger; // level up
        }
    }
    if (fingersChain.empty() && splitParentChunk) { // new top chunk is required
        // if (appendMode)
            currentFinger = rootFinger; // level up
        AddNewTopLevelChunk(); // add new chunk under root with currentFinger and newFinger
    }
    else if (fingersChain.empty()) {
        maxValueChanged = theKey > rootFinger.maxKey;
        minValueChanged = theKey < rootFinger.minKey;
        anyMinMaxChanged = minValueChanged || maxValueChanged;
        if (anyMinMaxChanged)
           UpdateMinMaxByFlags(rootFinger, theKey);
    } */
 /* PrintFingerInfo(parentFinger, "parentFinger " + FNS);
    PrintFingerInfo(currentFinger, "currentFinger " + FNS);
    PrintFingerInfo(newFinger, "newFinger " + FNS); */
    /* return 0;
} */
/*
template <typename KeyType> int EgFingers<KeyType>::UpdateFingersChainAfterDelete()
{
    KeyType oldValue;
    keysCountType fingersCount;
    bool isFirstFinger;
    bool isLastFinger;
    if(! parentFingerOffset) { // check / update root header
        if (maxValueChanged || minValueChanged) {
            if (maxValueChanged)
                rootFinger.maxKey =  newMaxValue;
            if (minValueChanged)
                rootFinger.minKey =  newMinValue;
            StoreFingerMinMaxOnly(rootFinger);
        }
        return 0;
    }
    while (parentFingerOffset && (maxValueChanged || minValueChanged)) {
        currentFingerOffset = parentFingerOffset; // up
        uint64_t fingersChunkOffset = currentFingerOffset - ( currentFingerOffset % fingersChunkSize ); // calc this chunk offset
        fingersFileStream.seekRead(fingersChunkOffset + egChunkCapacity * oneFingerSize); // get next parent finger offset
        fingersFileStream >> parentFingerOffset;
        keysCountType fingerNum = (currentFingerOffset - fingersChunkOffset) / oneFingerSize; // check first/last finger for min/max update of root finger
        if (parentFingerOffset) {
            fingersFileStream.seekRead(parentFingerOffset + sizeof(KeyType)*2);
            fingersFileStream >> fingersCount;
        } else
            fingersCount = rootFinger.itemsCount;
        isFirstFinger = (fingerNum == 0);
        isLastFinger  = (fingerNum == fingersCount-1);
        if (maxValueChanged) {
            fingersFileStream.seekRead(parentFingerOffset + sizeof(KeyType));
            fingersFileStream >> oldValue;
            if (newMaxValue < oldValue) {
                fingersFileStream.seekWrite(parentFingerOffset + sizeof(KeyType));
                fingersFileStream << newMaxValue;
            }
            if ((newMaxValue >= oldValue) || (! isLastFinger)) {
                maxValueChanged = false;
                break;
            }
            // EG_LOG_STUB << "newMaxValue = " << newMaxValue << FN;
        } else if (minValueChanged) {
            fingersFileStream.seekRead(parentFingerOffset);
            fingersFileStream >> oldValue;
            if (newMinValue > oldValue) {
                fingersFileStream.seekWrite(parentFingerOffset);
                fingersFileStream << newMinValue;
            }
            if ((newMinValue <= oldValue) || (! isFirstFinger)) {
                minValueChanged = false;
                break;
            }
            // EG_LOG_STUB << "newMinValue = " << newMinValue << FN;
        } else {
            maxValueChanged = false;
            minValueChanged = false;
            break;
        }
    }   // while
        // EG_LOG_STUB << "parentFingerOffset = " << parentFingerOffset << FN;
    if(! parentFingerOffset) { // check / update root header
        if (maxValueChanged || minValueChanged) {
            if (maxValueChanged)
                rootFinger.maxKey =  newMaxValue;
            else if (minValueChanged)
                rootFinger.minKey =  newMinValue;
            StoreFingerMinMaxOnly(rootFinger);
        }
    }
    maxValueChanged = false; // reset
    minValueChanged = false;
    return 0;
} */
/*
template <typename KeyType> bool EgFingers<KeyType>::UpdateFingerCountAfterDelete(keysCountType newKeysCount) {
    // EG_LOG_STUB << "currentFingerOffset = " << hex << currentFingerOffset << FN;
    if (rootFinger.myLevel == 0)    // only root
        currentFingerOffset = 0;    // FIXME check if required
    fingersFileStream.seekWrite(currentFingerOffset + sizeof(KeyType) * 2); // update keys count
    fingersFileStream << newKeysCount;
    if (minValueChanged)
        UpdateMinValueUp();
    if (maxValueChanged)
        UpdateMaxValueUp();
    return fingersFileStream.good();
}
*/
/*
template <typename KeyType> int EgFingers<KeyType>::UpdateMinValueUp()
{
    if (rootFinger.myLevel == 0) { // root only FIXME update
        if (newMinValue != rootFinger.minKey) {
            rootFinger.minKey = newMinValue;
            fingersFileStream.seekWrite(0); // root
            fingersFileStream << newMinValue;
        }
        return 0;
    }
    KeyType oldValue;
    while (currentFingerOffset) { // iterate tree
        fingersFileStream.seekRead(currentFingerOffset); // get old value
        fingersFileStream >> oldValue;
        if (newMinValue != oldValue) { // changed
            fingersFileStream.seekWrite(currentFingerOffset); // update finger
            fingersFileStream << newMinValue;
            fingersChunkOffset = currentFingerOffset - ( (currentFingerOffset-rootHeaderSize) % fingersChunkSize );  // check finger offset in chunk
            if (currentFingerOffset == fingersChunkOffset) { // first finger, go up tree
                fingersFileStream.seekRead(fingersChunkOffset + egChunkCapacity * oneFingerSize);
                fingersFileStream >> currentFingerOffset; // parentFingerOffset
            } else
                return 0; // not first finger
        } else
            return 0; // min not changed FIXME move to indexes
    }
    if (newMinValue != rootFinger.minKey) {
        rootFinger.minKey = newMinValue;
        fingersFileStream.seekWrite(0); // root
        fingersFileStream << newMinValue;
    }
    return 0;
}
*/
/*
template <typename KeyType> int EgFingers<KeyType>::UpdateMaxValueUp()
{
    if (rootFinger.myLevel == 0) { // root only FIXME update
        if (newMaxValue != rootFinger.maxKey) {
            rootFinger.maxKey = newMaxValue;
            fingersFileStream.seekWrite(sizeof(KeyType)); // root
            fingersFileStream << newMaxValue;
        }
        return 0;
    }
    KeyType oldValue;
    while (currentFingerOffset) { // iterate tree
        fingersFileStream.seekRead(currentFingerOffset + sizeof(KeyType)); // get old value
        fingersFileStream >> oldValue;
        if (newMaxValue != oldValue) { // changed
            fingersFileStream.seekWrite(currentFingerOffset + sizeof(KeyType)); // update finger
            fingersFileStream << newMaxValue;
            fingersChunkOffset = currentFingerOffset - ( (currentFingerOffset-rootHeaderSize) % fingersChunkSize );  // check finger offset in chunk
            if (currentFingerOffset == fingersChunkOffset) { // first finger, go up tree
                fingersFileStream.seekRead(fingersChunkOffset + egChunkCapacity * oneFingerSize);
                fingersFileStream >> currentFingerOffset; // parentFingerOffset
            } else
                return 0; // not first finger
        } else
            return 0; // min not changed FIXME move to indexes
    }
    if (newMaxValue != rootFinger.maxKey) {
        rootFinger.maxKey = newMaxValue;
        fingersFileStream.seekWrite(sizeof(KeyType)); // root
        fingersFileStream << newMaxValue;
    }
    return 0;
}
*/
/*
template <typename KeyType> void EgFingers<KeyType>::UpdateMinValueInFile(uint64_t fingerOffset, KeyType& newValue) {
    fingersFileStream.seekWrite(fingerOffset);
    fingersFileStream << newValue;
}
*/
/*
template <typename KeyType> void EgFingers<KeyType>::UpdateMaxValueInFile(uint64_t fingerOffset, KeyType& newValue)
{
    fingersFileStream.seekWrite(fingerOffset + sizeof(KeyType));
    fingersFileStream << newValue;
}
*/
/*
template <typename KeyType> inline void EgFingers<KeyType>::UpdateTheFingerMinMax(egFinger<KeyType>& theFinger) {
    if (theKey > theFinger.maxKey)
        theFinger.maxKey =  theKey;
    if (theKey < theFinger.minKey)
        theFinger.minKey =  theKey;
}
*/

/*
template <typename KeyType> int EgFingers<KeyType>::DeleteParentFinger() { // recursive
    if (rootFinger.myLevel == 0) { // root only FIXME
        return 1; // delete files
    }
    while (currentFingerOffset) {
        currentFinger.myPosInChunkBytes = (currentFingerOffset - rootHeaderSize) % fingersChunkSize;
        currentFinger.myChunkOffset   =  currentFingerOffset - currentFinger.myPosInChunkBytes;
        fingersFileStream.seekRead(currentFinger.myChunkOffset + egChunkCapacity * oneFingerSize); // get parent
        fingersFileStream >> parentFingerOffset;
        if (parentFingerOffset) { // parent not root FIXME 
            fingersFileStream.seekRead(parentFingerOffset);
            fingersFileStream >> parentFinger.minKey;
            fingersFileStream >> parentFinger.maxKey;
            fingersFileStream >> currentKeysCount;
        } else {
            parentFinger = rootFinger;
            currentKeysCount = rootFinger.itemsCount;
        }
    /*  PrintFingerInfo(parentFinger, "parentFinger " + FNS);
        PrintFingerInfo(currentFinger, "currentFinger " + FNS);

        EG_LOG_STUB << "currentFingerOffset " << hex << (int) currentFingerOffset
                 << " , parentFingerOffset " << hex << (int) parentFingerOffset
                 << " , currentKeysCount = " << currentKeysCount << FN; */
        /* if (currentKeysCount > 1) { // not last finger n=in chunk
            DeleteCurrentFinger();
            fingersFileStream.seekWrite(parentFingerOffset + sizeof(KeyType)*2); // update count
            keysCountType newCount = currentKeysCount - 1;
            fingersFileStream << newCount;
            if (parentFinger.minKey < currentFinger.minKey) {
                newMinValue = currentFinger.minKey;
                currentFingerOffset = parentFingerOffset;
                UpdateMinValueUp();
            }
            if (parentFinger.maxKey > currentFinger.maxKey)
            {
                newMaxValue = currentFinger.maxKey;
                currentFingerOffset = parentFingerOffset;
                UpdateMaxValueUp();
            }
            return 0;
        } else { // last finger
            DeleteFingersChunk(currentFinger.myChunkOffset); // just write zeroes
            currentFingerOffset = parentFingerOffset; // go up, delete parent
        }
    }
    return 0;
}
*/

template <typename KeyType> int EgFingers<KeyType>::FindFingerEQ_old() {
    int fingerPosition = 0;
    do
        ReadFingerFromChunk(currentFinger, fingerPosition);
    while ((currentFinger.maxKey < theKey) && (++fingerPosition < parentFinger.itemsCount));
    if (theKey < currentFinger.minKey) // check if key between fingers
        return 1;
    return 0;
}

template <typename KeyType> int EgFingers<KeyType>::FindIndexChunkEQ_old() {
    int result = 0;
    // EG_LOG_STUB << "theKey = " << hex << theKey << FN;
    if ((theKey < rootFinger.minKey) || (theKey > rootFinger.maxKey)) // key out of range
            return 1;
    if (rootFinger.myChunkIsLeaf) { // only root finger exists
        currentFinger = rootFinger;
        return 0;
    }
    parentFinger = rootFinger;
        
    do { // go fingers chain
        // PrintFingerInfo(parentFinger, "parentFinger " + FNS);
        LoadFingersChunk(parentFinger.nextChunkOffset);;
        // GetChunkIsLeaf(currentFinger.myChunkIsLeaf);
        // EG_LOG_STUB << "myLevel = " << currentFinger.myLevel << FN;
            // if one finger just return it
        if (parentFinger.itemsCount == 1)
            ReadFingerFromChunk(currentFinger, 0);
        else
            result = FindFingerEQ_old();  // go get currentFinger, 1 - between fingers, key not found, exit
        // PrintFingerInfo(currentFinger, "currentFinger " + FNS);

        if (! FingerIsRoot(currentFinger)) // FIXME level num
            parentFinger = currentFinger;
    } while ((! FingerIsRoot(currentFinger)) && !result);
    currentFinger.myChunkOffset = parentFinger.nextChunkOffset;  // FIXME chck if required
    return result;
}

template <typename KeyType> int EgFingers<KeyType>::FindIndexChunkLE_old() {
    int result = 0;
    // EG_LOG_STUB << "theKey = " << hex << theKey << FN;
    if (theKey < rootFinger.minKey) // key out of range
            return 1;
    if (rootFinger.myChunkIsLeaf) { // only root finger exists
        currentFinger = rootFinger;
        return 0;
    }
    parentFinger = rootFinger;
    do { // go fingers chain
        // PrintFingerInfo(parentFinger, "parentFinger " + FNS);
        LoadFingersChunk(parentFinger.nextChunkOffset);;
        // EG_LOG_STUB << "myLevel = " << currentFinger.myLevel << FN;
            // if one finger just return it
        if (parentFinger.itemsCount == 1)
            ReadFingerFromChunk(currentFinger, 0);
        else
            FindFingerLE();  // go get currentFinger
        // PrintFingerInfo(currentFinger, "currentFinger " + FNS);
        if (! FingerIsRoot(currentFinger)) // FIXME level num
            parentFinger = currentFinger;
    } while ((! FingerIsRoot(currentFinger)) && !result);
    currentFinger.myChunkOffset = parentFinger.nextChunkOffset;  // FIXME chck if required
    return result;
}


template <typename KeyType> void EgFingers<KeyType>::FindFingerLE_old() {
    int fingerPosition = parentFinger.itemsCount-1;
    do {
        ReadFingerFromChunk(currentFinger, fingerPosition);
        // EG_LOG_STUB << "fingerPosition = " << fingerPosition << " theKey = " << HEX << theKey 
        //            << " currentFinger.minKey = " << currentFinger.minKey << FN;
    } while ((currentFinger.minKey > theKey) && (--fingerPosition >= 0));
}

template <typename KeyType> void EgFingers<KeyType>::FindFingerLT_old() {   
    int fingerPosition = parentFinger.itemsCount-1;
    do
        ReadFingerFromChunk(currentFinger, fingerPosition);
    while ((currentFinger.minKey >= theKey) && (--fingerPosition >= 0));
}

template <typename KeyType> int EgFingers<KeyType>::FindIndexChunkLT_old() {
    int result = 0;
    // EG_LOG_STUB << "theKey = " << hex << theKey << FN;
    if (theKey <= rootFinger.minKey) // key out of range
            return 1;
    if (rootFinger.myChunkIsLeaf) { // only root finger exists
        currentFinger = rootFinger;
        return 0;
    }
    parentFinger = rootFinger;
    do { // go fingers chain
        // PrintFingerInfo(parentFinger, "parentFinger " + FNS);
        LoadFingersChunk(parentFinger.nextChunkOffset);;
        // EG_LOG_STUB << "myLevel = " << currentFinge.myLevel << FN;
        if (parentFinger.itemsCount == 1) // if one finger just return it
            ReadFingerFromChunk(currentFinger, 0);
        else
            FindFingerLT();  // go get currentFinger
        // PrintFingerInfo(currentFinger, "currentFinger " + FNS);
        if (! FingerIsRoot(currentFinger)) // FIXME level num
            parentFinger = currentFinger;
    } while ((! FingerIsRoot(currentFinger)) && !result);
    currentFinger.myChunkOffset = parentFinger.nextChunkOffset;  // FIXME chck if required
    return result;
}

template <typename KeyType> int EgFingers<KeyType>::FindIndexChunkGT_old() {
    int result = 0;
    // EG_LOG_STUB << "theKey = " << hex << theKey << FN;
    if (theKey >= rootFinger.maxKey) // key out of range
            return 1;
    if (rootFinger.myChunkIsLeaf) { // only root finger exists
        currentFinger = rootFinger;
        return 0;
    }
    parentFinger = rootFinger;
    do { // go fingers chain
        // PrintFingerInfo(parentFinger, "parentFinger " + FNS);
        LoadFingersChunk(parentFinger.nextChunkOffset);;
        // EG_LOG_STUB << "myLevel = " << currentFinger.myLevel << FN;
        if (parentFinger.itemsCount == 1) // if one finger just return it
            ReadFingerFromChunk(currentFinger, 0);
        else
            FindFingerGT(theKey);  // go get currentFinger
        // PrintFingerInfo(currentFinger, "currentFinger " + FNS);
        if (! FingerIsRoot(currentFinger)) // FIXME level num
            parentFinger = currentFinger;
    } while ((! FingerIsRoot(currentFinger)) && !result);
    currentFinger.myChunkOffset = parentFinger.nextChunkOffset;  // FIXME chck if required
    return result;
}

template <typename KeyType> int EgFingers<KeyType>::FindIndexChunkGE_old() {
    int result = 0;
    // EG_LOG_STUB << "theKey = " << hex << theKey << FN;
    if (theKey > rootFinger.maxKey) // key out of range
            return 1;
    if (rootFinger.myChunkIsLeaf) { // only root finger exists
        currentFinger = rootFinger;
        return 0;
    }
    parentFinger = rootFinger;
    do { // go fingers chain
        // PrintFingerInfo(parentFinger, "parentFinger " + FNS);
        LoadFingersChunk(parentFinger.nextChunkOffset);;
        // EG_LOG_STUB << "myLevel = " << currentFinger.myLevel << FN;
        if (parentFinger.itemsCount == 1) // if one finger just return it
            ReadFingerFromChunk(currentFinger, 0);
        else
            FindFingerGE(theKey);  // go get currentFinger
        // PrintFingerInfo(currentFinger, "currentFinger " + FNS);
        if (! FingerIsRoot(currentFinger)) // FIXME level num
            parentFinger = currentFinger;
    } while ((! FingerIsRoot(currentFinger)) && !result);
    currentFinger.myChunkOffset = parentFinger.nextChunkOffset;  // FIXME chck if required
    return result;
}
/*
template <typename KeyType> inline void EgFingers<KeyType>::FindFingerLE(KeyType& Key) {   
    int fingerPosition { parentFinger.itemsCount-1 }; // last
    do
        ReadFingerFromChunk(currentFinger, fingerPosition);
    while ((currentFinger.minKey > Key) && (--fingerPosition >= 0));
}*/
/*
template <typename KeyType> bool EgFingers<KeyType>::FindIndexChunkLE(KeyType& Key) { // indexes chunk, not fingers one FIXME compare to FindLE
    if (! OpenFileStream())
        return false;
    bool res {true};
    if (CompareGT(rootFinger.minKey, Key)) { // (rootFinger.minKey > Key) {
        fingersFileStream.close();
        return false; // key out of range
    }
    currentFinger = rootFinger;
    while (! currentFinger.myChunkIsLeaf && res) {
        if (CompareGT(currentFinger.minKey, Key)) { // (currentFinger.minKey > Key) { // FIXME check condition cases
            fingersFileStream.close();
            return false; // key out of range
        }
        parentFinger = currentFinger;
        res = res && LoadFingersChunk(parentFinger.nextChunkOffset); // sets parentFinger.itemsCount
        FindLastFinger(Key, CompareGT); // (currentValue > key)
        // FindFingerLE(Key); // to currentFinger
        fingersChain.push_back(parentFinger); // fill fingers chain to track path
    }
    return res;
} */

/*
template <typename KeyType> inline void EgFingers<KeyType>::FindFingerLT(KeyType& Key) {   
    int fingerPosition { parentFinger.itemsCount-1 }; // last
    do
        ReadFingerFromChunk(currentFinger, fingerPosition);
    while ((currentFinger.minKey >= Key) && (--fingerPosition >= 0));
} */

/*
template <typename KeyType> inline void EgFingers<KeyType>::FindFingerGT(KeyType& Key) {
    int fingerPosition {0};
    do
        ReadFingerFromChunk(currentFinger, fingerPosition);
    while ((currentFinger.maxKey <= Key) && (++fingerPosition < parentFinger.itemsCount));
} */

/*
template <typename KeyType> inline void EgFingers<KeyType>::FindFingerGE(KeyType& Key) {
    int fingerPosition {0}; // parentFinger.itemsCount-1;
    do {
        ReadFingerFromChunk(currentFinger, fingerPosition);
        // EG_LOG_STUB << "fingerPosition = " << fingerPosition << " theKey = " << HEX << theKey 
        //            << " currentFinger.minKey = " << currentFinger.minKey << FN;
    } while ((currentFinger.maxKey < Key) && (++fingerPosition <  parentFinger.itemsCount));
} */

template <typename KeyType> bool EgFingers<KeyType>::FindIndexChunkGE(KeyType& Key) { // indexes chunk, not fingers one FIXME compare to FindLE
    return FindIndexChunkGreater(Key, CompareLT);
}

template <typename KeyType> bool EgFingers<KeyType>::FindIndexChunkGT(KeyType& Key) { // indexes chunk, not fingers one FIXME compare to FindLE
    return FindIndexChunkGreater(Key, CompareLE);
}

/*

template <typename KeyType> bool EgFingers<KeyType>::FindIndexChunkGE(KeyType& Key) { // indexes chunk, not fingers one FIXME compare to FindLE
    if (! OpenFileStream())
        return false;
    bool res {true};
    if (Key > rootFinger.maxKey) {
        fingersFileStream.close();
        return false; // key out of range
    }
    currentFinger = rootFinger;
    while (! currentFinger.myChunkIsLeaf && res) {
        if (Key > currentFinger.maxKey) {
            fingersFileStream.close();
            return false; // key out of range
        }
        parentFinger = currentFinger;
        res = res && LoadFingersChunk(parentFinger.nextChunkOffset); // sets parentFinger.itemsCount
        FindFirstFinger(Key, CompareLT); // (currentValue < key)
        // FindFingerGE(Key); // to currentFinger
        fingersChain.push_back(parentFinger); // fill fingers chain to track path
    }
    return res;
}

template <typename KeyType> bool EgFingers<KeyType>::FindIndexChunkGT(KeyType& Key) { // indexes chunk, not fingers one FIXME compare to FindLE
    if (! OpenFileStream())
        return false;
    bool res {true};
    if (Key >= rootFinger.maxKey) {
        fingersFileStream.close();
        return false; // key out of range
    }
    currentFinger = rootFinger;
    while (! currentFinger.myChunkIsLeaf && res) {
        if (Key >= currentFinger.maxKey) { // FIXME check condition cases
            fingersFileStream.close();
            return false; // key out of range
        }
        parentFinger = currentFinger;
        res = res && LoadFingersChunk(parentFinger.nextChunkOffset); // sets parentFinger.itemsCount
        FindFirstFinger(Key, CompareLE); // (currentValue <= key)
        // FindFingerGT(Key); // to currentFinger
        fingersChain.push_back(parentFinger); // fill fingers chain to track path
    }
    return res;
}
*/



template <typename KeyType> bool EgFingers<KeyType>::FindIndexChunkLT(KeyType& Key) { // indexes chunk, not fingers one FIXME compare to FindLE
    return FindIndexChunkLess(Key, CompareGE);
}

template <typename KeyType> bool EgFingers<KeyType>::FindIndexChunkLE(KeyType& Key) { // indexes chunk, not fingers one FIXME compare to FindLE
    return FindIndexChunkLess(Key, CompareGT);
}

