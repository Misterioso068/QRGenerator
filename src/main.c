#include <stdio.h>
#include <SDL3/SDL.h>

#include "input_handler.h"
#include "qr_generator.h"
#include "gui.h"

int main(int argc, char* argv[]) {
    UserInput userIn;
    if (parse_input(argc, argv, &userIn)) {
        return -1;
    }

    int size;
    uint8_t **qr_code = create_qr(&userIn, &size);\

    int module_size = 10;
    GUI *gui = create_gui("QR Code", (size + 4 * 2) * module_size, (size + 4 * 2)* module_size);
    render_qr(gui, qr_code, size, 10);

    int run = 1;
    while (run) {
        handle_input(&run);
    }

    destroy_gui(gui);
    return 0;
}