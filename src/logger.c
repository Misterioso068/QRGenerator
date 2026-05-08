/**
 * @file logger.c
 * @brief Error logging and CLI help output utilities.
 *
 * This file provides formatted logging helpers used by the
 * command-line parser and QR generator application.
 *
 * Features include:
 * - Colored terminal error messages
 * - Usage/help menus
 * - Argument validation messages
 * - QR capacity error reporting
 *
 * ANSI escape color macros are defined in:
 * ANSI-color-codes.h
 */

#include <stdio.h>

#include "logger.h"

// ANSI-color-codes.h is public domain software from:
// https://gist.github.com/RabaDabaDoba/145049536f815903c79944599c6f952a (Unlicense)
#include "ANSI-color-codes.h"

/* ============================================================
 * Internal Helper Functions
 * ============================================================
 */

/**
 * @brief Prints the command-line argument reference table.
 *
 * Displays all supported CLI options and their accepted values.
 */
static void print_args_key(void) {
    fprintf(stderr, GRN "Arguments Key:\n" reset);

    fprintf(
        stderr,
        GRN "\t-M <encoding-type> : alphanumeric | numeric | byte | kanji | auto\n" reset
    );

    fprintf(
        stderr,
        GRN "\t-L <ecl_choice>    : L | M | Q | H\n" reset
    );

    fprintf(
        stderr,
        GRN "\t-D <data>          : data string to encode\n" reset
    );

    fprintf(
        stderr,
        GRN "\t-- \"<data>\"        : data string (use if data starts with '-')\n" reset
    );
}

/**
 * @brief Prints valid command-line usage examples.
 */
static void print_usage(void) {
    fprintf(stderr, YEL "Correct usage examples:\n" reset);

    fprintf(
        stderr,
        YEL "\t./build/qrgen -M <encoding-type> -L <ecl_choice> -D <data>\n" reset
    );

    fprintf(
        stderr,
        YEL "\t./build/qrgen -M <encoding-type> -L <ecl_choice> -- \"<data>\"\n" reset
    );
}

/**
 * @brief Prints the full help menu.
 *
 * Includes:
 * - Usage examples
 * - Argument descriptions
 */
static void print_help_menu(void) {
    print_usage();
    print_args_key();
}

/**
 * @brief Prints a formatted error message.
 *
 * @param msg Error message string.
 */
static void print_error(const char *msg) {
    fprintf(stderr, RED "ERROR: %s\n" reset, msg);
}

/**
 * @brief Prints an error message followed by the help menu.
 *
 * @param msg Error message string.
 */
static void print_error_with_help(const char *msg) {
    print_error(msg);
    print_help_menu();
}

/* ============================================================
 * Public Logger Functions
 * ============================================================
 */

/**
 * @brief Reports that no command-line arguments were provided.
 */
void arguments_not_provided(void) {
    print_error_with_help("No arguments provided");
}

/**
 * @brief Reports that too many arguments were provided.
 */
void too_many_arguments(void) {
    print_error_with_help("Too many arguments provided");
}

/**
 * @brief Reports that an option is missing its required value.
 *
 * Example:
 * @code
 * -M
 * @endcode
 * without a following encoding type.
 *
 * @param opt Option missing a value.
 */
void missing_option_value(const char *opt) {
    fprintf(
        stderr,
        RED "ERROR: Missing value for option '%s'\n" reset,
        opt
    );

    print_help_menu();
}

/**
 * @brief Reports that a required argument was not provided.
 *
 * @param opt Missing required argument.
 */
void missing_required_argument(const char *opt) {
    fprintf(
        stderr,
        RED "ERROR: Required argument '%s' not provided\n" reset,
        opt
    );

    print_help_menu();
}

/**
 * @brief Reports that a command-line option was specified multiple times.
 *
 * @param opt Duplicated option.
 */
void duplicate_argument(const char *opt) {
    fprintf(
        stderr,
        RED "ERROR: Option '%s' specified multiple times\n" reset,
        opt
    );

    print_help_menu();
}

/**
 * @brief Reports an invalid QR encoding mode.
 *
 * @param mode Invalid encoding mode string.
 */
void invalid_encoding_type_argument(const char *mode) {
    fprintf(
        stderr,
        RED "ERROR: Invalid encoding type '%s'\n" reset,
        mode
    );

    print_help_menu();
}

/**
 * @brief Reports an invalid QR error correction level.
 *
 * @param ecl Invalid error correction level string.
 */
void invalid_ecl_argument(const char *ecl) {
    fprintf(
        stderr,
        RED "ERROR: Invalid ECL type '%s'\n" reset,
        ecl
    );

    print_help_menu();
}

/**
 * @brief Reports an unknown command-line argument.
 *
 * @param input Unknown argument string.
 */
void unknown_argument(const char *input) {
    fprintf(
        stderr,
        RED "ERROR: Unknown argument '%s'\n" reset,
        input
    );

    print_help_menu();
}

/**
 * @brief Reports that the input data exceeds QR code capacity.
 *
 * Triggered when the selected encoding mode and error correction
 * level cannot store the provided amount of data.
 *
 * @param data_length Length of the provided input data.
 * @param max_capacity Maximum supported QR capacity.
 */
void data_too_long(int data_length, int max_capacity) {
    fprintf(
        stderr,
        RED
        "ERROR: Data length (%d) exceeds maximum QR code capacity (%d) "
        "for the selected encoding and ECL\n"
        reset,
        data_length,
        max_capacity
    );
}

/**
 * @brief Reports that no encoding type was provided.
 */
void encoding_type_not_provided() {
    fprintf(
        stderr,
        RED "ERROR: Encoding type was not provided." reset
    );
}