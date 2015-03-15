# Git Sample (Beargit)

1. Introduction
2. Description
3. Differences between beargit and git
4. Files


Beargit is an implementation of a distributed revision control system for managing source code and other files. 

At its core, beargit is essentially a simpler version of git. It maintains a .beargit/ subdirectory containing information about your repository.  For each commit that the user makes, a directory is created inside the .beargit/ directory (.beargit/<ID>, where <ID> is the ID of the commit). The .beargit/ directory additionally contains two files: .index (a list of all currently tracked files, one per line, no duplicates) and .prev (contains the ID of the last commit, or 0..0 if there is no commit yet).  Each .beargit/<ID> directory contains a copy of each tracked file (as well as the .index file) at the time of the commit, a .msg file that contains the commit message (one line) and a .prev file that contains the commit ID of the previous commit.

beargit can track individual files in the current working directory (no subdirectories!).

### Differences between beargit and git

   There are some key differences between this version of distributed revision system and git:
* The only supported commands are init, add, rm, commit, status and log. For each of them, only the most basic command line options are supported.
* beargit does not track diffs between files. Instead, each time you make a commit, it simply copies all files that are being tracked into the .beargit/<ID> directory (where <ID> is the commit ID).
* Commit IDs are not based on cryptographic hash functions, but instead are a fixed sequence of 40-character strings that only contain '6', '1' and 'c'.
* Any commits with a commit message that does not contain "GO BEARS!" (with exactly this capitalization and spelling) will be rejected with an error message.
* No user, date or other additional information is tracked by beargit. It does not allow to track subdirectories, or files starting with '.'.
* The rm command only causes beargit to stop tracking a file, but does not delete it from the file system.

### Files
   Here is the description of implemented files:
* beargit.c  - This is the file that contains the implementation of beargit.
* beargit.h - This file contains declarations of various constructs in beargit.c along with convenient #defines.
* main.c - Contains the main for beargit (which parses command line options and calls into the functions defined in beargit.c).
* Makefile - This tells the program make how to build your code when you run the make command.
* util.h - Contains helper functions that were used when completing the project.
