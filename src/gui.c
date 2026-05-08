/**
 * @file gui.c
 * @brief SDL-based graphical rendering for QR codes.
 *
 * This file provides a lightweight GUI layer built on SDL3 for:
 * - Creating application windows
 * - Managing rendering contexts
 * - Handling window events
 * - Rendering QR code module grids
 *
 * The QR code is rendered as a grid of black and white squares
 * with a required quiet zone border.
 */

#include <stdlib.h>

#include "gui.h"

/**
 * @brief Creates and initializes a GUI context.
 *
 * Allocates a GUI structure and creates:
 * - An SDL window
 * - An SDL renderer
 *
 * @param title Window title string.
 * @param width Window width in pixels.
 * @param height Window height in pixels.
 *
 * @return GUI* Pointer to the initialized GUI structure.
 *
 * @note The returned GUI object must be freed using destroy_gui().
 */
GUI* create_gui(const char *title, int width, int height) {

    GUI *gui = malloc(sizeof(GUI));

    gui->window =
        SDL_CreateWindow(
            title,
            width,
            height,
            SDL_WINDOW_RESIZABLE
        );

    gui->renderer =
        SDL_CreateRenderer(gui->window, NULL);

    return gui;
}

/**
 * @brief Destroys a GUI context and releases SDL resources.
 *
 * Frees:
 * - SDL renderer
 * - SDL window
 * - GUI structure memory
 *
 * @param gui Pointer to the GUI structure to destroy.
 */
void destroy_gui(GUI *gui) {

    SDL_DestroyRenderer(gui->renderer);

    SDL_DestroyWindow(gui->window);

    free(gui);
}

/**
 * @brief Processes SDL window events.
 *
 * Currently handles:
 * - Window close events
 *
 * If a quit event is received, the run flag is cleared,
 * causing the main application loop to terminate.
 *
 * @param run Pointer to the application run-state flag.
 */
void gui_handle_input(int *run) {

    SDL_Event e;

    while (SDL_PollEvent(&e)) {

        if (e.type == SDL_EVENT_QUIT) {
            *run = 0;
            break;
        }
    }
}

/**
 * @brief Renders a QR code grid to the SDL window.
 *
 * The renderer:
 * - Clears the background to white
 * - Draws black QR modules
 * - Applies a quiet zone border around the QR code
 *
 * Each QR module is rendered as a filled rectangle.
 *
 * @param gui GUI rendering context.
 * @param grid 2D QR module matrix.
 * @param size Width/height of the QR matrix in modules.
 * @param module_size Pixel size of each QR module.
 */
void gui_render_qr(
    GUI *gui,
    uint8_t **grid,
    int size,
    int module_size
) {

    /*
     * QR codes require a quiet zone border for
     * reliable scanner detection.
     */
    int quiet_zone = 4;

    /*
     * Clear the renderer to white.
     * This also forms the quiet zone background.
     */
    SDL_SetRenderDrawColor(
        gui->renderer,
        255, 255, 255, 255
    );

    SDL_RenderClear(gui->renderer);

    /*
     * Draw QR modules in black.
     */
    SDL_SetRenderDrawColor(
        gui->renderer,
        0, 0, 0, 255
    );

    for (int row = 0; row < size; row++) {

        for (int col = 0; col < size; col++) {

            /*
             * Render only dark modules.
             */
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

    /*
     * Present the completed frame to the window.
     */
    SDL_RenderPresent(gui->renderer);
}