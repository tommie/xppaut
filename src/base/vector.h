/**
 * Basic dynamic array data structure.
 *
 * Example:
 *
 *   VECTOR_DECLARE(intvec, IntVec, int)
 *   VECTOR_DEFINE(intvec, IntVec, int)
 *
 *   IntVec v = VECTOR_INIT;
 *   *intvec_append(&v) = 42;
 *   printf("%d\n", v.elems[v.len - 1]);
 *   *intvec_insert(&v, 0, 1) = 43;
 *   intvec_remove(&v, 0, 1);
 *   intvec_clean(&v);
 */
#ifndef XPPAUT_BASE_VECTOR_H
#define XPPAUT_BASE_VECTOR_H

#include <stdint.h>
#include <string.h>

/* --- Macros --- */
/**
 * Initialization for generated vector structures.
 */
#define VECTOR_INIT                                                            \
  { NULL, 0, 0 }

/**
 * Macro to declare the vector type and all functions for an element type.
 *
 * For use in header files or source files.
 *
 * @param nf the name prefix of functions.
 * @param nt name of the vector type to be generated.
 * @param T element type.
 */
#define VECTOR_DECLARE(nf, nt, T)                                              \
  typedef struct {                                                             \
    T *elems;                                                                  \
    size_t len;                                                                \
    size_t cap;                                                                \
  } nt;                                                                        \
  int nf##_init(nt *v, size_t cap);                                            \
  void nf##_clean(nt *v);                                                      \
  T *nf##_append(nt *v);                                                       \
  T *nf##_insert(nt *v, size_t i, size_t n);                                   \
  void nf##_remove(nt *v, size_t i, size_t n);

/**
 * Macro to define all functions for an element type.
 *
 * For use in source files.
 *
 * @param nf the name prefix of functions.
 * @param nt name of the vector type to be generated.
 * @param T element type.
 */
#define VECTOR_DEFINE(nf, nt, T)                                               \
  int nf##_init(nt *v, size_t cap) {                                           \
    return _vector_init((Vector *)v, sizeof(T[1]), cap);                       \
  }                                                                            \
  void nf##_clean(nt *v) { _vector_clean((Vector *)v); }                       \
  T *nf##_append(nt *v) { return _vector_append((Vector *)v, sizeof(T[1])); }  \
  T *nf##_insert(nt *v, size_t i, size_t n) {                                  \
    return _vector_insert((Vector *)v, sizeof(T[1]), i, n);                    \
  }                                                                            \
  void nf##_remove(nt *v, size_t i, size_t n) {                                \
    _vector_remove((Vector *)v, sizeof(T[1]), i, n);                           \
  }

/* --- Types --- */
/**
 * Base type for all vectors. The generated types must be possible to cast to
 * this.
 */
typedef struct {
  /** Elements of the vector. The allocated size is cap*sizeof(T). */
  void *elems;
  /** Number of elements in the vector. */
  size_t len;
  /** Capacity of the vector. cap >= len at all times. */
  size_t cap;
} Vector;

/* --- Functions --- */
/**
 * Initializes the vector, with a given initial capacity.
 *
 * Must not be called on a previously initialized vector, unless the vector
 * has been cleaned first.
 *
 * @param v the vector to initialize.
 * @param size the size of the element type.
 * @param cap the initial capacity.
 * @return Zero on success. Non-zero means the vector is still uninitialized.
 */
int _vector_init(Vector *v, size_t size, size_t cap);

/**
 * Deallocates the elements of the vector.
 *
 * @param v the vector to clean.
 */
void _vector_clean(Vector *v);

/**
 * Calls abort(3).
 *
 * This is to avoid having to include stdlib.h everywhere since it interferes
 * with libf77.
 */
void _vector_abort(void);

/**
 * Ensures the capacity of the vector is at least n elements.
 *
 * @param v the vector to operate on.
 * @param size the size of the element type.
 * @param n the minimum number of elements.
 * @return zero on success. Non-zero if the capacity may still be less than n.
 */
int _vector_ensure(Vector *v, size_t size, size_t n);

/**
 * Appends an element to the vector and return a pointer to the element.
 *
 * The element returned is uninitialized.
 *
 * @param v the vector to append to.
 * @param size the size of the element type.
 * @return a pointer to the new element.
 */
static inline void *_vector_append(Vector *v, size_t size) {
  if (_vector_ensure(v, size, v->len + 1))
    return NULL;

  return &((uint8_t *)v->elems)[size * v->len++];
}

/**
 * Inserts elements in the vector.
 *
 * The elements returned are uninitialized.
 *
 * Calls abort(3) if the index is out of bounds.
 *
 * @param v the vector to look up in.
 * @param size the size of the element type.
 * @param i index of the first new element.
 * @param n number of elements to insert.
 * @return a pointer to the new element.
 */
static inline void *_vector_insert(Vector *v, size_t size, size_t i, size_t n) {
  if (i > v->len)
    _vector_abort();

  if (_vector_ensure(v, size, v->len + n))
    return NULL;

  void *p = &((uint8_t *)v->elems)[i * size];
  memmove(&((uint8_t *)v->elems)[(i + n) * size], p, (v->len - i) * size);
  v->len += n;

  return p;
}

/**
 * Removes elements in the vector.
 *
 * Calls abort(3) if there was an attempt to remove an element that does not
 * exist.
 *
 * @param v the vector to look up in.
 * @param size the size of the element type.
 * @param i index of the first element to remove.
 * @param n number of elements to remove.
 */
static inline void _vector_remove(Vector *v, size_t size, size_t i, size_t n) {
  if (i + n > v->len)
    _vector_abort();

  if (i + n != v->len)
    memmove(&((uint8_t *)v->elems)[i * size],
            &((const uint8_t *)v->elems)[(i + n) * size],
            (v->len - i - 1) * size);

  v->len -= n;
}

#endif /* XPPAUT_BASE_VECTOR_H */
