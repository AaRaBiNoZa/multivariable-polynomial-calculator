/** @file
  Interface of a multivariable polynomial class.

  @authors Jakub Pawlewicz <pan@mimuw.edu.pl>, Marcin Peczarski <marpe@mimuw.edu.pl>,
  Adam Al-Hosam <aa429136@students.mimuw.edu.pl>
  @copyright Uniwersytet Warszawski
  @date 2021
*/

#ifndef __POLY_H__
#define __POLY_H__

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>

/** Type representing coefficients */
typedef long poly_coeff_t;

/** Type representing exponents. */
typedef int poly_exp_t;

struct Mono;

/**
 * Structure for storing a polynomial.
 * Polynomial is either an integer (constant polynomial), and then
 * arr == NULL, or a not empty array of monomials and then arr != NULL
 */
typedef struct Poly {
    /**
     * Union to store a polynomial's coefficient of a number of monomials in
     * the polynomial.
     * If arr == NULL, then it is an integer, else it is a not empty
     * array of monomials.
    */
    union {
        poly_coeff_t coeff; ///< coefficient
        size_t       size; ///< size of the polynomial; number of monomials
    };
    /** Array to store an array of monomials. */
    struct Mono *arr;
} Poly;

/**
 * Structure to store a monomial.
 * Monomial is of the form @f$px_i^n@f$.
 * Coefficient @f$p@f$ can also be
 * a polynomial of a next variable @f$x_{i+1}@f$.
 */
typedef struct Mono {
    Poly p; ///< współczynnik
    poly_exp_t exp; ///< wykładnik
} Mono;

/**
 * Returns a value of the monomial's exponent.
 * @param[in] m : monomial
 * @return value of the monomial's exponent.
 */
static inline poly_exp_t MonoGetExp(const Mono *m) {
    assert(m != NULL);
    return m->exp;
}

/**
 * Creates polynomial which is a coefficient (constant polynomial)
 * @param[in] c : value of the coefficient
 * @return polynomial
 */
static inline Poly PolyFromCoeff(poly_coeff_t c) {
    return (Poly) {.coeff = c, .arr = NULL};
}

/**
 * Creates a polynomial from a given monomial array.
 * @param[in] size : size of the array
 * @param[in] arr : array
 * @return : polynomial
 */
static inline Poly PolyFromSizeAndArray(size_t size, Mono *arr) {
    assert(arr != NULL && size != 0);
    return (Poly) {.size = size, .arr = arr};
}

/**
 * Creates a polynomial that equals zero.
 * @return polynomial
 */
static inline Poly PolyZero(void) {
    return PolyFromCoeff(0);
}

static inline bool PolyIsZero(const Poly *p);

/**
 * Creates a monomial @f$px_i^n@f$.
 * Takes over the contents of struct pointed by @p p.
 * @param[in] p : polynomial - coefficient of the monomial
 * @param[in] n : exponent
 * @return monomial @f$px_i^n@f$
 */
static inline Mono MonoFromPoly(const Poly *p, poly_exp_t n) {
    assert(n == 0 || !PolyIsZero(p));
    return (Mono) {.p = *p, .exp = n};
}

/**
 * Checks if a polynomial is a coefficient (if it is constant)
 * @param[in] p : monomial
 * @return Is the monomial a coefficient?
 */
static inline bool PolyIsCoeff(const Poly *p) {
    assert(p != NULL);
    return p->arr == NULL;
}

/**
 * Checks if the polynomial is equal to 0.
 * @param[in] p : polynomial
 * @return Is the polynomial equal to 0?
 */
static inline bool PolyIsZero(const Poly *p) {
    assert(p != NULL);
    return PolyIsCoeff(p) && p->coeff == 0;
}

/**
 * @brief Removes a polynomial from memory.
 * @details Function works in a hybrid way - recursively and iteratively.
 * Let's notice that a polynomial can be:
 * - a coefficient (constant polynomial) - then the memory is allocated
 * statically
 * - not empty array of monomials - memory is allocated dynamically - then
 * the monomials must be removed.
 * @param[in] p : polynomial
 */
void PolyDestroy(Poly *p);

/**
 * Removes a monomial from memory.
 * @param[in] m : monomial
 */
static inline void MonoDestroy(Mono *m) {
    assert(m != NULL);
    PolyDestroy(&m->p);
}

/**
 * @brief Performs a full, deep copy of a polynomial.
 * @details Function works in a hybrid way - recursively and iteratively.
 * If:
 * - polynomial is only a coefficient, then it returns the same polynomial
 * (nothing allocated)
 * - polynomial is a not empty list of monomials - then it creates an array
 * with an appropriate size and copies monomials.
 * @return copied polynomial
 */
Poly PolyClone(const Poly *p);

/**
 * Performs a full, deep copy of a monomial.
 * @param[in] m : monomial
 * @return copied monomial
 */
static inline Mono MonoClone(const Mono *m) {
    assert(m != NULL);
    return (Mono) {.p = PolyClone(&m->p), .exp = m->exp};
}

/**
 * Adds two polynomials.
 * @param[in] p : polynomial @f$p@f$
 * @param[in] q : polynomial @f$q@f$
 * @return @f$p + q@f$
 */
Poly PolyAdd(const Poly *p, const Poly *q);

/**
 * Adds two polynomials with the same exponent.
 * @param[in] m : monomial
 * @param[in] n : monomial
 * @return monomial @f$m + n@f$
 */
static inline Mono MonoAdd(const Mono *m, const Mono *n) {
    assert(m != NULL && n != NULL);
    return (Mono) {.p = PolyAdd(&m->p, &n->p), .exp = m->exp};
}

/**
 * @brief Sums an array of monomials and creates a polynomial from it.
 * @details Takes over the contents of @p monos array.
 * Creates an array with copies of pointers to contents of @p monos, and then
 * sorts it. Now adds monomials to the first element as long as the exponents
 * match and simultaneously deletes them from memory.
 * When it comes upon a first monomial, whose exponent doesn't match it checks
 * if a monomial that it gets after adding the variables with the same power
 * is not equal 0. If it is not hten it copies it to the next spot and repeats.
 * Ifit is  equal to 0 then removes it and sets  a next one to its place
 * (with a higher exponent).
 * It is imporant that new_index will never be bigger than i, because
 * the initial value of new_index is 0 and i is 1. In each iteration
 * new_index increases AT MOST by one.
 * @param[in] count : number of monomials
 * @param[in] monos : array of monomials
 * @return polynomial received after adding monomials.
 */
Poly PolyAddMonos(size_t count, const Mono monos[]);

/**
 * @brief Multiplies two polynomials.
 * @details Function creates monomials by multiplying each
 * factor from @p p by each from @p q and stores them in an array of length
 * @f$\deg(p) \cdot \deg(q)@f$.
 * Next - if needed, decreases the size of this array and using
 *  PolyAddMonos creates a polynomial that is a product.
 * @param[in] p : polynomial @f$p@f$
 * @param[in] q : polynomial @f$q@f$
 * @return @f$p * q@f$
 */
Poly PolyMul(const Poly *p, const Poly *q);

/**
 * Multiplies two monomials.
 * @param[in] m : monomial
 * @param[in] n : monomial
 * @return @f$ m \cdot n @f$
 */
static inline Mono MonoMul(const Mono *m, const Mono *n) {
    assert(m != NULL && n != NULL);
    return (Mono) {.p = PolyMul(&m->p, &n->p), .exp = m->exp + n->exp};
}

/**
 * Multiplies monomial by a constant.
 * @param[in] m : monomial
 * @param[in] coeff : constant
 * @return @f$ m \cdot coeff @f$
 */
static inline Mono MonoMulCoeff(const Mono *m, const Poly *coeff) {
    assert(m != NULL && coeff != NULL && PolyIsCoeff(coeff));
    return (Mono) {.p = PolyMul(&m->p, coeff), .exp = m->exp};
}

/**
 * Returns negated polynomial.
 * @param[in] p : polynomial @f$p@f$
 * @return @f$-p@f$
 */
Poly PolyNeg(const Poly *p);

/**
 * Returns negated monomial.
 * @param[in] m : monomial @f$m@f$
 * @return monomial @f$-m@f$
 */
static inline Mono MonoNeg(const Mono *m) {
    assert(m != NULL);
    Poly negative_poly = PolyNeg(&m->p);
    return MonoFromPoly(&negative_poly, m->exp);
}

/**
 * Subtracts two polynomials.
 * @param[in] p : polynomial @f$p@f$
 * @param[in] q : polynomial @f$q@f$
 * @return @f$p - q@f$
 */
Poly PolySub(const Poly *p, const Poly *q);

/**
 * Returns a degree of a polynomial in accordance to a given variable
 * (-1 for a polynomial equal to 0). Variables are indexed from 0.
 * Variable with index 0 means a main variable of the polynomial.
 * Bigger indices mean variables of polynomials that are the coefficients.
 * Function is performed recursively, with each call decreasing @p  var_idx
 * by 1. When value of @p var_idx is 0, it means that the function is at an
 * appropriate depth - is considering a good variable. If the variable with
 * given index doesn't exist, just so the function doesn't go deeper it returns
 * 0.
 * This function uses the assumption that the polynomials are sorted
 * according to the exponent, so the degree of a given polynomial
 * (highest exponent of a variable) is in the last place of a monomial array.
 * Function - if needed performs recursively for each monomial, so
 * a wanted degree will be a maximum from returned values.
 * @param[in] p : polynomial
 * @param[in] var_idx : variable index
 * @return degree of @p p according to a variable with index @p var_idx
 */
poly_exp_t PolyDegBy(const Poly *p, size_t var_idx);

/**
 * Returns a degree of a polynomial (-1 if polynomial equals 0).
 * @details Function works recursively. Computes the maximum of degrees of each
 * of the monomials where a monomial's degree is computed in #MonoDeg.
 * @param[in] p : polynomial
 * @return polynomial degree @f$\deg(p)@f$
 */
poly_exp_t PolyDeg(const Poly *p);

/**
 * Returns a monomial's degree.
 * @param[in] m : monomial
 * @return monomial's degree @f$\deg(m)@f$
 */
static inline poly_exp_t MonoDeg(const Mono *m) {
    assert(m != NULL);
    return m->exp + PolyDeg(&m->p);
}

/**
 * @brief Determines the equality of two polynomials.
 * @details After checking trivial cases, checks if number of monomials
 * is equal and if so - compares each of the monomials with #MonoIsEq.
 * @param[in] p : polynomial @f$p@f$
 * @param[in] q : polynomial @f$q@f$
 * @return @f$p = q@f$
 */
bool PolyIsEq(const Poly *p, const Poly *q);

/**
 * Determines the equality of two monomials.
 * @param[in] m : monomial @f$ax_i^n@f$
 * @param[in] n : monomial @f$bx_i^n@f$
 * @return @f$ a == b @f$
 */
static inline bool MonoIsEq(const Mono *m, const Mono *n) {
    assert(m != NULL && n != NULL);
    return (m->exp == n->exp && PolyIsEq(&m->p, &n->p));
}

/**
 * Computes the value of the polynomial in @p x.
 * Replaces the first variable of polynomial with x.
 * Result may be a polynomial if coefficients are polynomials. Then
 * the variables' indices are decreased by 1.
 * Formally, for polynomial @f$p(x_0, x_1, x_2, \ldots)@f$, the result is
 * a polynomial @f$p(x, x_0, x_1, \ldots)@f$.
 * Creates a zero polynomial and then adds to it values of each monomial in
 * @p p, freeing the memory along the way.
 * @param[in] p : polynomial @f$p@f$
 * @param[in] x : value of the argument @f$x@f$
 * @return @f$p(x, x_0, x_1, \ldots)@f$
 */
Poly PolyAt(const Poly *p, poly_coeff_t x);

/**
 * Computes @p nth power of @p x.
 * Uses the fast exponentiation algorithm.
 * @param[in] x : base
 * @param[in] n : exponent
 * @return @f$x^n@f$
 */
static inline poly_coeff_t PowerOf(poly_coeff_t x, poly_exp_t n) {
    assert(n >= 0);
    if (n == 0)
        return 1;
    else if (n == 1)
        return x;
    else if (n & 1) // is odd
        return x * PowerOf(x * x, (n - 1) / 2);
    else
        return PowerOf(x * x, n / 2);
}

/**
 * Computes the value of a polynomial in @p x.
 * Analogical to #PolyAt.
 * @param[in] m : monomial
 * @param[in] x : monomial
 * @return value of a monomial in @f$x@f$
 */
static inline Poly MonoAt(const Mono *m, poly_coeff_t x) {
    assert(m != NULL);
    Poly new_coeff_in_poly_form = PolyFromCoeff(PowerOf(x,m->exp));
    return PolyMul(&m->p, &new_coeff_in_poly_form);
}

/**
 * Sums an array of monomials and creates a polynomial from them.
 * Takes over the memory pointed by @p monos and it's contents. Can freely
 * modify contents of this memory. We assume that the memory pointed by @p monos
 * was allocated on the heap. if @p count or @p monos equals 0 (NULL), creates
 * a zero polynomial.
 * @param[in] count : number of monomials
 * @param[in] monos : array of monomials
 * @return polynomial that is a sum of monomials
 */
Poly PolyOwnMonos(size_t count, Mono *monos);

/**
 * Sums a list of monomials and creates a polynomial out of them. Doesn't
 * modify the contents of @p monos. If it is needed, then it performs full deep
 * copies of monomials from @p monos. If @p count or @p monos is equel to 0
 * (NULL) then it creates a zero polynomial.
 * @param[in] count : number of monomials
 * @param[in] monos : array of monomials
 * @return polynomial that is a sum of monomials
 */
Poly PolyCloneMonos(size_t count, const Mono monos[]);

/**
 * Operation of composing polynomials. With a given polynomial
 * @f$p@f$ and @f$k@f$ polynomials @f$q_0,q_1,q_2,...,q_{k-1}@f$ let @f$l@f$
 * be a number of variables of a polynomial @f$p@f$ and that those variables
 * are marked respectively @f$x_0,x_1,x_2,...,x_{l-1}@f$. The result of
 * compose is a polynomial @f$(p(q_0,q_1,q_2,...)@f$, that is a polynomial
 * created by replacing variables @f$x_i@f$ with polynomials  @f$q_i@f$
 * for @f$i=0,1,2,...,min(k,l) - 1@f$ in @f$p@f$. If @f$k<l@f$ then
 * variables @f$x_k,...,x_{l-1}@f$ are replaced with 0. For example,
 * if @f$k=0@f$ then the result of compose is @f$p(0,0,0,...)@f$.
 * @param p : polynomial that will have its variables replaced
 * @param k : number of polynomials in array
 * @param q : array of polynomials for replacing
 * @return : polynomial p with replaced variables
 */
Poly PolyCompose(const Poly *p, size_t k, const Poly q[]);

#endif /* __POLY_H__ */