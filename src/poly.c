/** @file
  Implementation of a multivariable polynomial class.

  @author Adam Al-Hosam <aa429136@students.mimuw.edu.pl>
  @copyright Uniwersytet Warszawski
  @date 2021
*/

#include <stdlib.h>
#include "poly.h"
#include "mono_array.h"
#include "error_handler.h"

/**
 * Value by which the monomial/polynomial is multiplied to negate it.
 */
#define NEG (-1)

/**
 * Degree of a constant  polynomial.
 */
#define CONST_DEGREE (-1)

/**
 * Any negative (such that any positive value is larger than it) value.
 * Helpful while finding maximum in #PolyDeg
 */ 
#define SMALL_VALUE (-1)

void PolyDestroy(Poly *p) {
    assert(p != NULL);

    if (!PolyIsCoeff(p)) {
        for (size_t i = 0; i < p->size; i++) {
            MonoDestroy(&p->arr[i]);
        }
        free(p->arr);
        p->arr = NULL;
    }
}

Poly PolyClone(const Poly *p) {
    assert(p != NULL);

    if (PolyIsCoeff(p)) {
        return PolyFromCoeff(p->coeff);
    }
    else {
        Mono *new_array = MonoNewArray(p->size);

        for (size_t i = 0; i < p->size; i++) {
            new_array[i] = MonoClone(&p->arr[i]);
        }

        return PolyFromSizeAndArray(p->size, new_array);
    }
}

/**
 * Helper function that adds two constant polynomials. Used in #PolyAdd.
 * @param[in] p : polynomial
 * @param[in] q : polynomial
 * @return constant polynomial @f$p+q@f$
 */
static Poly PolyAddTwoCoeffs(const Poly *p, const Poly *q) {
    assert(p != NULL && q != NULL);
    return PolyFromCoeff(p->coeff + q->coeff);
}

/**
 * Helper function for #PolyAddCoeffToNonCoeff.
 * It is used in a situation when a constant polynomial @p q is added to
 * polynomial @p p, which is not constant and @p p contains a monomial with
 * an exponent 0. Function creates a new monomial array, which is then used
 * to fill a result polynomial, but if a coefficient in lowest exponent of a
 * variable ends up 0, then it is ignored.
 * I'm using the invariant that polynomials are sorted according  to the
 * exponent.
 * @param[in] p : polynomial
 * @param[in] q : polynomial
 * @return polynomial @f$p+q@f$
 */
static Poly PolyAddCoeffIfPhasConst(const Poly *p, const Poly *q) {
    assert(p != NULL && q != NULL);

    Mono *result_array;
    size_t size = 0;

    Poly new_poly_for_x0 = PolyAdd(&p->arr[0].p, q);

    if (PolyIsZero(&new_poly_for_x0)) {
        PolyDestroy(&new_poly_for_x0);
        result_array = MonoNewArray(p->size - 1);
    }
    else {
        result_array = MonoNewArray(p->size);
        result_array[size++] = MonoFromPoly(&new_poly_for_x0, 0);
    }

    for (size_t i = 1; i < p->size; i++) {
        (result_array)[size++] = MonoClone(&p->arr[i]);
    }

    return PolyFromSizeAndArray(size, result_array);
}

/**
 * Helper function for #PolyAddCoeffToNonCoeff.
 * Used when adding a constant polynomial @p q to polynomial @p p, which is
 * not constant and @p p doesn't contain a monomial with  exponent 0.
 * Function creates a new monomial array, and then uses it to fill a result
 * polynomial.
 * Jest to wariant dodawania wielomianu stałego @p q do wielomianu @p p, który
 * nie jest stały oraz wielomian @p p nie zawiera jednomianu z zerową potęgą
 * zmiennej. Funkcja tworzy nową tablicę jednomianów, którą następnie
 * uzupełnia wynikowy wielomian.
 * I'm using the invariant that polynomials are sorted according  to the
 * exponent.
 * @param[in] p : not constant polynomial
 * @param[in] q : constant polynomial
 * @return polynomial @f$p+q@f$
 */
static Poly PolyAddCoeffIfPDoesntHaveConst(const Poly *p, const Poly *q) {
    assert(p != NULL && q != NULL);

    Mono *result_array;
    size_t size = 0;

    result_array = MonoNewArray(p->size + 1);
    result_array[size++] = MonoFromPoly(q, 0);

    for (size_t i = 0; i < p->size; i++) {
        result_array[size++] = MonoClone(&p->arr[i]);
    }

    return PolyFromSizeAndArray(size, result_array);
}

/**
 * Helper function for #PolyAdd.
 * Adds a constant polynomial to a polynomial which is not constant.
 * First if ensures that later in the function, it will be @p q that is
 * constant.
 * @param[in] p : polynomial
 * @param[in] q : polynomial
 * @return polynomial @f$p+q@f$
  */
static Poly PolyAddCoeffToNonCoeff(const Poly *p, const Poly *q) {
    assert(p != NULL && q != NULL);

    if (PolyIsCoeff(p)) {
        return PolyAddCoeffToNonCoeff(q, p);
    }
    assert(PolyIsCoeff(q));

    if (PolyIsZero(q)) {
        return PolyClone(p);
    }

    if (MonoGetExp(p->arr) == 0) {      // jest x_i^0
        return PolyAddCoeffIfPhasConst(p, q);
    }
    else {                              // nie ma x_i^0
        return PolyAddCoeffIfPDoesntHaveConst(p, q);
    }
}

/**
 * Helper function that "finishes" copying monomials.
 * (one of the @p p or @p q to the @p array_for_copy array).
 * @param[in] index_p : how many polynomials from p are already copied
 * @param[in] index_q : how many polynomials from q are already copied
 * @param[in] p : polynomial
 * @param[in] q : polynomial
 * @param[in] array_for_copy : an array to fill (partially filled)
 * @param[in] index_arr : number of filled elements in @p array_for_copy
 */
static void CopyWhatsLeft(size_t index_p, size_t index_q, const Poly *p,
                          const Poly *q, Mono *array_for_copy, size_t
                          *index_arr) {
    assert(p != NULL && q != NULL);

    if (index_p == p->size) {
        for (size_t i = index_q; i < q->size; i++) {
            array_for_copy[(*index_arr)++] = MonoClone(&q->arr[i]);
        }
    }
    else if (index_q == q->size) {
        for (size_t i = index_p; i < p->size; i++) {
            array_for_copy[(*index_arr)++] = MonoClone(&p->arr[i]);
        }
    }
}

/**
 * @brief Function that adds two polynomials none of which is constant.
 * I'm using the invariant that polynomials are sorted according  to the
 * exponent. Function adds two monomials if they have the same exponent, else
 * copies the one that has a lower exponent and increases the counter.
 * @param[in] p : polynomial
 * @param[in] q : polynomial
 * @return polynomial @f$p+q@f$
 */
static Poly PolyAddTwoNonCoeffs(const Poly *p, const Poly *q) {
    assert(p != NULL && q != NULL);

    // array to which the monomials are copied
    Mono *new_array = MonoNewArray(p->size + q->size);
    size_t index_arr = 0;

    // indices of currently looked at place in p i q;
    size_t index_p = 0, index_q = 0;

    // for convenience I remember the pointer to given monomials.
    Mono *mono_from_p, *mono_from_q;

    while (index_p < p->size && index_q < q->size) {
        mono_from_p = &p->arr[index_p];
        mono_from_q = &q->arr[index_q];

        if (MonoGetExp(mono_from_p) == MonoGetExp(mono_from_q)) {
            Mono new_mono = MonoAdd(mono_from_p, mono_from_q);

            if (!PolyIsZero(&new_mono.p)) {
                new_array[index_arr++] = new_mono;
            }
            else {  // coefficients got reduced
                MonoDestroy(&new_mono);
            }

            index_p += 1;
            index_q += 1;
        }
        else if (MonoGetExp(mono_from_p) > MonoGetExp(mono_from_q)) {
            new_array[index_arr++] = MonoClone(mono_from_q);
            index_q += 1;
        }
        else { // MonoGetExp(mono_from_p) < MonoGetExp(mono_from_q)
            new_array[index_arr++] = MonoClone(mono_from_p);
            index_p += 1;
        }
    }

    CopyWhatsLeft(index_p,index_q,p,q,new_array,&index_arr);
    return TrimAndInterpretMonoArr(new_array, index_arr,
                                   p->size + q->size);
}

Poly PolyAdd(const Poly *p, const Poly *q) {
    assert(p != NULL && q != NULL);

    if (PolyIsCoeff(p) && PolyIsCoeff(q)) {
        return PolyAddTwoCoeffs(p,q);
    }
    else if (PolyIsCoeff(p) || PolyIsCoeff(q)) {
        return PolyAddCoeffToNonCoeff(p,q);
    }
    else {
        return PolyAddTwoNonCoeffs(p,q);
    }
}

Poly PolyAddMonos(size_t count, const Mono monos[]) {
    if (count == 0) {
        return PolyZero();
    }

    Mono *copy_array = MonoNewArray(count);
    for (size_t i = 0; i < count; i++) {
        copy_array[i] = monos[i];
    }

    MonoSort(copy_array, count);

    size_t new_index = 0;
    for (size_t i = 1; i < count; i++) {
        Mono to_destroy = copy_array[i];
        if (MonoGetExp(&copy_array[new_index]) == MonoGetExp(&copy_array[i])) {
            Mono new_mono = MonoAdd(&copy_array[new_index], &copy_array[i]);
            MonoDestroy(&(copy_array[new_index]));
            copy_array[new_index] = new_mono;
        }
        else {
            if (!PolyIsZero(&copy_array[new_index].p)) {
                new_index += 1;
            }
            else {
                MonoDestroy(&copy_array[new_index]);
            }
            copy_array[new_index] = MonoClone(&copy_array[i]);
        }
        MonoDestroy(&to_destroy);
    }
    return TrimAndInterpretMonoArr(copy_array,  new_index + 1,new_index+ 1);
}

/**
 * Helper function for PolyMul.
 * Executes multiplying polynomial which is not a constant with a constant
 * polynomial.
 * First if ensures that later on @p q is the constant polynomial.
 * Function multiplies each coefficient in polynomial by a constant.
 * @param[in] p : polynomial
 * @param[in] q : polynomial
 * @return polynomial @f$p\cdot q@f$
 */
static Poly PolyMulCoeffAndNonCoeff(const Poly *p, const Poly *q) {
    assert(p != NULL && q != NULL);

    if (PolyIsCoeff(p)) {
        return PolyMulCoeffAndNonCoeff(q,p);
    }
    assert(PolyIsCoeff(q));
    if (PolyIsZero(q)) {
        return PolyZero();
    }

    Mono *result = MonoNewArray(p->size);

    for (size_t i = 0; i < p->size; i++) {
        result[i] = MonoMulCoeff(&p->arr[i], q);
    }

    Poly to_return = PolyAddMonos(p->size, result);
    free(result);

    return to_return;
}

Poly PolyMul(const Poly *p, const Poly *q) {
    assert(p != NULL && q != NULL);

    if (PolyIsCoeff(p) && PolyIsCoeff(q)) {
        return PolyFromCoeff(q->coeff * p->coeff);
    }
    else if (PolyIsCoeff(p) || PolyIsCoeff(q)) {
        return PolyMulCoeffAndNonCoeff(p, q);
    }
    else { // both are not constant
        Mono *new_array = MonoNewArray(p->size * q->size);
        size_t new_index = 0;
        for (size_t i = 0; i < p->size; i++) {
            for (size_t j = 0; j < q->size; j++) {
                Mono new_mono = MonoMul(&p->arr[i], &q->arr[j]);
                new_array[new_index++] = new_mono;
            }
        }

        Mono *new_smaller_array = realloc(new_array ,new_index * sizeof (Mono));
        CHECK_PTR(new_smaller_array);

        Poly resulting_poly = PolyAddMonos(new_index, new_smaller_array);
        free(new_smaller_array);

        return resulting_poly;
    }
}

Poly PolyNeg(const Poly *p) {
    assert(p != NULL);

    if (PolyIsCoeff(p)) {
        return PolyFromCoeff(NEG * p->coeff);
    }
    else {
        Mono *new_mono_array = MonoNewArray(p->size);
        for (size_t i = 0; i < p->size; i++) {
            new_mono_array[i] = MonoNeg(&(p->arr[i]));
        }

        return PolyFromSizeAndArray(p->size, new_mono_array);
    }
}

Poly PolySub(const Poly *p, const Poly *q) {
    assert(p != NULL && q != NULL);

    Poly negated_q = PolyNeg(q);
    Poly result = PolyAdd(p, &negated_q);
    PolyDestroy(&negated_q);

    return result;
}

poly_exp_t PolyDegBy(const Poly *p, size_t var_idx) {
    assert(p != NULL);

    if (PolyIsZero(p)) {
        return -1;
    }
    else if (PolyIsCoeff(p)) {
        return 0;
    }
    else if (var_idx == 0) { // good depth
        return MonoGetExp(&p->arr[p->size - 1]);
    }   // polynomial is somewhere in the coefficient
    else {
        poly_exp_t maxi = -1;
        for (size_t i = 0; i < p->size; i++) {
            poly_exp_t actual = PolyDegBy(&p->arr[i].p, var_idx - 1);
            if (actual > maxi) {
                maxi = actual;
            }
        }
        return maxi;
    }
}

poly_exp_t PolyDeg(const Poly *p) {
    assert(p != NULL);

    if (PolyIsZero(p)) {
        return CONST_DEGREE;
    }
    else if (PolyIsCoeff(p)) {
        return 0;
    }
    else {
        poly_exp_t maxi = SMALL_VALUE;
        for (size_t i = 0; i < p->size; i++) {
            poly_exp_t actual = MonoDeg(&p->arr[i]);
            if (actual > maxi) {
                maxi = actual;
            }
        }
        return maxi;
    }
}

bool PolyIsEq(const Poly *p, const Poly *q) {
    assert(p != NULL && q != NULL);

    if (PolyIsCoeff(p) && PolyIsCoeff(q)) {
        return p->coeff == q->coeff;
    }
    else if (PolyIsCoeff(p) || PolyIsCoeff(q)) {
        return false;
    }
    else { // both are real polynomials
        if (p->size != q->size) {
            return false;
        }
        else {
            for (size_t i = 0; i < p->size; i++) {
                if (!MonoIsEq(&p->arr[i], &q->arr[i])) {
                    return false;
                }
            }
        }
    }

    return true;
}

Poly PolyAt(const Poly *p, poly_coeff_t x) {
        assert(p != NULL);

        if (PolyIsCoeff(p)) {
            return PolyClone(p);
        }

        Poly result = PolyZero();

        for (size_t i = 0; i < p->size; i++) {
            Poly to_destroy = result; //old
            Poly new_poly = MonoAt(&p->arr[i], x);

            result = PolyAdd(&result, &new_poly);

            PolyDestroy(&new_poly);
            PolyDestroy(&to_destroy);
        }

        return result;
    }

Poly PolyOwnMonos(size_t count, Mono *monos) {
    if (monos == NULL) {
        return PolyZero();
    }
    else {
        Poly to_return = PolyAddMonos(count, monos);
        free(monos);
        return to_return;
    }
}

Poly PolyCloneMonos(size_t count, const Mono monos[]) {
    if (monos == NULL) {
        return PolyZero();
    }
    else {
        Mono *clone_mono_array = MonoNewArray(count);
        for (size_t i = 0; i < count; i++) {
            clone_mono_array[i] = MonoClone(&monos[i]);
        }

        Poly to_return = PolyAddMonos(count, clone_mono_array);
        free(clone_mono_array);
        return to_return;
    }
}

/**
 * Helper function which performs fast exponentiation of polynomials.
 * @param p : base of exponentiation
 * @param exp : exponent
 * @return : polynomial @f$p@f$ multiplied by itself @f$exp@f$ times.
 */
static Poly PolyPower(const Poly *p, poly_exp_t exp) {
    if (exp == 0) {
        return PolyFromCoeff(1);
    }
    else if (exp == 1) {
        return PolyClone(p);
    }
    else if (exp & 1) {
        Poly foo1 = PolyMul(p,p);
        Poly foo2 = PolyPower(&foo1, (exp - 1) / 2);
        Poly to_return = PolyMul(p, &foo2);
        PolyDestroy(&foo1);
        PolyDestroy(&foo2);
        return to_return;
    }
    else {
        Poly foo1 = PolyMul(p,p);
        Poly to_return = PolyPower(&foo1, exp / 2);
        PolyDestroy(&foo1);
        return to_return;
    }
}

static Poly PolyComposeHelper(const Poly *p, size_t k, size_t var_id,
                              const Poly q[]);

/**
 * Function which puts a polynomial from q array in a given monomial in place
 * variable with index var_id. If the exponent is 0 (and the coefficient is a
 * constant polynomial), it returns a copy of the coefficient. Else it
 * calls itself recursively for the coefficient and returns the value it gets
 * after multiplying received coefficient and polynomial that was put to the
 * monomial's power.
 * @param m : monomial to which the polynomial with be put in
 * @param k : number of elements in q
 * @param var_id : number of currently considered variable
 * @param q : polynomial array (for replacing)
 * @return polynomial created after putting a given polynomial to the monomial
 */
static Poly MonoComposeHelper(const Mono *m, size_t k, size_t var_id,
                              const Poly q[]) {
    Poly to_return;
    Poly coeff = PolyComposeHelper(&m->p, k, var_id + 1, q);

    if (m->exp == 0) {
        return coeff;
    }

    Poly to_destroy = coeff;
    if (var_id < k) {
        Poly after_exp = PolyPower(&q[var_id], m->exp);
        to_return = PolyMul(&coeff, &after_exp);
        PolyDestroy(&after_exp);
    }
    else {
        to_return = PolyZero();
    }
    PolyDestroy(&to_destroy);
    return to_return;

}

/**
 * Funkcja, która wykonuje operację złożenia. Wstawia pod zmienną var_id
 * w wielomianie @f$p@f$ odpowiedni wielomian z tablicy q.
 * @param p : wielomian, do którego będą wstawiane nowe wartości pod zmienne
 * @param k : ilość elementów w tablicy q
 * @param var_id : numer aktualnie rozpatrywanej zmiennej
 * @param q : tablica wielomianów (do podstawienia)
 * @return wielomian powstały po wstawieniu pod zmienną var_id odpowiedniego
 * wielomianu
 */
static Poly PolyComposeHelper(const Poly *p, size_t k, size_t var_id,
                              const Poly q[]) {
    if (PolyIsCoeff(p)) {
        return PolyClone(p);
    }
    else {
        Poly result = PolyZero();
        Poly to_destroy;
        Poly result_of_mono_compose;
        for (size_t i = 0; i < p->size; i++) {
            to_destroy = result;
            result_of_mono_compose = MonoComposeHelper(&p->arr[i], k, var_id,q);
            result = PolyAdd(&result, &result_of_mono_compose);
            PolyDestroy(&to_destroy);
            PolyDestroy(&result_of_mono_compose);
        }
        return result;
    }
}

Poly PolyCompose(const Poly *p, size_t k, const Poly q[]) {
    return PolyComposeHelper(p, k, 0, q);
}

