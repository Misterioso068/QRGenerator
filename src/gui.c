#include <stdlib.h>
#include "gui.h"

GUI* create_gui(const char *title, int width, int height) {
    GUI *gui = malloc(sizeof(GUI));

    gui->window   = SDL_CreateWindow(title, width, height, SDL_WINDOW_RESIZABLE);
    gui->renderer = SDL_CreateRenderer(gui->window, NULL);

    return gui;
}

void destroy_gui(GUI *gui) {
    SDL_DestroyRenderer(gui->renderer);
    SDL_DestroyWindow(gui->window);
    free(gui);
}

void gui_handle_input(int *run) {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_EVENT_QUIT) {
            *run = 0;
            break;
        }
    }
}

void gui_render_qr(GUI *gui, uint8_t **grid, int size, int module_size) {
    int quiet_zone = 4;

    // clear to white (this already covers the quiet zone)
    SDL_SetRenderDrawColor(gui->renderer, 255, 255, 255, 255);
    SDL_RenderClear(gui->renderer);

    // draw modules offset by quiet zone
    SDL_SetRenderDrawColor(gui->renderer, 0, 0, 0, 255);
    for (int row = 0; row < size; row++) {
        for (int col = 0; col < size; col++) {
            if (grid[row][col] == 1) {
                SDL_FRect rect = {
                    (col + quiet_zone) * module_size,
                    (row + quiet_zone) * module_size,
                    module_size,
                    module_size
                };
                SDL_RenderFillRect(gui->renderer, &rect);
            }
        }
    }

    SDL_RenderPresent(gui->renderer);
}