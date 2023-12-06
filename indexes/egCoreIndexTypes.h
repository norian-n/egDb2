#pragma once
#include <cstdint>

#define EG_LOG_STUB std::cout
#define FN " [" << __FUNCTION__  << "(), " << __FILE__ << "]" << std::endl
#define FNS std::string(" [") + std::string(__FUNCTION__) + std::string("(), ") + std::string(__FILE__) + std::string("]")
#define HEX std::hex

    // indexes related types
enum EgIndexFamilyTypeEnum {
    egNoType,
    egIntFT,
    egUnsignedIntFT,
    egFloatFT,
    egHashFT
};

struct EgIndexSettingsType {
    uint8_t                     indexSizeBits   {0};        // 8, 16, 32, 64, etc.
    uint8_t                     hashFunctionID  {0};        // data to index transformation function
    EgIndexFamilyTypeEnum       indexFamilyType {egNoType}; // egIntFT, egUnsignedIntFT, egFloatFT, etc.
};

template <typename KeyType> struct EgIndexStruct
{
    KeyType indexKey;
    uint64_t dataOffset;
};

//  ============================================================================
//          FINGERS DATA TYPES
//  ============================================================================

// Finger in the file: KeyMin KeyMax nextChunkOffset isLeaf(root only)
// Chunk in the file:  FingersArray parentFingerOffset chunkIsLeaf fingersCount 

typedef uint16_t keysCountType; // keys in a chunk
typedef uint16_t fingersLevelType;

namespace egIndexesNamespace {
    // uint64_t indexHeaderSize {sizeof(uint64_t)}; // * 2};   // header of indexes file
    const keysCountType egChunkCapacity {4};                // keys in chunk
} // egIndexesNamespace

template <typename KeyType> struct egFinger {
    KeyType minKey {0};             // max key value of next level/pointed chunk
    KeyType maxKey {0};             // max key value of next level/pointed chunk

    keysCountType itemsCount {0};   // next level/pointed chunk keys or fingers, stored in the chunk
    uint64_t nextChunkOffset {0};   // next level/pointed chunk file position (fingers or indexes file)

    uint64_t myPosInChunkBytes {0}; // finger position in current chunk
    uint64_t myChunkOffset     {0}; // file offset of current chunk
    uint8_t  myChunkIsLeaf     {0xff};// next level/pointed chunk is indexes (True, >0) or fingers (False, 0) chunk    
    uint64_t parentChunkOffset {0}; // file offset of parent's chunk
};
