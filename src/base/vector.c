#include "vector.h"

#include <stdlib.h>

int _vector_init(Vector *v, size_t size, size_t cap) {
  if (cap) {
    v->elems = malloc(size * cap);
    if (!v->elems)
      return 1;

    v->cap = cap;
  } else {
    v->elems = NULL;
    v->cap = 0;
  }
  v->len = 0;

  return 0;
}

void _vector_clean(Vector *v) {
  free(v->elems);
  v->elems = NULL;
  v->len = v->cap = 0;
}

void _vector_abort(void) {
  abort();
}

int _vector_ensure(Vector *v, size_t size, size_t n) {
  size_t cap = v->cap;

  while (n >= cap)
    cap = cap ? 2 * cap : 2;

  void *elems = realloc(v->elems, size * cap);
  if (!elems)
    return 1;

  v->elems = elems;
  v->cap = cap;

  return 0;
}
