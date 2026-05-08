/**
 * @file logger.h
 * @brief Error and usage message logging for the QR code generator.
 *
 * All functions print formatted error messages to stderr with ANSI
 * color codes. Help menu is included where appropriate.
 */
 
#ifndef LOGGER_H
#define LOGGER_H
 
/** @brief Logs an error when no arguments are provided. */
void arguments_not_provided(void);
 
/**
 * @brief Logs an error when an option is missing its value.
 * @param opt The option flag missing a value (e.g. "-M").
 */
void missing_option_value(const char *opt);
 
/**
 * @brief Logs an error when a required argument is not provided.
 * @param opt The required option that was omitted.
 */
void missing_required_argument(const char *opt);
 
/**
 * @brief Logs an error when an argument is specified more than once.
 * @param opt The duplicated option flag.
 */
void duplicate_argument(const char *opt);
 
/**
 * @brief Logs an error for an unrecognized encoding type.
 * @param mode The invalid encoding type string.
 */
void invalid_encoding_type_argument(const char *mode);
 
/**
 * @brief Logs an error for an unrecognized ECL value.
 * @param ecl The invalid ECL string.
 */
void invalid_ecl_argument(const char *ecl);
 
/**
 * @brief Logs an error for an unrecognized command-line argument.
 * @param input The unrecognized argument string.
 */
void unknown_argument(const char *input);
 
/**
 * @brief Logs an error when the data exceeds the maximum QR code capacity.
 * @param data_length  Length of the provided data.
 * @param max_capacity Maximum capacity for the selected encoding and ECL.
 */
void data_too_long(int data_length, int max_capacity);
 
/** @brief Logs an error when no encoding type is set on the UserInput struct. */
void encoding_type_not_provided(void);
 
#endif // LOGGER_H