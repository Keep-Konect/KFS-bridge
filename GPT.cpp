#pragma once

#include "GPT.h"
#include "kfs.h"
#include <stdlib.h> 
#include <stdio.h>
#include <string.h>

namespace GPT {

    PartionsInfo** AllPartitionsInfo;
    uint64_t AllPartitionsInfoNumber;
    char* Buffer = new char[0x1000];

    GPTHeader* GetGPTHeader() {
        memset(Buffer, 0, sizeof(GPTHeader));

        std::ifstream infile(filePath);

        infile.seekg(SizeofLBASector);
        infile.read(Buffer, SizeofLBASector);
        GPTHeader* ReturnValue = (GPTHeader*)malloc(sizeof(GPTHeader));
        memcpy(ReturnValue, Buffer, sizeof(GPTHeader));
        infile.close();
        return ReturnValue;
    }

    GUIDPartitionEntryFormat* GetGUIDPartitionEntryFormat(uint64_t LBAAddress, uint8_t which) {
        uint8_t MaxGUIDPartitionEntryFormatPerSectors = SizeofLBASector / sizeof(GUIDPartitionEntryFormat);
        memset(Buffer, 0, SizeofLBASector);

        void* buffer = malloc(sizeof(GUIDPartitionEntryFormat));
        memset(buffer, 0, sizeof(GUIDPartitionEntryFormat));

        std::ifstream infile(filePath);

        infile.seekg(LBAAddress * SizeofLBASector);

        if (infile.read(Buffer, SizeofLBASector)) {
            infile.seekg(0);
            infile.close();
            memcpy(buffer, (Buffer + (which * sizeof(GUIDPartitionEntryFormat))), sizeof(GUIDPartitionEntryFormat));
            GUIDPartitionEntryFormat* ReturnValue = (GUIDPartitionEntryFormat*)buffer;
            return ReturnValue;
        }
        else {
            infile.close();
            free(buffer);
            return NULL;
        }
    }

    uint64_t GetFirstFreeLBA() {
        GPTHeader* gptHeader = GetGPTHeader();
        Partitions* partitions = GetAllPartitions();
        uint64_t MaxLastUsedLBA = 0;
        if (partitions->NumberPartitionsCreated == 0) {

            return gptHeader->FirstUsableLBAPartitions;
        }
        for (int i = 0; i < partitions->NumberPartitionsCreated; i++) {
            if (partitions->AllParitions[i]->LastLBA > MaxLastUsedLBA) {
                MaxLastUsedLBA = partitions->AllParitions[i]->LastLBA;
            }
        }
        return MaxLastUsedLBA + 1; //because the last is used so we need to return the free
    }

    GUIDPartitionEntryFormat* GetPartitionByGUID(GUID* guid) {
        Partitions* AllPartitions = GetAllPartitions();

        for (int i = 0; i < AllPartitions->NumberPartitionsCreated; i++) {
            if (AllPartitions->AllParitions[i]->PartitionTypeGUID.Data1 == guid->Data1 &&
                AllPartitions->AllParitions[i]->PartitionTypeGUID.Data2 == guid->Data2 &&
                AllPartitions->AllParitions[i]->PartitionTypeGUID.Data3 == guid->Data3 &&
                AllPartitions->AllParitions[i]->PartitionTypeGUID.Data4 == guid->Data4) {
                return AllPartitions->AllParitions[i];
            }
        }
        return NULL;
    }

    Partitions* GetAllPartitions() {
        GPTHeader* gptHeader = GetGPTHeader();
        Partitions* ReturnValue = (Partitions*)malloc(sizeof(Partitions));

        memset(ReturnValue, 0, sizeof(Partitions));
        ReturnValue->IsPartitionsEntryBitmapFree = BitmapHeap(gptHeader->NumberPartitionEntries);

        uint8_t MaxGUIDPartitionEntryFormatPerSectors = SizeofLBASector / sizeof(GUIDPartitionEntryFormat);
        uint64_t PartitionEntriesStartingLBA = gptHeader->PartitionEntriesStartingLBA;


        GUIDPartitionEntryFormat* CheckEntry = (GUIDPartitionEntryFormat*)malloc(sizeof(GUIDPartitionEntryFormat));;
        for (int i = 0; i < gptHeader->NumberPartitionEntries; i++) {
            CheckEntry = (GUIDPartitionEntryFormat*)malloc(sizeof(GUIDPartitionEntryFormat));
            CheckEntry = GetGUIDPartitionEntryFormat(PartitionEntriesStartingLBA + (i / MaxGUIDPartitionEntryFormatPerSectors), i % MaxGUIDPartitionEntryFormatPerSectors);
            if (CheckEntry->FirstLBA != 0) {
                ReturnValue->AllParitions[ReturnValue->NumberPartitionsCreated] = CheckEntry;
                ReturnValue->NumberPartitionsCreated++;
                ReturnValue->IsPartitionsEntryBitmapFree.Set(i, false);
            }
            else {
                free(CheckEntry);
                ReturnValue->IsPartitionsEntryBitmapFree.Set(i, true);
            }
        }

        return ReturnValue;
    }

    uint64_t GetFreeSizePatition() {
        GPTHeader* gptHeader = GetGPTHeader();
        Partitions* partitions = GetAllPartitions();

        uint64_t UsedLBASectors = 0;
        uint64_t TotalUsableLBASectors = gptHeader->LastUsableLBAPartitions - gptHeader->FirstUsableLBAPartitions;

        for (int i = 0; i < partitions->NumberPartitionsCreated; i++) {
            //the problem come from here
            UsedLBASectors += partitions->AllParitions[i]->LastLBA - partitions->AllParitions[i]->FirstLBA;
        }
        uint64_t freeSizeSectors = TotalUsableLBASectors - UsedLBASectors;
        uint64_t freeSize = (freeSizeSectors * SizeofLBASector) - 1;
        return freeSize;
    }


    GUID* GetReservedGUIDPartitionType() {
        GUID* KotReservedGUID = (GUID*)malloc(sizeof(GUID));
        KotReservedGUID->Data1 = 0x47A1ACC0;
        KotReservedGUID->Data2 = 0x3B40;
        KotReservedGUID->Data3 = 0x2A53;
        KotReservedGUID->Data4 = 0xF38D3D321F6D;
        return KotReservedGUID;
    }

    GUID* GetDataGUIDPartitionType() {
        GUID* KotReservedGUID = (GUID*)malloc(sizeof(GUID));
        KotReservedGUID->Data1 = 0x64617461;
        KotReservedGUID->Data2 = 0x3B40;
        KotReservedGUID->Data3 = 0x2A53;
        KotReservedGUID->Data4 = 0xF38D3D321F6D;
        return KotReservedGUID;
    }

    GUID* GetSystemGUIDPartitionType() {
        GUID* KotReservedGUID = (GUID*)malloc(sizeof(GUID));
        KotReservedGUID->Data1 = 0xC12A7328;
        KotReservedGUID->Data2 = 0xF81F;
        KotReservedGUID->Data3 = 0x11D2;
        KotReservedGUID->Data4 = 0x00A0C93EC93B;
        return KotReservedGUID;
    }

    void AssignNamePartitonsGUID() {
        bool systemPartInit = false;
        uint64_t PatitionNumber = 0;
        for (int i = 0; i < AllPartitionsInfoNumber; i++) {
            if (AllPartitionsInfo[i]->Partition->Flags == 7) { //flags disk system
                AllPartitionsInfo[i]->PatitionNumber = 0;
                systemPartInit = true;
            }
            else if (AllPartitionsInfo[i]->Partition->Flags == 5) { //flags to mount the disk
                PatitionNumber++;
                AllPartitionsInfo[i]->PatitionNumber = PatitionNumber;
            }
        }
    }

    /* Partition class */

    Partition::Partition(GUIDPartitionEntryFormat* partition) {
        this->partition = partition;

    }

    void Partition::Read(uint64_t firstByte, uint64_t size, void* buffer) {
        uint64_t LBAFirstSector = this->partition->FirstLBA + (firstByte / SizeofLBASector);
        memset(buffer, 0, size);
        uint64_t sizeRead = 0;
        uint64_t sizeToRead = 0;
        uint64_t sectorsToRead = 0;
        uint64_t sectorsRead = 0;
        std::ifstream infile(filePath, std::ios::binary | std::ios::out);

        for (int i = 0; i < Divide(size, 0x1000); i++) {
            sizeToRead = size - sizeRead;
            if (sizeToRead > 0x1000) {
                sizeToRead = 0x1000;
            }

            sectorsToRead = Divide(sizeToRead, SizeofLBASector);
            infile.seekg((LBAFirstSector + sectorsRead) * SizeofLBASector);
            infile.read(Buffer, sectorsToRead * SizeofLBASector);
            infile.seekg(0);
            if (sizeRead != 0) {
                memcpy((void*)((uint64_t)buffer + sizeRead), Buffer, sizeToRead);
            }
            else {
                memcpy((void*)((uint64_t)buffer + sizeRead), (void*)((uint64_t)Buffer + firstByte % SizeofLBASector), sizeToRead); //Get the correct first byte
            }

            sizeRead += sizeToRead;
            sectorsRead += sectorsToRead;
        }
        infile.close();

    }

    void Partition::Write(uint64_t firstByte, uint64_t size, void* buffer) {
        uint64_t LBAFirstSector = (firstByte / SizeofLBASector) + this->partition->FirstLBA;

        uint64_t sizeWrite = 0;
        uint64_t sizeToWrite = 0;
        uint64_t sectorsToWrite = 0;
        uint64_t sectorsWrite = 0;
        std::fstream file(filePath, std::ios::binary | std::ios::out | std::ios::in);
        for (int i = 0; i < Divide(size, 0x1000); i++) {
            sizeToWrite = size - sizeWrite;
            if (sizeToWrite > 0x1000) {
                sizeToWrite = 0x1000;
            }

            sectorsToWrite = Divide(sizeToWrite, SizeofLBASector);
            
            file.seekg((LBAFirstSector + sectorsWrite) * SizeofLBASector);
            file.read(Buffer, sectorsToWrite * SizeofLBASector);
            file.seekg(0);

            if (sizeWrite != 0) {
                memcpy(Buffer, (void*)((uint64_t)buffer + sizeWrite), sizeToWrite);
            }
            else {
                memcpy((void*)((uint64_t)Buffer + firstByte % SizeofLBASector), (void*)((uint64_t)buffer + sizeWrite), sizeToWrite);
            }

            file.seekp((LBAFirstSector + sectorsWrite) * SizeofLBASector);
            file.write(Buffer, sectorsToWrite * SizeofLBASector);
            file.seekp(0);

            sizeWrite += sizeToWrite;
            sectorsWrite += sectorsToWrite;
        }
        file.close();
    }
}

