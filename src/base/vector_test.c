#include "vector.h"

#include <stdlib.h>
#include <check.h>

VECTOR_DECLARE(intvec, IntVec, int)
VECTOR_DEFINE(intvec, IntVec, int)

START_TEST(test_static_init) {
  IntVec v = VECTOR_INIT;

  ck_assert_int_eq(v.len, 0);
  ck_assert_int_eq(v.cap, 0);
}
END_TEST

START_TEST(test_init_clean) {
  IntVec v;

  ck_assert_int_eq(intvec_init(&v, 0), 0);
  ck_assert_int_eq(v.len, 0);
  ck_assert_int_eq(v.cap, 0);
  intvec_clean(&v);

  ck_assert_int_eq(intvec_init(&v, 10), 0);
  ck_assert_ptr_ne(v.elems, NULL);
  ck_assert_int_eq(v.len, 0);
  ck_assert_int_eq(v.cap, 10);
  intvec_clean(&v);
}
END_TEST

START_TEST(test_append) {
  IntVec v = VECTOR_INIT;

  ck_assert_ptr_ne(intvec_append(&v), NULL);
  ck_assert_int_eq(v.len, 1);
  ck_assert_int_ge(v.cap, 1);
  intvec_clean(&v);
}
END_TEST

START_TEST(test_insert) {
  IntVec v = VECTOR_INIT;
  int *pv = intvec_insert(&v, 0, 3);

  ck_assert_ptr_ne(pv, NULL);
  *pv = 42;
  ck_assert_int_eq(v.len, 3);
  ck_assert_int_ge(v.cap, 3);

  /* Insert at head. */
  ck_assert_ptr_ne(intvec_insert(&v, 0, 2), NULL);

  ck_assert_int_eq(v.len, 5);
  ck_assert_int_ge(v.cap, 5);
  ck_assert_int_eq(v.elems[2], 42);
  intvec_clean(&v);
}
END_TEST

START_TEST(test_remove) {
  IntVec v = VECTOR_INIT;

  ck_assert_ptr_ne(intvec_append(&v), NULL);
  *intvec_append(&v) = 42;
  intvec_remove(&v, 0, 1);

  ck_assert_int_eq(v.len, 1);
  ck_assert_int_eq(v.elems[0], 42);
  intvec_clean(&v);
}
END_TEST

Suite *vector_suite(void) {
  Suite *ret = suite_create("vector");
  TCase *tc = tcase_create("Core");

  tcase_add_test(tc, test_static_init);
  tcase_add_test(tc, test_init_clean);
  tcase_add_test(tc, test_append);
  tcase_add_test(tc, test_insert);
  tcase_add_test(tc, test_remove);
  suite_add_tcase(ret, tc);

  return ret;
}

int main() {
  SRunner *sr = srunner_create(vector_suite());

  srunner_set_tap(sr, "-");
  srunner_run_all(sr, CK_NORMAL);
  int nf = srunner_ntests_failed(sr);
  srunner_free(sr);

  return nf ? EXIT_FAILURE : EXIT_SUCCESS;
}
