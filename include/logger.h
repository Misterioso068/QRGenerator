#ifndef LOGGER_H
#define LOGGER_H

void arguments_not_provided(void);
void missing_option_value(const char *opt);
void missing_required_argument(const char *opt);
void duplicate_argument(const char *opt);
void invalid_encoding_type_argument(const char *mode);
void invalid_ecl_argument(const char *ecl);
void unknown_argument(const char *input);


#endif // LOGGER_H