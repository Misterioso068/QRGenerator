#ifndef GUI_H
#define GUI_H

#include <stdint.h>
#include <SDL3/SDL.h>

typedef struct {
    SDL_Window   *window;
    SDL_Renderer *renderer;
} GUI;

GUI*  create_gui(const char *title, int width, int height);
void  destroy_gui(GUI *gui);
void handle_input(int *run);
void  render_qr(GUI *gui, uint8_t **grid, int size, int module_size);

#endif // GUI_H