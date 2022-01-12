/** @file
  Implementation of a multivariable polynomial stack.

  @author Adam Al-Hosam <aa429136@students.mimuw.edu.pl>
  @copyright Uniwersytet Warszawski
  @date 2021
*/

#include "stack.h"
#include "input_output.h"

/// When increasing the stack's size, this is the multiplier.
#define SIZE_EXPAND_CONST 2

/**
 * When checking if it is good to reduce allocated memory i check if
 * the used space is at least SIZE_SHRINK_BOUND times smaller than reserved.
 */
#define SIZE_SHRINK_BOUND 4

void StackInit(Tstack *s) {
    s->size = 0;
    s->reserved = 0;
    s->elements = NULL;
}

bool StackIsEmpty(Tstack *s) {
    return s->size == 0;
}

/**
 * Function to resize the stack.
 * After doing the resizing, it checks the validity of the pointer.
 * @param s : stack to resize
 * @param new_size : new size
 */
static void StackResize(Tstack *s, size_t new_size) {
    s->reserved = new_size;
    s->elements = realloc(s->elements, s->reserved * sizeof (Poly));

    if (new_size != 0) {
        CHECK_PTR(s->elements);
    }
}

void Push(Tstack *s, Poly poly_to_push) {
    if (s->size == s->reserved) {
        StackResize(s,s->reserved * SIZE_EXPAND_CONST + 1);
    }
    s->elements[s->size++] = poly_to_push;
}

Poly Pop(Tstack *s) {
    s->size--;
    Poly to_return = s->elements[s->size];

    if (s->size <= s->reserved / SIZE_SHRINK_BOUND) {
        StackResize(s, s->size);
    }
    return to_return;
}

bool StackDoesHaveAtLeastTwoElements(Tstack *s) {
    return s->size >= 2;
}

void Empty(Tstack *s) {
    Poly to_destroy;

    while(s->size > 0) {
        to_destroy = Pop(s);
        PolyDestroy(&to_destroy);
    }
}

size_t StackSize(Tstack *s) {
    return s->size;
}