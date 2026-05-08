/**
 * @file main.c
 * @brief Entry point for the QR code generator application.
 *
 * This file handles:
 * - Parsing command-line input
 * - Generating the QR code matrix
 * - Creating the SDL GUI window
 * - Rendering the QR code
 * - Running the GUI event loop
 */
#include "input_handler.h"
#include "qr_generator.h"
#include "gui.h"

/**
 * @def WINDOW_SIZE
 * @brief Width and height of the application window in pixels.
 */
#define WINDOW_SIZE 800

/**
 * @def QUIET_ZONE
 * @brief Number of white modules surrounding the QR code.
 *
 * QR codes require a quiet zone border to improve scanner reliability.
 */
#define QUIET_ZONE 4

/**
 * @brief Program entry point.
 *
 * Parses user input, generates a QR code matrix, initializes the GUI,
 * renders the QR code, and runs the application event loop until the
 * window is closed.
 *
 * @param argc Number of command-line arguments.
 * @param argv Array of command-line argument strings.
 *
 * @return int
 * @retval 0 Program completed successfully.
 * @retval -1 An error occurred during input parsing, QR generation,
 *             or GUI initialization.
 */
int main(int argc, char* argv[]) {
    UserInput userIn;

    /* Parse command-line arguments into the user input structure. */
    if (parse_input(argc, argv, &userIn)) {
        return -1;
    }

    int size;

    /* Generate the QR code matrix. */
    uint8_t **qr_code = create_qr(&userIn, &size);
    if (!qr_code) {
        return -1;
    }

    /*
     * Total number of modules including the required quiet zone border.
     */
    int total_modules = size + QUIET_ZONE * 2;

    /* Calculate pixel size for each QR module. */
    int module_size = WINDOW_SIZE / total_modules;

    /* Create the application GUI window. */
    GUI *gui = create_gui("QR Code", WINDOW_SIZE, WINDOW_SIZE);
    if (!gui) {
        return -1;
    }

    /* Render the generated QR code to the window. */
    gui_render_qr(gui, qr_code, size, module_size);

    /* Main application loop. */
    int run = 1;
    while (run) {
        gui_handle_input(&run);
    }

    /* Clean up GUI resources before exiting. */
    destroy_gui(gui);

    return 0;
}