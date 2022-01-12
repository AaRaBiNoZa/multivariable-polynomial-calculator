/** @file
  Implementation of multivariable polynomial calculator.

  @authors Adam Al-Hosam <aa429136@students.mimuw.edu.pl>
  @copyright Uniwersytet Warszawski
  @date 2021
*/

#ifndef _POSIX_C_SOURCE
/// Directive necessary for getline to work.
#define _POSIX_C_SOURCE 200809L
#endif

#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "stack.h"
#include "input_output.h"
#include "mono_array.h"

/// String representing ZERO command.
#define ZERO_STRING "ZERO\0"

/// String representing IS_COEFF command.
#define IS_COEFF_STRING "IS_COEFF\0"

/// String representing IS_ZERO command.
#define IS_ZERO_STRING "IS_ZERO\0"

/// String representing CLONE command.
#define CLONE_STRING "CLONE\0"

/// String representing ADD command.
#define ADD_STRING "ADD\0"

/// String representing MUL command.
#define MUL_STRING "MUL\0"

/// String representing NEG command.
#define NEG_STRING "NEG\0"

/// String representing SUB command.
#define SUB_STRING "SUB\0"

/// String representing IS_EQ command.
#define IS_EQ_STRING "IS_EQ\0"

/// String representing DEG command.
#define DEG_STRING "DEG\0"

/// String representing PRINT command.
#define PRINT_STRING "PRINT\0"

/// String representing POP command.
#define POP_STRING "POP\0"

/// String representing DEG_BY command.
#define DEG_BY_STRING "DEG_BY"

/// String representing DEG_BY command with a space.
#define DEG_BY_WITH_SPACE_STRING "DEG_BY "

/// Length of DEG_BY command.
#define DEG_BY_LEN 6

/// String representing AT command.
#define AT_STRING "AT"

/// String representing AT command with a space.
#define AT_WITH_SPACE_STRING "AT "

/// Length of AT command.
#define AT_LEN 2

/// String representing COMPOSE command with a space.
#define COMPOSE_STRING "COMPOSE"

/// String representing COMPOSE command with a space.
#define COMPOSE_WITH_SPACE_STRING "COMPOSE "

/// Length of COMPOSE command.
#define COMPOSE_LEN 7

/// Char distinguishing a comment.
#define COMMENT_CHAR '#'

/// Getline error code.
#define GETLINE_ERROR (-1)

/**
* Function that determines if an input string matches a given command.
* Checks if two strings (char arrays) are the same
* up to the last char - it expects that if one string ends, the other will
* still contain a newline char, but does enable both strings to be exactly
* the same.
* When global variable feof is true, it means that reading input is done and
* the current command is the last one.
* @param instr : instrukcja
* @param to_cmp : wczytany napis
* @return czy napis reprezentuje daną instrukcję
*/
static bool InstrCmp(const char *instr, const char *to_cmp) {
  assert(instr != NULL && to_cmp != NULL);

  int index = 0;
  while (instr[index] != NULL_CHAR) {
    if (instr[index] != to_cmp[index]) {
      return false;
    }
    index++;
  }

  if (instr[index] == NULL_CHAR && to_cmp[index] == NULL_CHAR) {
    if (!feof(stdin)) {
      return false;
    }
    return true;
  } else if (instr[index] == NULL_CHAR && to_cmp[index] == NEWLINE
      && to_cmp[index + 1] == NULL_CHAR) {
    return true;
  } else {
    return false;
  }
}

/**
 * ZERO command in calc. It pushes a zero polynomial to the stack.
 * @param s : stack
 */
static void CalcZero(Tstack *s) {
  Push(s, PolyZero());
}

/**
 * Checks if a given polynomial is constant.
 * If it is, prints 1 to standard output, else it prints 0.
 * @param poly : polynomial to check
 */
static void CalcIsCoeff(Poly *poly) {
  if (PolyIsCoeff(poly)) {
    PrintTrue();
  } else {
    PrintFalse();
  }
}

/**
 * Checks if a given polynomial is zero polynomial.
 * If it is, prints 1 to standard output, else it prints 0.
 * @param poly : polynomial to check
 */
static void CalcIsZero(Poly *poly) {
  if (PolyIsZero(poly)) {
    PrintTrue();
  } else {
    PrintFalse();
  }
}

/**
 * Returns deep copy of a given polynomial. Works exactly like
 * PolyClone, but I have created a separate function because of the logical
 * structure of the program.
 * @param poly : polynomial to copy
 * @return : deep copy of a given polynomial
 */
static Poly CalcClone(Poly *poly) {
  return PolyClone(poly);
}

/**
 * Adds two polynomials and then destroys them.
 * @param first : polynomial @f$p@f$
 * @param second : polynomial @f$q@f$
 * @return polynomial @f$p+q@f$
 */
static Poly CalcAdd(Poly *first, Poly *second) {
  Poly result = PolyAdd(first, second);
  PolyDestroy(first);
  PolyDestroy(second);
  return result;
}

/**
 * Multiplies two polynomials and then destroys them.
 * @param first : polynomial @f$p@f$
 * @param second : polynomial @f$q@f$
 * @return polynomial @f$p\cdotq@f$
 */
static Poly CalcMul(Poly *first, Poly *second) {
  Poly result = PolyMul(first, second);
  PolyDestroy(first);
  PolyDestroy(second);
  return result;
}

/**
 * Negates a given polynomial.
 * Creates a negated polynomial and destroys the original, saves the negated
 * one in the address of the original.
 * @param poly : polynomial to negate
 */
static void CalcNeg(Poly *poly) {
  Poly result = PolyNeg(poly);
  PolyDestroy(poly);
  *poly = result;
}

/**
 * Subtracts two polynomials and then it destroys them.
 * @param first : polynomial @f$p@f$
 * @param second : polynomial @f$q@f$
 * @return polynomial @f$p-q@f$
 */
static Poly CalcSub(Poly *first, Poly *second) {
  Poly result = PolySub(first, second);
  PolyDestroy(first);
  PolyDestroy(second);

  return result;
}

/**
 * Checks if two polynomials are the same.
 * If they are, prints 1 to standard output, else it prints 0.
 * @param first : first polynomial to compare
 * @param second : second polynomial to compare
 */
static void CalcIsEq(Poly *first, Poly *second) {
  if (PolyIsEq(first, second)) {
    PrintTrue();
  } else {
    PrintFalse();
  }
}

/**
 * Prints a result of command PolyDeg to standard output.
 * @param poly : polynomial to perform PolyDeg on.
 */
static void CalcDeg(Poly *poly) {
  printf("%d\n", PolyDeg(poly));
}

/**
 * Prints a result of command PolyDegBy to standard output.
 * @param poly : polynomial to perform PolyDegBy on.
 * @param var_idx : parameter of the command.
 */
static void CalcDegBy(Poly *poly, size_t var_idx) {
  printf("%d\n", PolyDegBy(poly, var_idx));
}

/**
 * Computes the result of PolyAt for a polynomial and a given value and then
 * destroys it, and saves the result in the original polynomial's address.
 * @param poly : polynomial to perform the command on.
 * @param x : command parameter.
 */
static void CalcAt(Poly *poly, poly_coeff_t x) {
  Poly result = PolyAt(poly, x);
  PolyDestroy(poly);
  *poly = result;
}

/**
 * Prints the polynomial to standard output.
 * @param poly : polynomial to print.
 */
static void CalcPrint(Poly *poly) {//n
  PolyPrint(poly);
  printf("\n");
}

/**
 * Function that takes care of operations which do not take polynomials
 * from the stack.
 * @param s : stos
 * @param instruction : instruction
 */
static void NullaryOperation(Tstack *s, char *instruction) {
  if (InstrCmp(ZERO_STRING, instruction)) {
    CalcZero(s);
  }
}

/**
 * Function that takes care of operations which take exactly one polynomial
 * from the stack. First it takes a parameter and then tries to determine
 * which operation it refers to and then does appropriate actions.
 * After doing that it returns the polynomial back to the stack.
 * (the exception is POP command).
 * Function requires that the string representing an instruction be a
 * VALID instruction.
 * @param s : stack
 * @param instruction : instruction
 * @param line_num : line number
 */
static void UnaryOperation(Tstack *s, char *instruction, size_t line_num) {
  if (StackIsEmpty(s)) {
    HandleErrorCode(STACK_UNDERFLOW_CODE, line_num);
  } else {
    Poly top = Pop(s);
    if (InstrCmp(IS_COEFF_STRING, instruction)) {
      CalcIsCoeff(&top);
    } else if (InstrCmp(IS_ZERO_STRING, instruction)) {
      CalcIsZero(&top);
    } else if (InstrCmp(CLONE_STRING, instruction)) {
      Poly clone = CalcClone(&top);
      Push(s, clone);
    } else if (InstrCmp(NEG_STRING, instruction)) {
      CalcNeg(&top);
    } else if (InstrCmp(DEG_STRING, instruction)) {
      CalcDeg(&top);
    } else if (InstrCmp(PRINT_STRING, instruction)) {
      CalcPrint(&top);
    }
    Push(s, top);
    if (InstrCmp(POP_STRING, instruction)) {
      Poly to_destroy = Pop(s);
      PolyDestroy(&to_destroy);
    }
  }
}

/**
 * Function that takes care of operations which take exactly two polynomials
 * from the stack. First it takes them from the stack and then tries to
 * determine which operation it refers to and then does appropriate actions.
 * After doing that it returns the result (or polynomials) back to the stack.
 * Function requires that the string representing an instruction be a
 * VALID instruction.
 * @param s : stack
 * @param instruction : instruction
 * @param line_num : line number
 */
static void BinaryOperation(Tstack *s, char *instruction, size_t line_num) {
  if (!StackDoesHaveAtLeastTwoElements(s)) {
    HandleErrorCode(STACK_UNDERFLOW_CODE, line_num);
  } else {
    Poly first = Pop(s);
    Poly second = Pop(s);
    if (InstrCmp(ADD_STRING, instruction)) {
      Push(s, CalcAdd(&first, &second));
    } else if (InstrCmp(MUL_STRING, instruction)) {
      Push(s, CalcMul(&first, &second));
    } else if (InstrCmp(SUB_STRING, instruction)) {
      Push(s, CalcSub(&first, &second));
    } else if (InstrCmp(IS_EQ_STRING, instruction)) {
      CalcIsEq(&first, &second);
      Push(s, second);
      Push(s, first);
    }
  }
}

/**
 * Function that takes care of operations which take exactly one polynomial
 * and a parameter. First it checks if any of the known commands match. Next
 * it checks if there is exactly one space after a command. After that
 * it converts the number and checks if the last read character is '\n' or '\0'
 * (the second one only if it detects end of file). If the last char is not
 * valid, then it sends an appropriate signal to ErrorHandler, if it is valid,
 * then it checks if a stack underflow doesn't occur. In the end if everything
 * was successful it performs a given operation.
 * This function requires that the beginning of the string (instruction
 * without a parameter) was matching to any of the known commands.
 * @param s : stack
 * @param instruction : name of the instruction
 * @param line_num : line number
 */
static void ParametricUnaryOperation(Tstack *s, char *instruction,
                                     size_t line_num) {

  Poly top;
  char *last;

  if (strncmp(instruction, DEG_BY_STRING, DEG_BY_LEN) == 0) {
    if (strncmp(instruction, DEG_BY_WITH_SPACE_STRING, DEG_BY_LEN + 1) == 0
        && isdigit(instruction[DEG_BY_LEN + 1])) {
      errno = 0;
      size_t var_idx = strtoull(&instruction[DEG_BY_LEN + 1], &last,
                                NUMBER_BASE);

      if ((*last != NEWLINE && !(feof(stdin) && *last == NULL_CHAR)) ||
          !IsDegByValid(var_idx)) {
        HandleErrorCode(DEG_BY_WRONG_VAR_CODE, line_num);
      } else if (StackIsEmpty(s)) {
        HandleErrorCode(STACK_UNDERFLOW_CODE, line_num);
      } else {
        top = Pop(s);
        CalcDegBy(&top, var_idx);
        Push(s, top);
      }
    } else {
      if (!isspace(instruction[DEG_BY_LEN])) {
        HandleErrorCode(WRONG_COMMAND_CODE, line_num);
      } else {
        HandleErrorCode(DEG_BY_WRONG_VAR_CODE, line_num);
      }
    }
  } else if (strncmp(instruction, AT_STRING, AT_LEN) == 0) {
    if (strncmp(instruction, AT_WITH_SPACE_STRING, AT_LEN + 1) == 0
        && (isdigit(instruction[AT_LEN + 1]) ||
            instruction[AT_LEN + 1] == MINUS_SIGN)) {

      errno = 0;
      poly_coeff_t coeff = strtol(&instruction[AT_LEN + 1], &last,
                                  NUMBER_BASE);

      if ((*last != NEWLINE && !(feof(stdin) && *last == NULL_CHAR))
          || !IsCoeffOrAtArgValid(coeff)) {
        HandleErrorCode(AT_WRONG_VAL_CODE, line_num);
      } else if (StackIsEmpty(s)) {
        HandleErrorCode(STACK_UNDERFLOW_CODE, line_num);
      } else {
        top = Pop(s);
        CalcAt(&top, coeff);
        Push(s, top);
      }
    } else {
      if (!isspace(instruction[AT_LEN])) {
        HandleErrorCode(WRONG_COMMAND_CODE, line_num);
      } else {
        HandleErrorCode(AT_WRONG_VAL_CODE, line_num);
      }
    }
  } else if (strncmp(instruction, COMPOSE_STRING, COMPOSE_LEN) == 0) {
    if (strncmp(instruction, COMPOSE_WITH_SPACE_STRING, COMPOSE_LEN + 1) == 0
        && isdigit(instruction[COMPOSE_LEN + 1])) {
      errno = 0;
      size_t count = strtoull(&instruction[COMPOSE_LEN + 1], &last,
                              NUMBER_BASE);

      if ((*last != NEWLINE && !(feof(stdin) && *last == NULL_CHAR)) ||
          !IsComposeValid(count)) {
        HandleErrorCode(COMPOSE_WRONG_PARAM_CODE, line_num);
      } else if (StackSize(s) - 1 < count) {
        HandleErrorCode(STACK_UNDERFLOW_CODE, line_num);
      } else {
        Poly *arr = malloc(count * sizeof(Poly));
        CHECK_PTR(arr);

        Poly main_to_compose = Pop(s);
        for (size_t i = count; i > 0; i--) {
          arr[i - 1] = Pop(s);
        }

        Push(s, PolyCompose(&main_to_compose, count, arr));

        for (size_t i = 0; i < count; i++) {
          PolyDestroy(&arr[i]);
        }
        PolyDestroy(&main_to_compose);
        free(arr);
      }
    } else {
      if (!isspace(instruction[COMPOSE_LEN])) {
        HandleErrorCode(WRONG_COMMAND_CODE, line_num);
      } else {
        HandleErrorCode(COMPOSE_WRONG_PARAM_CODE, line_num);
      }
    }
  }

}

/**
 * A big if tree recognizing a string and redirecting to an appropriate
 * helper function. If it isn't able to match the input to any of the
 * known commands, it sends an appropriate message do ErrorHandler.
 * @param s : stack
 * @param instruction : string representing an instruction
 * @param line_num : line number
 */
static void CalcInterpretOperation(Tstack *s, char *instruction,
                                   size_t line_num) {
  if (InstrCmp(ZERO_STRING, instruction)) {
    NullaryOperation(s, instruction);
  } else if (InstrCmp(IS_COEFF_STRING, instruction)) {
    UnaryOperation(s, instruction, line_num);
  } else if (InstrCmp(IS_ZERO_STRING, instruction)) {
    UnaryOperation(s, instruction, line_num);
  } else if (InstrCmp(CLONE_STRING, instruction)) {
    UnaryOperation(s, instruction, line_num);
  } else if (InstrCmp(ADD_STRING, instruction)) {
    BinaryOperation(s, instruction, line_num);
  } else if (InstrCmp(MUL_STRING, instruction)) {
    BinaryOperation(s, instruction, line_num);
  } else if (InstrCmp(NEG_STRING, instruction)) {
    UnaryOperation(s, instruction, line_num);
  } else if (InstrCmp(SUB_STRING, instruction)) {
    BinaryOperation(s, instruction, line_num);
  } else if (InstrCmp(IS_EQ_STRING, instruction)) {
    BinaryOperation(s, instruction, line_num);
  } else if (InstrCmp(DEG_STRING, instruction)) {
    UnaryOperation(s, instruction, line_num);
  } else if (InstrCmp(PRINT_STRING, instruction)) {
    UnaryOperation(s, instruction, line_num);
  } else if (InstrCmp(POP_STRING, instruction)) {
    UnaryOperation(s, instruction, line_num);
  } else if (strncmp(instruction, DEG_BY_STRING, DEG_BY_LEN) == 0) {
    ParametricUnaryOperation(s, instruction, line_num);
  } else if (strncmp(instruction, AT_STRING, AT_LEN) == 0) {
    ParametricUnaryOperation(s, instruction, line_num);
  } else if (strncmp(instruction, COMPOSE_STRING, COMPOSE_LEN) == 0) {
    ParametricUnaryOperation(s, instruction, line_num);
  } else {
    HandleErrorCode(WRONG_COMMAND_CODE, line_num);
  }

}

/**
 * A function that reads a single line.
 * It creates a new ErrorHandler, which it will use to handle errors.
 * Reads a line with getline, in the case of an error, a comment or a
 * blank line it does nothing. If the first read character will be a letter,
 * then it interprets the line as a command. Else it interprets it as a
 * polynomial and reads it.
 * A helper variable help determines the end of a read polynomial; if it is
 * not '\n' or '\0' by the time reading input is done (!feof), it sends
 * an appropriate message to ErrorHandler.
 * Next it checks ErrorHandler's state and if there are no errors, it places
 * the polynomial on the top of the stack.
 * After performing it frees all of the allocated by getline memory.
 * @param s : stack
 * @param line_number : line number
 */
static void CalcReadLine(Tstack *s, size_t line_number) {
  char *line = NULL;
  size_t dummy = 0;
  ErrorHandler handler = NewErrorHandler(line_number);

  ;
  if (getline(&line, &dummy, stdin) == GETLINE_ERROR
      || line[0] == COMMENT_CHAR || line[0] == NEWLINE) {}
  else if (isalpha(line[0])) {
    CalcInterpretOperation(s, line, line_number);
  } else {
    char *help = NULL;
    Poly input_poly = PolyRead(line, &help, &handler);
    if (help != NULL && help[0] != NEWLINE && !(help[0] == NULL_CHAR
        && feof(stdin))) {
      ErrorHandlerSetCode(&handler, WRONG_POLY_CODE);
    }
    if (IsError(&handler)) {
      HandleError(handler);
      PolyDestroy(&input_poly);
    } else {
      Push(s, input_poly);
    }
  }

  free(line);
}

/**
 * Creates a new stack and initializes it appropriately. Reads lines
 * until the end of file and after that destroys the stack with its contents.
 * @return : 0 if everything went correctly, else the program will exit
 * somewhere else
 */
int main() {
  Tstack stack;
  StackInit(&stack);
  size_t line_number = 0;
  while (!feof(stdin)) {
    line_number++;
    CalcReadLine(&stack, line_number);
  }
  Empty(&stack);

  return 0;
}


