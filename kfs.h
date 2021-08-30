#pragma once

#include "gpt.h"
#include "main.h"
#include <cstdint>

namespace FileSystem {
#define MaxPath 512
#define MaxName 256
#define MaxPassword 256
#define MaxUserRight 256   

#define GUIDData1 0x47A1ACC0
#define GUIDData2 0x3B41 
#define GUIDData3 0x2A53
#define GUIDData4 0xF38D3D321F6D 

#define HeaderInfoPosition sizeof(ClusterHeader)
#define FileInfoPosition sizeof(ClusterHeader) + sizeof(HeaderInfo)
#define DataPosition sizeof(ClusterHeader) + sizeof(HeaderInfo) + sizeof(FileInfo)

    PACK(struct root {
        char        Name[MaxName];
        uint64_t    FirstClusterForFile;
        uint64_t    FirstClusterFile;
        uint64_t    FID;
        uint64_t    LastClusterAllocated;
    });

    PACK(struct KFSinfo {
        uint64_t      BitmapSizeByte;
        uint64_t      BitmapSizeCluster;
        uint64_t      BitmapPosition;
        uint64_t      ClusterSize;
        uint64_t      NumberOfCluster;
        root          Root;
        GUID          IsInit;
    });



    PACK(struct ClusterHeader {
        uint64_t LastCluster;
        uint64_t NextCluster;
    });

    PACK(struct timeInfoFS {
        Time CreateTime;
        Time ModifyTime;
    });

    PACK(struct HeaderInfo {
        bool IsFile = false;
        uint64_t FID = 0;
        uint64_t ParentLocationCluster = 0; //location of the parent's header
        uint64_t Version = 0;
    });

    PACK(struct FileInfo {
        /* location info */
        uint64_t LastClusterOfTheFile;
        uint64_t ClusterHeaderPostition;
        uint64_t BytesSize;
        uint64_t ClusterSize; //number of Cluster 
        char Path[MaxPath];
        char Name[MaxName];

        /* userRight */
        char Password[MaxPassword];
        GUID Owner;

        /* time */
        timeInfoFS TimeInfoFS;

        uint64_t NextCluster;
    });

    PACK(struct FolderInfo {
        /* location info */
        uint64_t ClusterHeaderPostion;
        uint64_t FirstClusterData;
        uint64_t numberFiles;
        uint64_t BytesSize;
        uint64_t FileClusterSize; //number of Cluster 
        char Path[MaxPath];
        char Name[MaxName];

        /* userRight */
        char Password[MaxPassword];
        GUID Owner;

        /* time */
        timeInfoFS TimeInfoFS;

        uint64_t LastClusterRequested;
        uint64_t Mode;

    });

    PACK(struct AllocatePartition {
        uint64_t FirstCluster;
        uint64_t LastCluster;
    });

    struct PartitionNameAndGUID {
        char* name;
        GUID* UniquePartitionGUID;
    };

    class File;
    class Folder;

    PACK(struct KFS {
        KFS(GPT::Partition* partition);

        void InitKFS();

        File* OpenFile(char* filePath);
        void Close(File* file);

        AllocatePartition* Allocate(uint64_t size, Folder* folder, uint64_t lastClusterRequested);
        void Free(uint64_t Cluster, bool DeleteData);
        uint64_t RequestCluster();
        void LockCluster(uint64_t Cluster);
        void UnlockCluster(uint64_t Cluster);
        bool CheckCluster(uint64_t Cluster);
        void GetClusterData(uint64_t Cluster, void* buffer);
        void SetClusterData(uint64_t Cluster, void* buffer);

        uint64_t mkdir(char* filePath, uint64_t mode);
        Folder* readdir(char* filePath);

        void flist(char* filename);
        bool IsDirExist(char* filepath);

        File* fopen(char* filename, char* mode);
        FileInfo* NewFile(char* filePath, Folder* folder);

        uint64_t GetFID();
        bool UpdatePartitionInfo();
        void UpdateFolderInfo(FolderInfo* folderInfo);
        void UpdateFileInfo(FileInfo* fileInfo);


        GPT::Partition* globalPartition;
        KFSinfo* KFSPartitionInfo;
    });

    PACK(struct File {
        FileInfo* fileInfo;
        char* Mode;
        KFS* kfs;
        uint64_t Read(uint64_t start, uint64_t size, void* buffer);
        uint64_t Write(uint64_t start, uint64_t size, void* buffer);
    });

    PACK(struct Folder {
        FolderInfo* folderInfo;
        KFS* kfs;
    });
};
