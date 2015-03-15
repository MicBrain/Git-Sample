# Git Sample (Beargit)

1. Introduction
2. Description
3. Differences between beargit and git


Beargit is an implementation of a distributed revision control system for managing source code and other files. 

At its core, beargit is essentially a simpler version of git. It maintains a .beargit/ subdirectory containing information about your repository.  For each commit that the user makes, a directory is created inside the .beargit/ directory (.beargit/<ID>, where <ID> is the ID of the commit). The .beargit/ directory additionally contains two files: .index (a list of all currently tracked files, one per line, no duplicates) and .prev (contains the ID of the last commit, or 0..0 if there is no commit yet).  Each .beargit/<ID> directory contains a copy of each tracked file (as well as the .index file) at the time of the commit, a .msg file that contains the commit message (one line) and a .prev file that contains the commit ID of the previous commit.

beargit can track individual files in the current working directory (no subdirectories!).

### Differences between beargit and git

   There are some key differences between this version of distributed revision system and git:
* The only supported commands are init, add, rm, commit, status and log. For each of them, only the most basic command line options are supported.
* beargit does not track diffs between files. Instead, each time you make a commit, it simply copies all files that are being tracked into the .beargit/<ID> directory (where <ID> is the commit ID).
   
