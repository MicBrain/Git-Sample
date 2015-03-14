#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <CUnit/Basic.h>
#include "beargit.h"
#include "util.h"

#undef printf
#undef fprintf

/* The suite initialization function.
 * You'll probably want to delete any leftover files in .beargit from previous
 * tests, along with the .beargit directory itself.
 *
 * You'll most likely be able to share this across suites.
 */
int init_suite(void)
{
    // preps to run tests by deleting the .beargit directory if it exists
    fs_force_rm_beargit_dir();
    unlink("TEST_STDOUT");
    unlink("TEST_STDERR");
    return 0;
}

/* You can also delete leftover files after a test suite runs, but there's
 * no need to duplicate code between this and init_suite 
 */
int clean_suite(void)
{
    return 0;
}

/* Simple test of fread().
 * Reads the data previously written by testFPRINTF()
 * and checks whether the expected characters are present.
 * Must be run after testFPRINTF().
 */
void simple_sample_test(void)
{
    // This is a very basic test. Your tests should likely do more than this.
    // We suggest checking the outputs of printfs/fprintfs to both stdout
    // and stderr. To make this convenient for you, the tester replaces
    // printf and fprintf with copies that write data to a file for you
    // to access. To access all output written to stdout, you can read 
    // from the "TEST_STDOUT" file. To access all output written to stderr,
    // you can read from the "TEST_STDERR" file.
    int retval;
    retval = beargit_init();
    CU_ASSERT(0==retval);
    retval = beargit_add("asdf.txt");
    CU_ASSERT(0==retval);
}

struct commit {
  char msg[MSG_SIZE];
  struct commit* next;
};


void free_commit_list(struct commit** commit_list) {
  if (*commit_list) {
    free_commit_list(&((*commit_list)->next));
    free(*commit_list);
  }

  *commit_list = NULL;
}

void run_commit(struct commit** commit_list, const char* msg) {
    int retval = beargit_commit(msg);
    CU_ASSERT(0==retval);

    struct commit* new_commit = (struct commit*)malloc(sizeof(struct commit));
    new_commit->next = *commit_list;
    strcpy(new_commit->msg, msg);
    *commit_list = new_commit;
}

void simple_log_test(void)
{
    struct commit* commit_list = NULL;
    int retval;
    retval = beargit_init();
    CU_ASSERT(0==retval);
    FILE* asdf = fopen("asdf.txt", "w");
    fclose(asdf);
    retval = beargit_add("asdf.txt");
    CU_ASSERT(0==retval);
    run_commit(&commit_list, "GO BEARS!1");
    run_commit(&commit_list, "GO BEARS!2");
    run_commit(&commit_list, "GO BEARS!3");

    retval = beargit_log();
    CU_ASSERT(0==retval);

    struct commit* cur_commit = commit_list;

    const int LINE_SIZE = 512;
    char line[LINE_SIZE];

    FILE* fstdout = fopen("TEST_STDOUT", "r");
    CU_ASSERT_PTR_NOT_NULL(fstdout);

    while (cur_commit != NULL) {
      char refline[LINE_SIZE];

      // First line is empty
      CU_ASSERT_PTR_NOT_NULL(fgets(line, LINE_SIZE, fstdout));
      CU_ASSERT(!strcmp(line,"\n"));

      // Second line is commit -- don't check the ID.
      CU_ASSERT_PTR_NOT_NULL(fgets(line, LINE_SIZE, fstdout));
      CU_ASSERT(!strncmp(line,"commit", strlen("commit")));

      // Third line is msg
      sprintf(refline, "    %s\n", cur_commit->msg);
      CU_ASSERT_PTR_NOT_NULL(fgets(line, LINE_SIZE, fstdout));
      CU_ASSERT_STRING_EQUAL(line, refline);

      cur_commit = cur_commit->next;
    }

    // Last line is empty
    CU_ASSERT_PTR_NOT_NULL(fgets(line, LINE_SIZE, fstdout));
    CU_ASSERT(!strcmp(line,"\n"));

    CU_ASSERT_PTR_NULL(fgets(line, LINE_SIZE, fstdout));

    // It's the end of output
    CU_ASSERT(feof(fstdout));
    fclose(fstdout);

    free_commit_list(&commit_list);
}

// Advanced test for add function
void first_test(void) {
    int eval;
    eval = beargit_init();
    CU_ASSERT(0==eval);
    eval = beargit_add("foo.txt");
    CU_ASSERT(0==eval);
    int doom;
    doom = beargit_add("add.txt");
    CU_ASSERT(0==doom);
    int coom;
    coom = beargit_add("add.txt");
    CU_ASSERT(0!=coom);
    int final;
    final = beargit_add("last.txt");
    CU_ASSERT(0==final);
}

// Advanced test for remove function
void second_test(void) {
  int first;
  first = beargit_init();
  CU_ASSERT(0==first);
  int second;
  second = beargit_add("foo.txt");
  int third;
  third = beargit_add("add.txt");
  int forth;
  forth = beargit_rm("foo.txt");
  CU_ASSERT(0==forth);
  int fifth;
  fifth = beargit_rm("foo.txt");
  CU_ASSERT(0!=fifth);
}

// Advanced test for status function
void third_test(void) {
  int val;
  val = beargit_init();
  CU_ASSERT(0==val);
  int a;
  a = beargit_add("foo.txt");
  int b;
  b = beargit_add("boo.txt");
  int c;
  c = beargit_add("zoo.txt");
  int d;
  d = beargit_status();
  CU_ASSERT(0==d);
}

// Advanced test for commit function
void forth_test(void)
{
  int foo;
  foo = beargit_init();
  int doo;
  doo = beargit_commit("");
  CU_ASSERT(1 == doo);
  int zoo;
  zoo = beargit_commit("GOOOOOOOOO");
  CU_ASSERT(1==zoo);
  int loo;
  loo = beargit_commit("GO BEARS");
  CU_ASSERT(loo==1);
  int final;
  final = beargit_commit("GO BEARS!");
  CU_ASSERT(final == 0);
  int last;
  last = beargit_commit("dgf GO BEARS!");
  CU_ASSERT(last == 0);
}

// Advanced test for log function
void fifth_test(void) {
  int a;
  a = beargit_init();
  int b;
  b = beargit_log();
  CU_ASSERT(b == 1);
  int loo;
  loo = beargit_commit("GO BEARS");
  int c;
  c = beargit_log();
  CU_ASSERT(c==1);
  int last;
  last = beargit_commit("dgf GO BEARS!");
  int d;
  d = beargit_log();
  CU_ASSERT(d == 0);
  int final;
  int blast;
  final = beargit_commit("FDfg sdgf GO BEARS!");
  blast = beargit_commit("GO BEARS! fdgfsgs");
  int e;
  e = beargit_log();
  CU_ASSERT(e == 0);
}

// Advanced test for branch function
void sixth_test(void) {
  int foo;
  foo = beargit_init();
  int final;
  int boo;
  final = beargit_commit("FDfg sdgf GO BEARS!");
  boo = beargit_commit("GO BEARS! fdgfsgs");
  int k;
  k = beargit_branch();
  CU_ASSERT(0==k);
  int z;
  z = beargit_branch();
  CU_ASSERT(z == 0);
}

/* The main() function for setting up and running the tests.
 * Returns a CUE_SUCCESS on successful running, another
 * CUnit error code on failure.
 */
int cunittester()
{
   CU_pSuite pSuite = NULL;
   CU_pSuite pSuite2 = NULL;
   CU_pSuite pSuite3 = NULL;
   CU_pSuite pSuite4 = NULL;
   CU_pSuite pSuite5 = NULL;
   CU_pSuite pSuite6 = NULL;
   CU_pSuite pSuite7 = NULL;
   CU_pSuite pSuite8 = NULL;

   /* initialize the CUnit test registry */
   if (CUE_SUCCESS != CU_initialize_registry())
      return CU_get_error();

   /* add a suite to the registry */
   pSuite = CU_add_suite("Suite_1", init_suite, clean_suite);
   if (NULL == pSuite) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* Add tests to the Suite #1 */
   if (NULL == CU_add_test(pSuite, "Simple Test #1", simple_sample_test))
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

   pSuite2 = CU_add_suite("Suite_2", init_suite, clean_suite);
   if (NULL == pSuite2) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   if (NULL == CU_add_test(pSuite2, "Log output test", simple_log_test))
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

   pSuite3 = CU_add_suite("Extra-Suite_1", init_suite, clean_suite);
   if (NULL == pSuite3) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   if (NULL == CU_add_test(pSuite3, "Advanced test for add function", first_test))
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

   pSuite4 = CU_add_suite("Extra-Suite_2", init_suite, clean_suite);
   if (NULL == pSuite4) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   if (NULL == CU_add_test(pSuite4, "Advanced test for remove function", second_test))
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

   pSuite5 = CU_add_suite("Extra-Suite_3", init_suite, clean_suite);
   if (NULL == pSuite5) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   if (NULL == CU_add_test(pSuite5, "Advanced test for status function", third_test))
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

   pSuite6 = CU_add_suite("Extra-Suite_4", init_suite, clean_suite);
   if (NULL == pSuite6) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   if (NULL == CU_add_test(pSuite6, "Advanced test for commit function", forth_test))
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

   pSuite7 = CU_add_suite("Extra-Suite_5", init_suite, clean_suite);
   if (NULL == pSuite7) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   if (NULL == CU_add_test(pSuite7, "Advanced test for log function", fifth_test))
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

   pSuite7 = CU_add_suite("Extra-Suite_6", init_suite, clean_suite);
   if (NULL == pSuite7) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   if (NULL == CU_add_test(pSuite7, "Advanced test for branch function", sixth_test))
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

   CU_basic_set_mode(CU_BRM_VERBOSE);
   CU_basic_run_tests();
   CU_cleanup_registry();
   return CU_get_error();
}
