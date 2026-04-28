#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL3/SDL.h>

#include "input_handler.h"
#include "logger.h"
#include "qr_capacity.h"


int main(int argc, char* argv[]) {
    UserInput userIn;
    if (parse_input(argc, argv, &userIn)) {
        return -1;
    }

    printf("Enter String: ");
    char input[1024];
    scanf("%s", input);
    printf("Your input is: %s\n", input);

    return 0;
}