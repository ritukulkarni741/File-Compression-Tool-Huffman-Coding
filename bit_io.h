
#ifndef BIT_IO_H
#define BIT_IO_H

#include <stdio.h>
#include <stdint.h>

typedef struct {
    FILE *file;
    unsigned char buffer;
    int bitCount;
    int error;
} BitWriter;

typedef struct {
    FILE *file;
    unsigned char buffer;
    int bitsRemaining;
    int error;
} BitReader;

void initBitWriter(BitWriter *writer, FILE *file);

int writeBit(BitWriter *writer, int bit);

int flushBitWriter(BitWriter *writer);

void initBitReader(BitReader *reader, FILE *file);

int readBit(BitReader *reader);

#endif
