
#include "bit_io.h"

void initBitWriter(BitWriter *writer, FILE *file)
{
    writer->file = file;
    writer->buffer = 0;
    writer->bitCount = 0;
    writer->error = 0;
}

int writeBit(BitWriter *writer, int bit)
{
    if (writer == NULL || writer->file == NULL || writer->error)
        return 0;

    writer->buffer = (writer->buffer << 1) | (bit & 1);

    writer->bitCount++;

    if (writer->bitCount == 8) {

        if (fputc(writer->buffer, writer->file) == EOF) {
            writer->error = 1;
            return 0;
        }

        writer->buffer = 0;
        writer->bitCount = 0;
    }

    return 1;
}

int flushBitWriter(BitWriter *writer)
{
    if (writer == NULL || writer->file == NULL || writer->error)
        return 0;

    if (writer->bitCount > 0) {

        writer->buffer <<= (8 - writer->bitCount);

        if (fputc(writer->buffer, writer->file) == EOF) {
            writer->error = 1;
            return 0;
        }

        writer->buffer = 0;
        writer->bitCount = 0;
    }

    return 1;
}

void initBitReader(BitReader *reader, FILE *file)
{
    reader->file = file;
    reader->buffer = 0;
    reader->bitsRemaining = 0;
    reader->error = 0;
}

int readBit(BitReader *reader)
{
    if (reader == NULL || reader->file == NULL || reader->error)
        return -1;

    if (reader->bitsRemaining == 0) {

        int byte = fgetc(reader->file);

        if (byte == EOF) {
            if (ferror(reader->file))
                reader->error = 1;

            return -1;
        }

        reader->buffer = (unsigned char)byte;
        reader->bitsRemaining = 8;
    }

    int bit = (reader->buffer >> (reader->bitsRemaining - 1)) & 1;

    reader->bitsRemaining--;

    return bit;
}
