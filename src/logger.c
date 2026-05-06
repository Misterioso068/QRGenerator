#include <stdio.h>
#include "logger.h"
#include "ANSI-color-codes.h"

// Logger helpers
static void print_args_key(void) {
    fprintf(stderr, GRN "Arguments Key:\n" reset);
    fprintf(stderr, GRN "\t-M <encoding-type> : alphanumeric | numeric | byte | kanji\n" reset);
    fprintf(stderr, GRN "\t-L <ecl_choice>    : L | M | Q | H\n" reset);
    fprintf(stderr, GRN "\t-D <data>          : data string to encode\n" reset);
    fprintf(stderr, GRN "\t-- \"<data>\"        : data string (use if data starts with '-')\n" reset);
}

static void print_usage(void) {
    fprintf(stderr, YEL "Correct usage examples:\n" reset);
    fprintf(stderr, YEL "\t./build/qrgen -M <encoding-type> -L <ecl_choice> -D <data>\n" reset);
    fprintf(stderr, YEL "\t./build/qrgen -M <encoding-type> -L <ecl_choice> -- \"<data>\"\n" reset);
}

static void print_help_menu(void) {
    print_usage();
    print_args_key();
}

static void print_error(const char *msg) {
    fprintf(stderr, RED "ERROR: %s\n" reset, msg);
}

static void print_error_with_help(const char *msg) {
    print_error(msg);
    print_help_menu();
}

// Public logger functions
void arguments_not_provided(void) {
    print_error_with_help("No arguments provided");
}

void too_many_arguments(void) {
    print_error_with_help("Too many arguments provided");
}

void missing_option_value(const char *opt) {
    fprintf(stderr, RED "ERROR: Missing value for option '%s'\n" reset, opt);
    print_help_menu();
}

void missing_required_argument(const char *opt) {
    fprintf(stderr, RED "ERROR: Required argument '%s' not provided\n" reset, opt);
    print_help_menu();
}

void duplicate_argument(const char *opt) {
    fprintf(stderr, RED "ERROR: Option '%s' specified multiple times\n" reset, opt);
    print_help_menu();
}

void invalid_encoding_type_argument(const char *mode) {
    fprintf(stderr, RED "ERROR: Invalid encoding type '%s'\n" reset, mode);
    print_help_menu();
}

void invalid_ecl_argument(const char *ecl) {
    fprintf(stderr, RED "ERROR: Invalid ECL type '%s'\n" reset, ecl);
    print_help_menu();
}

void unknown_argument(const char *input) {
    fprintf(stderr, RED "ERROR: Unknown argument '%s'\n" reset, input);
    print_help_menu();
}

void data_too_long(int data_length, int max_capacity) {
    fprintf(stderr, RED "ERROR: Data length (%d) exceeds maximum QR code capacity (%d) for the selected encoding and ECL\n" reset, data_length, max_capacity);
}

void encoding_type_not_provided() {
    fprintf(stderr, RED "ERROR: Encoding type was not provided.");
}