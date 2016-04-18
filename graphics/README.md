# Installation #
Run "make" in this folder.

## Dependencies ##
### libPNG ###
Should be available in most package managers.

# Description #
This codebase is under development and is not meant to be run. 

At the current commit, it writes out everything Tibia draws into ppm files. However, the program is unstable due to pipe capacity limitations. I plan on using shared memory instead.


To make the program draw the ppm files, do the following:

1. Create a folder in this directory called "tibia. Put the Tibia files (Tibia executable, Tibia.dat, Tibia.spr, etc.) there.
2. Create a folder in this directory called "drawnTiles".
3. Run "bin/Debug/GraphicsLayer" from this directory in the commandline.
4. You should be able to see images being written to in real-time in the "drawnTiles" folder.
