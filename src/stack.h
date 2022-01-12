/** @file
  Interface of a multivariable polynomial stack.

  @author Adam Al-Hosam <aa429136@students.mimuw.edu.pl>
  @copyright Uniwersytet Warszawski
  @date 2021
*/

#ifndef STACK_H
#define STACK_H

#include "poly.h"

/**
 * Structure representing a multivariable polynomial stack as an array.
 */
typedef struct Stack {
    size_t size;        ///< ilość elementów na stosie
    size_t reserved;    ///< ilość zarezerwowanej pamięci
    Poly *elements;     ///< wielomiany
} Tstack;

/**
 * Initializing an empty stack.
 * @param s : stack to initialize
 */
void StackInit(Tstack *s);

/**
 * Checks if the stack is empty.
 * @param s : stack to check
 * @return is stack empty?
 */
bool StackIsEmpty(Tstack *s);

/**
 * @brief Places a polynomial on top of the stack.
 * @details When increasing the  memory adds 1, because the stack might have a
 * size of 0.
 * @param s : stack
 * @param poly_to_push : polynomial to place on top
 */
void Push(Tstack *s, Poly poly_to_push);

/**
 * Takes of a polynomial from the top of the stack.
 * @param s : stack
 * @return polynomial from the top of the stack
 */
Poly Pop(Tstack *s);

/**
 * Checks if the stack has at least 2 polynomials.
 * @param s : stos
 * @return Does the stack have at least 2 polynomials?
 */
bool StackDoesHaveAtLeastTwoElements(Tstack *s);

/**
 * @brief Clears the contents of the stack (and frees the memory of the contents)
 * @details Pops of and destroys a polynomial as long as the stack is not empty.
 * The array will for sure be freed, because when size reaches 0, then for sure
 * the size of the array will be reduced in Pop.
 * @param s : stack to empty
 */
void Empty(Tstack *s);

/**
 * Returns the number of elements on the stack.
 * @param s : stos
 * @return number  of elements on the stack
 */
size_t StackSize(Tstack *s);

#endif //STACK_H
