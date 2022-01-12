/** @file
  Interface of a monomial array class.

  @author Adam Al-Hosam <aa429136@students.mimuw.edu.pl>
  @copyright Uniwersytet Warszawski
  @date 2021
*/

#ifndef MONOARRAY_H
#define MONOARRAY_H

#include "poly.h"

/**
 * Structure representing a dynamic monomial array.
 */
typedef struct DynamicMonoArray {
    Mono *mono_array; ///< monomial array
    size_t size;      ///< number of monomials inside
    size_t reserved;  ///< amount of reserved space
} DynamicMonoArray;

/**
 * Returns an array of Mono structures of @p size length.
* Does it safely - checks if allocating memory was a success.
* @param[in] size : length of an array which we want to allocate.
* @return pointer to a first element of the array.
*/
Mono *MonoNewArray(size_t size);

/**
 * @brief Function that fixes the monomial  array after some operations.
 * @brief especially if the array takes up more memory than it needs.
 * @details Checks edge cases:
 * - if used space is of size 0, it means that the array is actually empty.
 * - if the array contains only one polynomial with coefficient 0. then it is
 * changed for a constant polynomial 0.
 * - if the array contains only one polynomial with a coefficient different from
 * 0 then it returns only the coefficient. ( similar cases, separated for
 * readibility)
 * - if the array is full, it creates a polynomial out of it
 * - if used space is less than size of the array then the unused space
 * is freed and a polynomial is created from the rest.
 * @param[in] array_to_resize : an array to check
 * @param[in] used : used space
 * @param[in] reserved : reserved space
 * @return polynomial created from a given array.
 */
Poly TrimAndInterpretMonoArr(Mono *array_to_resize, size_t used,
                             size_t reserved);

/**
 * Sorts a  monomial array.
 * @param[in] array_to_sort : array to sort
 * @param[in] size : size of the array
 */
void MonoSort(Mono *array_to_sort, size_t size);

/**
 * Destroys a monomial array with its contents.
 * @param[in] array_to_destroy : an array to destroy
 * @param[in] size : its size
 */
void MonoArrayDestroy(Mono *array_to_destroy, size_t size);

/**
 * Creates a new  dynamic monomial array.
 * @return a new dynamic monomial array.
 */
DynamicMonoArray NewDynamicMonoArray();

/**
 * Adds a monomial to a dynamic monomial array .
 * @param dynamic_array : dynamic monomial array
 * @param mono_to_add : monomial to add.
 */
void DynamicMonoArrayAdd(DynamicMonoArray *dynamic_array, Mono *mono_to_add);

#endif //MONOARRAY_H
