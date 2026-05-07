#include <stdio.h>
#include <SDL3/SDL.h>

#include "input_handler.h"
#include "qr_generator.h"
#include "gui.h"

#define WINDOW_SIZE 800
#define QUIET_ZONE 4

int main(int argc, char* argv[]) {
    UserInput userIn;
    if (parse_input(argc, argv, &userIn)) {
        return -1;
    }

    int size;
    uint8_t **qr_code = create_qr(&userIn, &size);
    if (!qr_code) {
        return -1;
    }

    int total_modules = size + QUIET_ZONE * 2;
    int module_size = WINDOW_SIZE / total_modules;

    GUI *gui = create_gui("QR Code", WINDOW_SIZE, WINDOW_SIZE);
    if (!gui) {
        return -1;
    }
    gui_render_qr(gui, qr_code, size, module_size);

    int run = 1;
    while (run) {
        gui_handle_input(&run);
    }

    destroy_gui(gui);
    return 0;
}