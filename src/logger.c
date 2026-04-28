#include <stdio.h>
#include "logger.h"
#include "ANSI-color-codes.h"

// Logger helpers
static void print_args_key(void) {
    fprintf(stderr, GRN "Arguments Key:\n");
    fprintf(stderr, GRN "\t<encoding-type>: alphanumeric | numeric | byte | kanji\n" reset);
    fprintf(stderr, GRN "\t<ecl_choice>: L | M | Q | H\n" reset);
}

static void print_usage(void) {
    fprintf(stderr, YEL "Correct usage examples: \n\t./build/main -M <encoding-type> -L <ecl_choice>\n\t./build/main -L <ecl_choice> -M <encoding-type>\n" reset);
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
    print_usage();
    print_args_key();
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