#ifndef BMP_H
#define BMP_H
#include <stdint.h>

#define BITMAPFILEHEADERSIZE 14

#pragma pack(push, 1)

typedef struct {
    uint16_t type;              // Magic identifier: 0x4d42
    uint32_t size;              // File size in bytes
    uint16_t reserved1;         // Not used
    uint16_t reserved2;         // Not used
    uint32_t offset;
} BITMAPFILEHEADER;

#pragma pack(pop)
#endif