#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

const char * file_stdout = "TEST_STDOUT";
const char * file_stderr = "TEST_STDERR";

int fake_print(char* fmt, ...) {
    // append to file
    char data[2048]; // if your line is longer than this, you're doing something wrong
    va_list args;
    va_start(args, fmt);
    vsprintf(data, fmt, args);
    va_end(args);
    FILE *fp = fopen(file_stdout, "a");
    if (fp != NULL) {
        fputs(data, fp);
        fclose(fp);
    }
    return 0;
}

int fake_fprint(FILE* stream, char* fmt, ...) {
    // append to file
    char data[2048]; // if your line is longer than this, you're doing something wrong
    va_list args;
    const char * filename_to_open;
    if (stream == stdout) {
        filename_to_open = file_stdout;
    } else if (stream == stderr) {
        filename_to_open = file_stderr;
    } else {
        va_start(args, fmt);
        vfprintf(stream, fmt, args);
        va_end(args);
        return 0;
    }
    va_start(args, fmt);
    vsprintf(data, fmt, args);
    va_end(args);
    FILE *fp = fopen(filename_to_open, "a");
    if (fp != NULL) {
        fputs(data, fp);
        fclose(fp);
    }
    return 0;
}

int is_sane_path(const char* path) {
  if (strlen(path) > 512)
    return 0;

  // Only allow modifying files in .beargit directory
  //const char* prefix = ".beargit";
  //if (strncmp(prefix, path, strlen(prefix)) != 0)
  //  return 0;

  return 1;
}
