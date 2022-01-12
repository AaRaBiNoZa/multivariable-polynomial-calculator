/** @file
  Implementation of input-output of multivariable polynomials.

  @author Adam Al-Hosam <aa429136@students.mimuw.edu.pl>
  @copyright Uniwersytet Warszawski
  @date 2021
*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include "input_output.h"
#include "mono_array.h"

/// char that separates coefficient of a monomial from the exponent.
#define SEPARATOR ','

/// char representing a beginning of a monomial.
#define OPENING_BRACKET '('

/// char representing an ending of a monomial.
#define CLOSING_BRACKET ')'

/// char representing adding
#define PLUS_SIGN '+'

/// char representing subtracting
#define MINUS_SIGN '-'

/**
 * Monomial that normally should not be able to exist in the program, used only
 * for help while reading input, when program detects an error, but still
 * has to return from recursive calls and then free the memory.
 * It exists only so it can be destroyed...
 * @return polynomial @f$0^0@f$
 */
static Mono MonoDummy() {
    return (Mono) {.p = PolyZero(), .exp = 0};
}

void MonoPrint(Mono *m) {
    printf("%c", OPENING_BRACKET);
    PolyPrint(&(m->p));
    printf("%c%d)", SEPARATOR, m->exp);
}

void PolyPrint(Poly *p) {
    if (PolyIsCoeff(p)) {
        printf("%ld", p->coeff);
    }
    else {
        MonoPrint(&(p->arr[0]));
        for (size_t i = 1; i < p->size; i++) {
            printf("%c", PLUS_SIGN);
            MonoPrint(&(p->arr[i]));
        }
    }
}

Mono MonoRead(char *string, char **last, ErrorHandler *handler) {
    if (IsError(handler)) {
        return MonoDummy();
    }

    if (string[0] != OPENING_BRACKET) {
        ErrorHandlerSetCode(handler, WRONG_POLY_CODE);
        return MonoDummy();
    }

    Poly coeff = PolyRead(string + 1, last, handler);

    if (IsError(handler)) {
        PolyDestroy(&coeff);
        return MonoDummy();
    }

    if (*last[0] != SEPARATOR) {
        ErrorHandlerSetCode(handler, WRONG_POLY_CODE);
        PolyDestroy(&coeff);
        return MonoDummy();
    }

    if (!isdigit((*last)[1])) {
        ErrorHandlerSetCode(handler, WRONG_POLY_CODE);
        PolyDestroy(&coeff);
        return MonoDummy();
    }

    errno = 0;
    poly_exp_t exp = (int) strtol(*last + 1, last, NUMBER_BASE);
    if (!IsExpValid(exp)) {
        ErrorHandlerSetCode(handler, WRONG_POLY_CODE);
        PolyDestroy(&coeff);
        return MonoDummy();
    }

    if (*last[0] != CLOSING_BRACKET) {
        ErrorHandlerSetCode(handler, WRONG_POLY_CODE);
        PolyDestroy(&coeff);
        return MonoDummy();
    }

    if (PolyIsZero(&coeff) && exp != 0) {
        return MonoDummy();
    }
    else {
        return MonoFromPoly(&coeff, exp);
    }
}

Poly PolyRead(char *string, char **last, ErrorHandler *handler) {
    if (isdigit(string[0]) || string[0] == MINUS_SIGN) {
        errno = 0;
        Poly to_return = PolyFromCoeff(strtol(string, last, NUMBER_BASE));

        if (!IsCoeffOrAtArgValid(to_return.coeff)) {
            ErrorHandlerSetCode(handler, WRONG_POLY_CODE);
            return PolyZero();
        }

        return to_return;
    }
    else if (string[0] == OPENING_BRACKET){
        DynamicMonoArray monos = NewDynamicMonoArray();
        Mono to_add;

        do {
            if (string[0] == PLUS_SIGN) {
                string++;
            }

            to_add = MonoRead(string, last, handler);

            if (IsError(handler)) {
                MonoArrayDestroy(monos.mono_array, monos.size);
                return PolyZero();
            }

            string = *last + 1;
            if (!(PolyIsZero(&to_add.p) && to_add.exp == 0)) {
                DynamicMonoArrayAdd(&monos, &to_add);
            }
        } while (string[0] == PLUS_SIGN);


        if ((string[0] != SEPARATOR && string[0] != NEWLINE
                            && string[0] != NULL_CHAR)
                            || (string[0] == NULL_CHAR && !feof(stdin))){
            ErrorHandlerSetCode(handler, WRONG_POLY_CODE);
        }

        *last = &string[0];
        Poly to_return = PolyAddMonos(monos.size, monos.mono_array);
        free(monos.mono_array);
        return to_return;
    }
    else {
        ErrorHandlerSetCode(handler, WRONG_POLY_CODE);
        return PolyZero();
    }
}