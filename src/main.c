#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL3/SDL.h>
#include "ANCI-color-codes.h"

#include "qr_capacity.h"

/*
    Encoding types: Alphanumeric, Numeric, Byte, Kanji
    Error Correction Levels (ECL's): L (7%), M (15%), Q (25%), and H (30%)
*/

int main(int argc, char* argv[]) {
    argc--; // Remove 1 for unwanted ./main call.

    if (argc <= 0) {
        perror()
    }

    printf("%d\n", argc);

    char input[1024];
    scanf("%s", input);
}