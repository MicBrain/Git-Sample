/**
 * This file contains utility functions to use in the homework. Do not modify!
 */
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>

int fake_print(char* fmt, ...);
int fake_fprint(FILE* stream, char* fmt, ...);
int is_sane_path(const char* path);

/* In testing mode (initialized with -DTESTING fed to gcc and done automatically
 * when you run make beargit-unittest), we need to replace printf and fprintf 
 * with "fakes" that redirect your output to files, which you can read from 
 * in your testing code.
 */
#ifdef TESTING
#define printf fake_print
#define fprintf fake_fprint
#endif

static const char* path = "";
static const char* dirname = "";
static const char* filename = "";
static const char* src = "";
static const char* dst = "";

#define PRINT_FILENAME(fn) \
  if (strlen(fn) > 0) \
    fprintf(stderr, "  * %s = %s\n", # fn, fn);

#define ASSERT_ERROR_MESSAGE(fn, msg) \
  if (!(fn)) { \
    fprintf(stderr, "\n        *** AN ERROR OCCURRED IN %s ***\n\n", __func__); \
    fprintf(stderr, "  This should not happen. Most likely, you tried to access\n"); \
    fprintf(stderr, "  or manipulate a file that doesn't exist, or create one\n"); \
    fprintf(stderr, "  that already does. You should first try to delete the\n"); \
    fprintf(stderr, "  .beargit directory and start over; maybe a previous run\n"); \
    fprintf(stderr, "  corrupted it. If not, you need to fix the problem in\n"); \
    fprintf(stderr, "  your program -- the following error message might give\n"); \
    fprintf(stderr, "  more information about what went wrong:\n\n"); \
    fprintf(stderr, "  Error in %s: %s\n\n", __func__, msg); \
    fprintf(stderr, "  Non-empty variables in scope:\n"); \
    PRINT_FILENAME(path) \
    PRINT_FILENAME(dirname) \
    PRINT_FILENAME(filename) \
    PRINT_FILENAME(src) \
    PRINT_FILENAME(dst) \
    fprintf(stderr, "\n"); \
    exit(1); \
  }

static void fs_mkdir(const char* dirname) {
  ASSERT_ERROR_MESSAGE(dirname != NULL, "dirname is not a valid string");
  ASSERT_ERROR_MESSAGE(is_sane_path(dirname), "dirname is not a valid path within .beargit");
  int ret = mkdir(dirname, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  ASSERT_ERROR_MESSAGE(ret == 0, "creating directory failed");
}

static void fs_rm(const char* filename) {
  ASSERT_ERROR_MESSAGE(filename != NULL, "filename is not a valid string");
  ASSERT_ERROR_MESSAGE(is_sane_path(filename), "filename is not a valid path within .beargit");
  int ret = unlink(filename);
  ASSERT_ERROR_MESSAGE(ret == 0, "deleting/unlinking file failed");
}

static void fs_force_rm_beargit_dir() {
  // BAD HACK. Don't use this in real-world code.
  // This removes the .beargit directory and directs all output to /dev/null
  system("rm -rf .beargit");
}

static void fs_mv(const char* src, const char* dst) {
  ASSERT_ERROR_MESSAGE(src != NULL, "src is not a valid string");
  ASSERT_ERROR_MESSAGE(dst != NULL, "dst is not a valid string");
  ASSERT_ERROR_MESSAGE(is_sane_path(src), "src is not a valid path within .beargit");
  ASSERT_ERROR_MESSAGE(is_sane_path(dst), "dst is not a valid path within .beargit");
  int ret = rename(src, dst);
  ASSERT_ERROR_MESSAGE(ret == 0, "renaming file failed");
}

static void fs_cp(const char* src, const char* dst) {
  ASSERT_ERROR_MESSAGE(src != NULL, "src is not a valid string");
  ASSERT_ERROR_MESSAGE(dst != NULL, "dst is not a valid string");
  ASSERT_ERROR_MESSAGE(is_sane_path(dst), "dst is not a valid path within .beargit");

  FILE* fin = fopen(src, "r");
  ASSERT_ERROR_MESSAGE(fin != NULL, "couldn't open source file");
  FILE* fout = fopen(dst, "w");
  ASSERT_ERROR_MESSAGE(fout != NULL, "couldn't open destination file");

  char buffer[4096];
  int size;

  while ((size = fread(buffer, 1, 4096, fin)) > 0) {
    fwrite(buffer, 1, size, fout);
  }

  fclose(fin);
  fclose(fout);
}

static void write_string_to_file(const char* filename, const char* str) {
  FILE* fout = fopen(filename, "w");
  ASSERT_ERROR_MESSAGE(fout != NULL, "couldn't open file");
  fwrite(str, 1, strlen(str)+1, fout);
  fclose(fout);
}

static void read_string_from_file(const char* filename, char* str, int size) {
  FILE* fin = fopen(filename, "r");
  ASSERT_ERROR_MESSAGE(fin != NULL, "couldn't open file");
  fread(str, 1, size, fin);
  fclose(fin);
}

static int fs_check_dir_exists(const char* dirname) {
  struct stat s;
  int ret_code = stat(dirname, &s);
  return !(ret_code == -1 || !(S_ISDIR(s.st_mode)));
}
