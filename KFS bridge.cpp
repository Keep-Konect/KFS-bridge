#include <stdio.h>
#include <iostream>
#include <fstream>
#include "main.h"
#include "GPT.h"
#include "kfs.h"

std::fstream MainFile;
char* filePath;

int main()
{   
    filePath = (char*)"D:\\Data\\users\\Konect\\1Documents\\programmation\\Kot\\Data\\debugDisk.vhd";
    GPT::Partition dataPartition = GPT::Partition(GPT::GetPartitionByGUID(GPT::GetDataGUIDPartitionType()));
    FileSystem::KFS* Fs = new FileSystem::KFS(&dataPartition); 
    Fs->mkdir((char*)"system", 777);
    Fs->mkdir((char*)"system/background", 777);
    FileSystem::File* Picture = Fs->fopen((char*)"system/background/1.bmp", (char*)"r");
    char* filename = (char*)"D:\\Data\\users\\Konect\\1Documents\\programmation\\Kot\\Data\\1.bmp";
    std::ifstream PictureWindows(filename, std::ifstream::ate | std::ifstream::binary);
    uint64_t pictureSize = PictureWindows.tellg();
    void* bufferPicture = malloc(pictureSize);
    PictureWindows.read((char*)bufferPicture, pictureSize);
    PictureWindows.close();
    printf("%x", bufferPicture);
    Picture->Write(0, pictureSize, bufferPicture);
    Fs->flist((char*)"system/background");
    printf("%x", Picture->fileInfo->BytesSize);
}

bool Bitmap::operator[](uint64_t index) {
    return Get(index);
}

bool Bitmap::Get(uint64_t index) {
    if (index > Size * 8) return false;
    uint64_t byteIndex = index / 8;
    uint8_t bitIndex = index % 8;
    uint8_t bitIndexer = 0b10000000 >> bitIndex;
    if ((Buffer[byteIndex] & bitIndexer) > 0) {
        return true;
    }
    return false;
}

bool Bitmap::Set(uint64_t index, bool value) {
    if (index > Size * 8) return false;
    uint64_t byteIndex = index / 8;
    uint8_t bitIndex = index % 8;
    uint8_t bitIndexer = 0b10000000 >> bitIndex;
    Buffer[byteIndex] &= ~bitIndexer;
    if (value) {
        Buffer[byteIndex] |= bitIndexer;
    }
    return true;
}

BitmapHeap::BitmapHeap(uint64_t size) {
    bitmap = (Bitmap*)malloc(sizeof(Bitmap));
    bitmap->Size = size;
    bitmap->Buffer = (uint8_t*)malloc(Divide(size, 8));
}

void BitmapHeap::Free() {
    free(bitmap->Buffer);
    free(bitmap);
}

bool BitmapHeap::operator[](uint64_t index) {
    return bitmap->Get(index);
}

bool BitmapHeap::Get(uint64_t index) {
    return bitmap->Get(index);
}

bool BitmapHeap::Set(uint64_t index, bool value) {
    return bitmap->Set(index, value);
}

char** split(char* str, char* delimiters) {
    char* entry = str;
    int len = strlen(str);
    char** ReturnValue = (char**)malloc(0x200);
    memset(ReturnValue, 0, 0x200);
    int currentItemNumber = 0;
    int checkNum = 0;
    char* c = delimiters;
    int currentCharNumber = 0;
    int charNumberStart = 0;

    int lastCharEnd = 0;

    while (*str != 0) {
        ReturnValue[currentItemNumber] = (char*)malloc(0x1000);
        memset(ReturnValue[currentItemNumber], 0, 0x1000);
        currentCharNumber++;

        if (*str == *c) {
            checkNum++;
            c++;
        }
        else {
            c = delimiters;
            checkNum = 0;
        }

        if (checkNum >= strlen(delimiters)) {
            c = delimiters;

            charNumberStart = currentCharNumber - strlen(delimiters);

            if (charNumberStart < 0) {
                charNumberStart = 0;
            }
            int y = 0;
            for (int i = lastCharEnd; i < charNumberStart; i++) {
                ReturnValue[currentItemNumber][y++] = entry[i];
            }
            lastCharEnd = charNumberStart + strlen(delimiters);
            c = delimiters;

            currentItemNumber++;
            checkNum = 0;
        }
        str++;
    }

    ReturnValue[currentItemNumber] = (char*)malloc(0x1000);
    memset(ReturnValue[currentItemNumber], 0, 0x1000);
    int y = 0;
    for (int i = lastCharEnd; i < len; i++) {
        ReturnValue[currentItemNumber][y++] = entry[i];
    }

    return ReturnValue;
}

bool ReadBit(uint8_t byte, int position)
{
    return (byte >> position) & 0x1;
}

uint8_t WriteBit(uint8_t byte, int position, bool value)
{
    if (value) {
        byte |= 1 << position;
    }
    else {
        byte &= ~(1 << position);
    }

    return byte;
}


uint64_t Divide(uint64_t value, uint64_t divider) {
    uint64_t returnValue = value / divider;
    if ((value % divider) != 0) {
        returnValue++;
    }
    return returnValue;
}

bool strcmpKot(char* a, char* b) {
    int alen = strlen(a);
    int blen = strlen(b);
    if (alen == blen) {
        for (int i = 0; i < alen; i++) {
            if (a[i] != b[i]) {
                return false;
            }
        }
        return true;
    }
    else {
        return false;
    }
}