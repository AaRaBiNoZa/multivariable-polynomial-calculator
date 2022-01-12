/** @file
  Interface of the program's ErrorHandler.

  @authors Adam Al-Hosam <aa429136@students.mimuw.edu.pl>
  @copyright Uniwersytet Warszawski
  @date 2021
*/

#ifndef ERRORHANDLER_H
#define ERRORHANDLER_H

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

/// Macro for checking pointers after malloc/realloc
#define CHECK_PTR(p)  \
  do {                \
    if (p == NULL) {  \
      exit(1);        \
    }                 \
  } while (0)

/// Value of an error code that signals that there are no errors.
#define NO_ERROR_CODE (-1)

/// Message about a wrong command.
#define WRONG_COMMAND_MESSAGE "WRONG COMMAND"

/// Error code of a wrong command.
#define WRONG_COMMAND_CODE 0

/// Message about a not valid parameter of DEG_BY.
#define DEG_BY_WRONG_VAR_MESSAGE "DEG BY WRONG VARIABLE"

/// Error code of a not valid parameter of DEG_BY.
#define DEG_BY_WRONG_VAR_CODE 1

/// Message about a not valid parameter of AT.
#define AT_WRONG_VAL_MESSAGE "AT WRONG VALUE"

/// Error code of a not valid parameter of AT.
#define AT_WRONG_VAL_CODE 2

/// Message about the stack having not enough polynomials.
#define STACK_UNDERFLOW_MESSAGE "STACK UNDERFLOW"

/// Error code of a stack underflow.
#define STACK_UNDERFLOW_CODE 3

/// Message about an error during parsing of the polynomial.
#define WRONG_POLY_MESSAGE "WRONG POLY"

/// Error code of a not valid polynomial.
#define WRONG_POLY_CODE 4

/// Error code when there is no memory.
#define NO_MEMORY_CODE 5

/// Message about a no memory error.
#define NO_MEMORY_MESSAGE "NO MEMORY"

/// Error code of a not valid parameter of COMPOSE.
#define COMPOSE_WRONG_PARAM_CODE 6

/// Message about a not valid parameter of COMPOSE.
#define COMPOSE_WRONG_PARAM_MESSAGE "COMPOSE WRONG PARAMETER"

/**
 * Struct storing information if there is any error in the program.
 */
typedef struct ErrorHandler {
    size_t line_number; ///< current line number
    int code; ///< error code
} ErrorHandler;

/**
 * Creating a new ErrorHandler with a line number set.
 * @param w : current line number to set.
 * @return : ErrorHandler with a set line number.
 */
ErrorHandler NewErrorHandler(size_t w);

/**
 * Setting an error code in the ErrorHandler.
 * @param handler : handler for which we should change the error code.
 * @param code : error code to set.
 */
void ErrorHandlerSetCode(ErrorHandler *handler, int code);

/**
 * @brief Handling an error with a given code; printing appropriate information.

 * @details If there was no error, the function returns false, else the switch
 * instruction matches error code to a message and then prints the message
 * to stderr.
 * @param handler : handler which stores info about an error
 * @return true - if there was an error, else false
 */
bool HandleError(ErrorHandler handler);

/**
 * @brief Informs if a current handler signals any errors.
 * @details Aside from checking if a value is within a valid range, it checks
 * the value of errno, because the number could overflow while being read.
 * Because of checking errno, it's very important to call this function
 * as fast as possible after reading the number. (so errno cannot
 * change because of some other reason).
 * @param e : handler to check
 * @return does handler signal an error
 */
bool IsError(ErrorHandler *e);

/**
 * @brief Performs actions required to handle an error with a given code
 * @details Creates a new ErrorHandler, sets its error code and then
 * executes it.
 * @param code : error code
 * @param line_num : current line number
 */
void HandleErrorCode(int code, size_t line_num);

/**
 * @brief Checks if the value of a polynomial coeff or AT parameter is valid.
 * @details Aside from checking if a value is within a valid range, it checks
 * the value of errno, because the number could overflow while being read.
 * Because of checking errno, it's very important to call this function
 * as fast as possible after reading the number. (so errno cannot
 * change because of some other reason).
 * @param n : value to check
 * @return is the value valid
 */
bool IsCoeffOrAtArgValid(long long int n);

/**
 * @brief Checks if the value of the exponent is valid.
 * @details Aside from checking if a value is within a valid range, it checks
 * the value of errno, because the number could overflow while being read.
 * Because of checking errno, it's very important to call this function
 * as fast as possible after reading the number. (so errno cannot
 * change because of some other reason).
 * @param n : value to check
 * @return is the value valid
 */
bool IsExpValid(long int n);

/**
 * @brief Checks if the value of the DEG_BY parameter is valid.
 * @details Aside from checking if a value is within a valid range, it checks
 * the value of errno, because the number could overflow while being read.
 * Because of checking errno, it's very important to call this function
 * as fast as possible after reading the number. (so errno cannot
 * change because of some other reason).
 * @param n : value to check
 * @return is the value valid
 */
bool IsDegByValid(long long unsigned int n);

/**
 * Checks if the value of the CONPOSE parameter is valid
 * @param n : value to check
 * @return is the value valid
 */
bool IsComposeValid(long long unsigned int n);

#endif //ERRORHANDLER_H
