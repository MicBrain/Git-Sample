
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include "beargit.h"
#include "util.h"

/* Implementation Notes:
 *
 * - Functions return 0 if successful, 1 if there is an error.
 * - All error conditions in the function description need to be implemented
 *   and written to stderr. We catch some additional errors for you in main.c.
 * - Output to stdout needs to be exactly as specified in the function description.
 *   * fs_cp(src,dst): copy file <src> to <dst>, overwriting <dst> if it exists
 *   * write_string_to_file(filename,str): write <str> to filename (overwriting contents)
 *   * read_string_from_file(filename,str,size): read a string of at most <size> (incl.
 *     NULL character) from file <filename> and store it into <str>. Note that <str>
 *     needs to be large enough to hold that string.
 *  - You NEED to test your code. The autograder we provide does not contain the
 *    full set of tests that we will run on your code. See "Step 5" in the homework spec.
 */

/* beargit init
 *
 * - Create .beargit directory
 * - Create empty .beargit/.index file
 * - Create .beargit/.prev file containing 0..0 commit id
 *
 * Output (to stdout):
 * - None if successful
 */
int beargit_init(void) {
  fs_mkdir(".beargit");
  FILE* findex = fopen(".beargit/.index", "w");
  fclose(findex);
  FILE* fbranches = fopen(".beargit/.branches", "w");
  fprintf(fbranches, "%s\n", "master");
  fclose(fbranches);
  write_string_to_file(".beargit/.prev", "0000000000000000000000000000000000000000");
  write_string_to_file(".beargit/.current_branch", "master");
  return 0;
}

/* beargit add <filename>
 * 
 * - Append filename to list in .beargit/.index if it isn't in there yet
 *
 * Possible errors (to stderr):
 * >> ERROR: File <filename> already added
 *
 * Output (to stdout):
 * - None if successful
 */
int beargit_add(const char* filename) {
  FILE* findex = fopen(".beargit/.index", "r");
  FILE *fnewindex = fopen(".beargit/.newindex", "w");
  char line[FILENAME_SIZE];
  while(fgets(line, sizeof(line), findex)) {
    strtok(line, "\n");
    if (strcmp(line, filename) == 0) {
      fprintf(stderr, "ERROR: File %s already added\n", filename);
      fclose(findex);
      fclose(fnewindex);
      fs_rm(".beargit/.newindex");
      return 3;
    }
    fprintf(fnewindex, "%s\n", line);
  }
  fprintf(fnewindex, "%s\n", filename);
  fclose(findex);
  fclose(fnewindex);
  fs_mv(".beargit/.newindex", ".beargit/.index");
  return 0;
}

/* beargit rm <filename>
 * 
 * See "Step 2" in the homework 1 spec.
 *
 */
int beargit_rm(const char* filename) {
 FILE* findex = fopen(".beargit/.index", "r");
 FILE *fnewindex = fopen(".beargit/.newindex", "w");
 char line[FILENAME_SIZE];
 int checker;
 checker = 0;
 while(fgets(line, sizeof(line), findex)) {
  strtok(line, "\n");
  if (strcmp(line, filename) != 0) {
    fprintf(fnewindex, "%s\n", line);
  } else {
    checker = 5;
  }
}
if (checker == 5) {
  fclose(findex);
  fclose(fnewindex);
  fs_mv(".beargit/.newindex", ".beargit/.index");
  return 0;
} else {
    fprintf(stderr, "ERROR: File %s not tracked\n", filename);
    fclose(findex);
    fclose(fnewindex);
    return 1;
  }
}

/* beargit commit -m <msg>
 *
 * See "Step 3" in the homework 1 spec.
 *
 */
const char* go_bears = "GO BEARS!";
int is_commit_msg_ok(const char* msg) {
  int pointMSG = 0, pointConst = 0;
  while (msg[pointMSG] != '\0') {
    if (go_bears[pointConst] == msg[pointMSG]) {
      pointConst ++;
      pointMSG ++;
    } else {
      if (pointMSG != 0 && pointConst != 0)
        pointConst = 0;
     else {
        pointConst = 0;
        pointMSG++;
      }
    }
    if (pointConst == 9) 
      return 5;
  }
  return 0;
}

void next_commit_id_hw1(char* commit_id) {
  char* final = commit_id;
  int checker = 1;
  while (*commit_id != '\0') {
    switch(*commit_id) {
      case '0':
        if (checker == 0)
          *commit_id = 'c';
        else if (checker == 1) {
          *commit_id = '6';
          checker = 0;
        }
        break;
      case 'c':
        if (checker == 0)
          *commit_id = 'c';
        else if (checker == 1) {
          *commit_id = '6';
          checker = 0;
        }
        break;
      case '1':
        if (checker == 0)
          *commit_id = '1';
        else if (checker == 1) {
          *commit_id = '6';
          checker = 0;
        }
        break;
      case '6':
        if (checker == 0)
          *commit_id = '6';
        else if (checker == 1) {
          *commit_id = 'c';
          checker = 1;
        }
        break;
      default: 
        break;
    }
    commit_id++;
  }
  commit_id = final;
}

int beargit_commit_hw1(const char* msg) {
  if (!is_commit_msg_ok(msg)) {
    fprintf(stderr, "ERROR: Message must contain \"%s\"\n", go_bears);
    return 1;
  } else {
    char commit_id[COMMIT_ID_SIZE];
    read_string_from_file(".beargit/.prev", commit_id, COMMIT_ID_SIZE);
    next_commit_id(commit_id);
    char new_directory[COMMIT_ID_SIZE + 15];
    sprintf(new_directory, "%s%s", ".beargit/", commit_id);
    fs_mkdir(new_directory);
    char index_directory[COMMIT_ID_SIZE + 25];
    sprintf(index_directory, "%s%s", new_directory, "/.index");
    fs_cp(".beargit/.index", index_directory);
    char prev_directory[COMMIT_ID_SIZE + 25];
    sprintf(prev_directory, "%s%s", new_directory, "/.prev");
    fs_cp(".beargit/.prev", prev_directory);
    FILE* index_file = fopen(".beargit/.index", "r");
    char name_of_file[FILENAME_SIZE];
    while (fgets(name_of_file, sizeof(name_of_file), index_file)) {
      strtok(name_of_file, "\n");
      char current_file_dir[COMMIT_ID_SIZE + 25];
      sprintf(current_file_dir, "%s/%s", new_directory, name_of_file);
      fs_cp(name_of_file, current_file_dir);
    }
    char msg_filename[COMMIT_ID_SIZE + 25];
    sprintf(msg_filename, "%s/%s", new_directory, ".msg");
    write_string_to_file(msg_filename, msg);
    write_string_to_file(".beargit/.prev", commit_id);
    fclose(index_file);
    return 0;
  } 
}

/* beargit status
 *
 * See "Step 1" in the homework 1 spec.
 *
 */
int beargit_status() {
  FILE* findex = fopen(".beargit/.index", "r");
  char line[FILENAME_SIZE];
  int counter = 0;
  fprintf(stdout, "%s\n", "Tracked files:");
  while (fgets(line, sizeof(line), findex)) {
    counter++;
    strtok(line, "\n");
    printf("\n");
    fprintf(stdout, "%s", "  ");
    fprintf(stdout, "%s", line);
  }
  fprintf(stdout, "\n\n");
  fprintf(stdout, "%d", counter);
  fprintf(stdout, "%s", " ");
  fprintf(stdout, "%s\n", "files total");
  fclose(findex);
  return 0;
}

/* beargit log
 *
 * See "Step 4" in the homework 1 spec.
 *
 */
int beargit_log() {
  char last_commit[COMMIT_ID_SIZE]; 
  int index = 0;
  int checker = 10; 
  read_string_from_file(".beargit/.prev", last_commit, COMMIT_ID_SIZE);
  while (strcmp(last_commit, "0000000000000000000000000000000000000000") != 0) {
    char content[100];
    fprintf(stdout, "\ncommit %s", last_commit);
    checker = 50;
    sprintf(content, "%s%s%s", ".beargit/", last_commit, "/.msg");
    read_string_from_file(content, content, 120);
    char lastSource[100];
    fprintf(stdout, "\n    %s\n", content);
    sprintf(lastSource, "%s%s%s", ".beargit/", last_commit,  "/.prev");
    read_string_from_file(lastSource, last_commit, COMMIT_ID_SIZE);
  }
  if (checker == 10) {
    fprintf(stderr, "ERROR: There are no commits!\n");
    return 1;
  }
  if (checker == 50)
    fprintf(stdout, "\n");
  return 0;
}


// ---------------------------------------
// HOMEWORK 2 
// ---------------------------------------
// This adds a check to beargit_commit that ensures we are on the HEAD of a branch.

int beargit_commit(const char* msg) {
  char current_branch[BRANCHNAME_SIZE];
  read_string_from_file(".beargit/.current_branch", current_branch, BRANCHNAME_SIZE);
  if (strlen(current_branch) == 0) {
    fprintf(stderr, "ERROR: Need to be on HEAD of a branch to commit\n");
    return 1;
  }
  return beargit_commit_hw1(msg);
}

const char* digits = "61c";

void next_commit_id(char* commit_id) {
  char current_branch[BRANCHNAME_SIZE];
  read_string_from_file(".beargit/.current_branch", current_branch, BRANCHNAME_SIZE);
  // The first COMMIT_ID_BRANCH_BYTES=10 characters of the commit ID will
  // be used to encode the current branch number. This is necessary to avoid
  // duplicate IDs in different branches, as they can have the same pre-
  // decessor (so next_commit_id has to depend on something else).
  int n = get_branch_number(current_branch);
  for (int i = 0; i < COMMIT_ID_BRANCH_BYTES; i++) {
    // This translates the branch number into base 3 and substitutes 0 for
    // 6, 1 for 1 and c for 2.
    commit_id[i] = digits[n%3];
    n /= 3;
  }
  // Use next_commit_id to fill in the rest of the commit ID.
  next_commit_id_hw1(commit_id + COMMIT_ID_BRANCH_BYTES);
}

// This helper function returns the branch number for a specific branch, or
// returns -1 if the branch does not exist.
int get_branch_number(const char* branch_name) {
  FILE* fbranches = fopen(".beargit/.branches", "r");
  int branch_index = -1;
  int counter = 0;
  char line[FILENAME_SIZE];
  while(fgets(line, sizeof(line), fbranches)) {
    strtok(line, "\n");
    if (strcmp(line, branch_name) == 0) {
      branch_index = counter;
    }
    counter++;
  }
  fclose(fbranches);
  return branch_index;
}

/* beargit branch
 *
 * See "Step 6" in the homework 1 spec.
 *
 */
int beargit_branch() {
  char current_branch[BRANCHNAME_SIZE];
  read_string_from_file(".beargit/.current_branch", current_branch, BRANCHNAME_SIZE);
  FILE* findex = fopen(".beargit/.branches", "r");
  char line[BRANCHNAME_SIZE];
  while (fgets(line, sizeof(line), findex)) {
    strtok(line, "\n");
    if (strcmp(line, current_branch) != 0) {
      fprintf(stdout, "%s", "  ");
      fprintf(stdout, "%s\n", line);
    } else {
      fprintf(stdout, "%s", "* ");
      fprintf(stdout, "%s\n", line);
    }
  }
  fclose(findex);
  return 0;
}
/* beargit checkout
 *
 * See "Step 7" in the homework 1 spec.
 *
 */
int checkout_commit(const char* commit_id) {
  if (strcmp(commit_id, "0000000000000000000000000000000000000000") == 0) {
    write_string_to_file(".beargit/.index", "");
  } else {
    FILE* findex = fopen(".beargit/.index", "r");
    char line[BRANCHNAME_SIZE];
    while(fgets(line, sizeof(line), findex)) {
      strtok(line, "\n");
      fs_rm(line);
    }
    fclose(findex);
    char init_loc[150];
    sprintf(init_loc, "%s%s%s", ".beargit/", commit_id, "/.index");
    FILE* last = fopen(init_loc, "r");
    char new_line[BRANCHNAME_SIZE];
    while(fgets(new_line, sizeof(new_line), last)) {
      strtok(new_line, "\n");
      char final_place[200];
      sprintf(final_place, ".beargit/%s/%s", commit_id, new_line);
      fs_cp(final_place, new_line);
    }
    fs_cp(init_loc, ".beargit/.index");
    fclose(last);
  }
  write_string_to_file(".beargit/.prev", commit_id);
  return 0;
}

int is_it_a_commit_id(const char* commit_id) {
  if (strlen(commit_id) != 40) {
    return 0;
  } else {
    while (*commit_id !='\0') {
      if (*commit_id != '6' && *commit_id != '1' && *commit_id != 'c') {
        return 0;
      }
      commit_id++;
    }
    return 1;
  }
}

int beargit_checkout(const char* arg, int new_branch) {
  char current_branch[BRANCHNAME_SIZE];
  read_string_from_file(".beargit/.current_branch", current_branch, BRANCHNAME_SIZE);
  if (strlen(current_branch)) {
    char current_branch_file[BRANCHNAME_SIZE+100];
    sprintf(current_branch_file, ".beargit/.branch_%s", current_branch);
    fs_cp(".beargit/.prev", current_branch_file);
  }
  if (is_it_a_commit_id(arg)) {
    char commit_dir[FILENAME_SIZE + 100] = ".beargit/";
    strcat(commit_dir, arg);
    if (!fs_check_dir_exists(commit_dir)) {
      fprintf(stderr, "ERROR: Commit %s does not exist\n", arg);
      return 1;
    }
    write_string_to_file(".beargit/.current_branch", "");
    return checkout_commit(arg);
  }
  const char* branch_name = arg;
  int branch_exists = (get_branch_number(branch_name) >= 0);
  if (branch_exists && new_branch) {
    fprintf(stderr, "ERROR: A branch named %s already exists\n", branch_name);
    return 1;
  } else if (!branch_exists && !new_branch) {
    fprintf(stderr, "ERROR: No branch %s exists\n", branch_name);
    return 1;
  }
  char branch_file[FILENAME_SIZE] = ".beargit/.branch_";
  strcat(branch_file, branch_name);
  if (new_branch) {
    FILE* fbranches = fopen(".beargit/.branches", "a");
    fprintf(fbranches, "%s\n", branch_name);
    fclose(fbranches);
    fs_cp(".beargit/.prev", branch_file); 
  }
  write_string_to_file(".beargit/.current_branch", branch_name);
  char branch_head_commit_id[COMMIT_ID_SIZE];
  read_string_from_file(branch_file, branch_head_commit_id, COMMIT_ID_SIZE);
  return checkout_commit(branch_head_commit_id);
}

