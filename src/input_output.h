/** @file
  Interface of input-output of multivariable polynomials.

  @author Adam Al-Hosam <aa429136@students.mimuw.edu.pl>
  @copyright Uniwersytet Warszawski
  @date 2021
*/

#ifndef INPUT_OUTPUT_H
#define INPUT_OUTPUT_H

#include "poly.h"
#include "error_handler.h"

/// newline char
#define NEWLINE '\n'

/// null char
#define NULL_CHAR '\0'

/// base of the system in which we read numbers
#define NUMBER_BASE 10

/// subtraction char
#define MINUS_SIGN '-'

/// string that should be printed when program needs to print boolean true.
#define TRUE_STRING "1\n"

/// string that should be printed when program needs to print boolean false.
#define FALSE_STRING "0\n"

/**
 * Prints a monomial to standard output.
 * @param m : monomial to print
 */
void MonoPrint(Mono *m);

/**
 * Prints a polynomial to standard output.
 * @param p : polynomial to print.
 */
void PolyPrint(Poly *p);

/**
 * @brief Reads a monomial from a string and returns it as a result.
 * @details First, the function reads a polynomial that is a coefficient and then
 * a number representing the exponent.
 * Function does in this order:
 * - checks if the handler didn't discover an error
 * - checks if the first char of monomial is a opening bracket
 * (if not then returns MonoDummy and registers an error)
 * - reads the polynomial from the next character
 * - if it discovers an error, it deletes the read polynomial and returns
 * MonoDummy
 * - checks if the read char is a separator (','). If it is not then returns
 * MonoDummy and registers an error.
 * - checks if the next read char is a digit. If it is not then returns
 * MonoDummy and registers an error.
 * - tries to read a number with strtol. If it is not valid then returns
 * MonoDummy and registers an error.
 * - checks if the next char is a closing bracket. If it is not then returns
 * MonoDummy and registers an error.
 * - finally it interprets the monomial - if it was a  monomial in the form
 * @f$0^x@f$, then returns MonoDummy, and ignores it afterwards.
 * @param string : beginning of a string containing monomial
 * @param last : pointer to a char array - it will  be set as a last char
 *               after reading the monomial (should be  ')')
 * @param handler : parameter of ErrorHandler type to handle errors
 * @return read monomial (if there were no errors) or (0,0) monomial.
 */
Mono MonoRead(char *string, char **last, ErrorHandler *handler);

/**
 * @brief Reads a polynomial from a string and returns it as a result.
 * @details Checks if the string is a number, if it is then it reads it and
 * returns a constant polynomial.
 * If the first char is an opening bracket then it creates a dynamic monomial
 * array and while it reads a plus sign after reading it will repeat it.
 * After it runs across a different char then plus sign, if it will not be
 * a separator (','), newline or a nullchar, or it will be a nullchar but
 * program doesn't detect eof, then it registers an error. Next it sets
 * last to an appropriate value and afterwards creates a new polynomial from
 * the monomial array.
 * If the string wasn't a number and didn't start with an opening bracket then
 * the program registers an error and returns zero polynomial.
 * @param string : beginning of a string containing polynomial
 * @param last : pointer to a char array - it will  be set as a last char
 *               after reading the polynomial (should be either ',',
 *              '\n', or if it is the last line, it can be'\0')
 * @param handler : parameter of ErrorHandler type to handle errors
 * @return read monomial (if there were no errors) or zero polynomial.
 */
Poly PolyRead(char *string, char **last, ErrorHandler *handler);

/**
 * Prints a logical value true  to standard output.
 */
static inline void PrintTrue() {
    printf(TRUE_STRING);
}

/**
 * Prints a logical value false to standard output.

 */
static inline void PrintFalse() {
    printf(FALSE_STRING);
}

#endif //INPUT_OUTPUT_H
