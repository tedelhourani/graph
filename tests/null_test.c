#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <setjmp.h>
#include <cmocka.h>

/* Initialize and clean up resources after each test run */
int setup (void ** state) {
  printf("Starting\n");
  return 0;
}

int teardown (void ** state) {
  printf("Done\n");
  return 0;
}


/* A test case that does nothing and succeeds. */
static void null_test_success(void **state) {
  (void) state; /* unused */
}
static void null_null_test_success(void **state) {
  (void) state; /* unused */
  /*assert_true (0);*/
}


int main(void) {
  int failed_test_count;

  const struct CMUnitTest tests[] =
    {
     cmocka_unit_test(null_test_success),
     cmocka_unit_test(null_null_test_success),
    };

  failed_test_count = cmocka_run_group_tests(tests, NULL, NULL);
  return failed_test_count;
}
