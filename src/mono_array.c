/** @file
  Implementation of a monomial array class.

  @author Adam Al-Hosam <aa429136@students.mimuw.edu.pl>
  @copyright Uniwersytet Warszawski
  @date 2021
*/

#include "mono_array.h"
#include <stdlib.h>
#include "error_handler.h"


/// Constant to multiply the size if there is a need to allocate more memory.
#define RESIZE_CONST 2

Mono *MonoNewArray(size_t size) {
    if (size <= 0) {
        return NULL;
    }

    Mono *ptr_to_new_mono_array = malloc(size * sizeof (Mono));
    CHECK_PTR(ptr_to_new_mono_array);

    return ptr_to_new_mono_array;
}

Poly TrimAndInterpretMonoArr(Mono *array_to_resize, size_t used,
                             size_t reserved) {
    if (used == 0) {    // everything got reduced
        MonoArrayDestroy(array_to_resize, used);
        return PolyZero();
    }
    else if (used == 1 && PolyIsZero(&array_to_resize[0].p)) {
        MonoArrayDestroy(array_to_resize, used);
        return PolyZero();
    }
    else if (used == 1 && MonoGetExp(&array_to_resize[0]) == 0 &&
             PolyIsCoeff(&array_to_resize[0].p)) {
        poly_coeff_t for_later = array_to_resize[0].p.coeff;
        MonoArrayDestroy(array_to_resize, used);
        return PolyFromCoeff(for_later);
    }
    else if (used == reserved) {  //everything ok, takes as much as reserved
        return PolyFromSizeAndArray(reserved, array_to_resize);
    }
    else {
        Mono *for_result = realloc(array_to_resize, used * sizeof (Mono));
        CHECK_PTR(for_result);
        return PolyFromSizeAndArray(used,for_result);
    }
}

/**
 * Helper function for a qsort in #MonoSort.
 */
static int Comparator(const void *a, const void *b) {
    Mono arg1 = *(const Mono*) a;
    Mono arg2 = *(const Mono*) b;

    if (arg1.exp > arg2.exp) {
        return 1;
    }
    else if (arg1.exp < arg2.exp) {
        return -1;
    }
    return 0;
}

void MonoSort(Mono *array_to_sort, size_t size) {
    qsort(array_to_sort, size, sizeof(Mono), Comparator);
}

void MonoArrayDestroy(Mono *array_to_destroy, size_t size) {
    for (size_t i = 0; i < size; i++) {
        MonoDestroy(&array_to_destroy[i]);
    }
    free(array_to_destroy);
}

DynamicMonoArray NewDynamicMonoArray() {
    return (DynamicMonoArray) {.size = 0, .reserved = 0, .mono_array = NULL};
}

void DynamicMonoArrayAdd(DynamicMonoArray *dynamic_array, Mono *mono_to_add) {
    if (dynamic_array->size == dynamic_array->reserved) {
        dynamic_array->reserved = dynamic_array->reserved * RESIZE_CONST + 1;
        dynamic_array->mono_array = realloc(dynamic_array->mono_array,
                                      dynamic_array->reserved * sizeof (Mono));
        CHECK_PTR(dynamic_array->mono_array);
    }

    dynamic_array->mono_array[dynamic_array->size++] = *mono_to_add;
}