/** @file
  Implementation of the program's ErrorHandler.

  @author Adam Al-Hosam <aa429136@students.mimuw.edu.pl>
  @copyright Uniwersytet Warszawski
  @date 2021
*/

#include "error_handler.h"

/// Message about an unexpected error.
#define UNEXPECTED_ERROR_MESSAGE "UNEXPECTED ERROR CODE"

/// Upper bound for polynomial coefficient value.
#define COEFF_OR_AT_UPPER_BOUND 9223372036854775807

/// Lower bound for polynomial coefficient value.
#define COEFF_OR_AT_LOWER_BOUND (-COEFF_OR_AT_UPPER_BOUND-1LL)

/// Upper bound for the polynomials exponent value.
#define EXP_UPPER_BOUND 2147483647

/// Lower bound for the polynomials exponent value.
#define EXP_LOWER_BOUND 0

/// Upper bound for DEG_BY parameter.
#define DEG_BY_UPPER_BOUND 18446744073709551615U

/// Lower bound for DEG_BY parameter.
#define COMPOSE_UPPER_BOUND 18446744073709551615U

ErrorHandler NewErrorHandler(size_t w) {
    return (ErrorHandler) {.line_number = w, .code = NO_ERROR_CODE};
}

void ErrorHandlerSetCode(ErrorHandler *handler, int code) {
    handler->code = code;
}

bool HandleError(ErrorHandler handler) {
    char *ending;
    if (handler.code == NO_ERROR_CODE) {
        return false;
    }
    switch (handler.code) {
        case WRONG_COMMAND_CODE:
            ending = WRONG_COMMAND_MESSAGE;
            break;
        case DEG_BY_WRONG_VAR_CODE:
            ending = DEG_BY_WRONG_VAR_MESSAGE;
            break;
        case AT_WRONG_VAL_CODE:
            ending = AT_WRONG_VAL_MESSAGE;
            break;
        case STACK_UNDERFLOW_CODE:
            ending = STACK_UNDERFLOW_MESSAGE;
            break;
        case WRONG_POLY_CODE:
            ending = WRONG_POLY_MESSAGE;
            break;
        case COMPOSE_WRONG_PARAM_CODE:
            ending = COMPOSE_WRONG_PARAM_MESSAGE;
            break;
        case NO_MEMORY_CODE:
            fprintf(stderr, NO_MEMORY_MESSAGE);
            exit(1);
        default:
            fprintf(stderr, UNEXPECTED_ERROR_MESSAGE);
            exit(1);
    }
    fprintf(stderr, "ERROR %zu %s\n", handler.line_number, ending);
    return true;
}

bool IsError(ErrorHandler *e) {
    return e->code != NO_ERROR_CODE;
}

void HandleErrorCode(int code, size_t line_num) {
    ErrorHandler temp = NewErrorHandler(line_num);
    ErrorHandlerSetCode(&temp, code);
    HandleError(temp);
}

bool IsCoeffOrAtArgValid(long long int n) {
    return errno != 0 || n < COEFF_OR_AT_LOWER_BOUND
                      || n > COEFF_OR_AT_UPPER_BOUND
                      ? false : true;
}

bool IsExpValid(long int n) {
    return errno != 0 || n < EXP_LOWER_BOUND
                      || n > EXP_UPPER_BOUND
                      ? false : true;
}

bool IsDegByValid(long long unsigned int n) {
    return errno != 0 || n > DEG_BY_UPPER_BOUND ? false : true;
}

bool IsComposeValid(long long unsigned int n) {
    return errno != 0 || n > COMPOSE_UPPER_BOUND ? false : true;
}