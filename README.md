# Git Sample (Beargit)

Beargit is an implementation of a distributed revision control system for managing source code and other files. 

At its core, beargit is essentially a simpler version of git. It maintains a .beargit/ subdirectory containing information about your repository.  For each commit that the user makes, a directory is created inside the .beargit/ directory (.beargit/<ID>, where <ID> is the ID of the commit). The .beargit/ directory additionally contains two files: .index

beargit can track individual files in the current working directory (no subdirectories!).
