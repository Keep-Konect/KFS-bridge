#pragma once
#include <cstdint>
#include <fstream>
#pragma warning(disable : 4996)

extern std::fstream MainFile;
extern char* filePath;

uint64_t Divide(uint64_t value, uint64_t divider);
char** split(char* str, char* delimiters);

class Bitmap {
public:
    uint64_t Size;
    uint8_t* Buffer;
    bool operator[](uint64_t index);
    bool Get(uint64_t index);
    bool Set(uint64_t index, bool value);
};

class BitmapHeap {
private:
    Bitmap* bitmap;
public:
    BitmapHeap(uint64_t size);
    void Free();
    bool operator[](uint64_t index);
    bool Get(uint64_t index);
    bool Set(uint64_t index, bool value);
};

#define PACK( __Declaration__ ) __pragma( pack(push, 1) ) __Declaration__ __pragma( pack(pop))

PACK(struct Time {
    uint8_t seconds;
    uint8_t minutes;
    uint8_t hours;
    uint8_t days;
    uint8_t months;
    uint64_t years;
});

PACK(struct GUID {
    uint32_t Data1;
    uint16_t Data2;
    uint16_t Data3;
    uint64_t Data4;
});

#define uint8_Limit 255
#define uint16_Limit 65535
#define uint32_Limit 4294967295
#define uint64_Limit 18446744073709551615

#define MaxParitionsNumber 128
#define SizeofLBASector 512

bool ReadBit(uint8_t byte, int position);
uint8_t WriteBit(uint8_t byte, int position, bool value);
uint64_t Divide(uint64_t value, uint64_t divider);
bool strcmpKot(char* a, char* b);