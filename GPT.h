#pragma once
#include <cstdint>
#include "main.h"

namespace GPT {

    PACK(struct GPTHeader {
        uint8_t Signature[8];
        uint8_t Revision[4];
        uint32_t HeaderSize;
        uint32_t CRC32;
        uint32_t Reserved;
        uint64_t CurrentLBA;
        uint64_t BackupLBA;
        uint64_t FirstUsableLBAPartitions;
        uint64_t LastUsableLBAPartitions;
        GUID DiskGUID;
        uint64_t PartitionEntriesStartingLBA;
        uint32_t NumberPartitionEntries;
        uint32_t SizePartitionEntry;
        uint32_t CRC32PartitionArray;
    });

    PACK(struct GUIDPartitionEntryFormat {
        GUID PartitionTypeGUID;
        GUID UniquePartitionGUID;
        uint64_t FirstLBA;
        uint64_t LastLBA;
        uint64_t Flags;
        uint16_t PartitionName[36];
    });


    struct Partitions {
        GUIDPartitionEntryFormat* AllParitions[MaxParitionsNumber];
        uint64_t NumberPartitionsCreated;
        BitmapHeap IsPartitionsEntryBitmapFree;
    };

    struct PartionsInfo {
        GUIDPartitionEntryFormat* Partition;
        uint64_t PatitionNumber;
    };

    extern PartionsInfo** AllPartitionsInfo;
    extern uint64_t AllPartitionsInfoNumber;


    GPTHeader* GetGPTHeader();


    GUIDPartitionEntryFormat* GetGUIDPartitionEntryFormat(uint64_t LBAAddress, uint8_t Which);

    uint64_t GetFirstFreeLBA();

    Partitions* GetAllPartitions();

    uint64_t GetFreeSizePatition();

    GUID* GetReservedGUIDPartitionType();
    GUID* GetDataGUIDPartitionType();

    GUIDPartitionEntryFormat* GetPartitionByGUID(GUID* guid);

    void AssignNamePartitonsGUID();



    class Partition {
    public:
        Partition(GUIDPartitionEntryFormat* partition);
        void Read(uint64_t firstByte, uint64_t size, void* buffer);
        void Write(uint64_t firstByte, uint64_t size, void* buffer);
        GUIDPartitionEntryFormat* partition;
    };
}