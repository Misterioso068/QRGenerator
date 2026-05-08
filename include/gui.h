/**
 * @file gui.h
 * @brief SDL3 window management and QR code rendering.
 */
 
#ifndef GUI_H
#define GUI_H
 
#include <stdint.h>
#include <SDL3/SDL.h>
 
/**
 * @brief Holds the SDL3 window and renderer context.
 */
typedef struct {
    SDL_Window   *window;    /**< SDL3 window handle. */
    SDL_Renderer *renderer;  /**< SDL3 2D renderer handle. */
} GUI;
 
/**
 * @brief Creates a GUI context with an SDL3 window and renderer.
 * @param title  Window title string.
 * @param width  Window width in pixels.
 * @param height Window height in pixels.
 * @return Pointer to a newly allocated GUI. Caller must call destroy_gui().
 */
GUI* create_gui(const char *title, int width, int height);
 
/**
 * @brief Destroys the GUI context and frees all associated resources.
 * @param gui Pointer to the GUI to destroy.
 */
void destroy_gui(GUI *gui);
 
/**
 * @brief Polls SDL3 events and sets *run to 0 on quit.
 * @param run Pointer to the main loop control flag.
 */
void gui_handle_input(int *run);
 
/**
 * @brief Renders a QR code grid to the SDL3 window.
 *
 * Clears to white, draws dark modules (grid == 1) as black rectangles,
 * and presents the frame. A 4-module quiet zone offset is applied.
 *
 * @param gui         GUI context.
 * @param grid        2D QR module array (1 = dark, 0 = light).
 * @param size        Grid width and height in modules.
 * @param module_size Pixel size of each module.
 */
void gui_render_qr(GUI *gui, uint8_t **grid, int size, int module_size);
 
#endif // GUI_H