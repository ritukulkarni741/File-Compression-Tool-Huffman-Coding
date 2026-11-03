
#include <stdio.h>
#include <string.h>

#include "file_handler.h"

void displayUsage(const char *programName)
{
    printf("\nHUFFMAN FILE COMPRESSION TOOL\n");

    printf("\nUsage:\n");

    printf("%s compress <input> <output>\n", programName);

    printf("%s decompress <input> <output>\n", programName);
}

int main(int argc, char *argv[])
{
    if (argc != 4) {
        displayUsage(argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "compress") == 0) {

        if (compressFile(argv[2], argv[3]))
            return 0;

        return 1;
    }

    else if (strcmp(argv[1], "decompress") == 0) {

        if (decompressFile(argv[2], argv[3]))
            return 0;

        return 1;
    }

    else {

        printf("Invalid command: %s\n", argv[1]);

        displayUsage(argv[0]);

        return 1;
    }
}
