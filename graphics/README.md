# Installation #
Run "make" in this folder.

## Dependencies ##
### libPNG ###
Should be available in most package managers.

# Description #
This codebase is under development and is not meant to be run. 

At the current commit, it writes out images drawn by Tibia that it can identify from a set of images.


To make the program do this, do the following:

1. Create a folder in this directory called "tibia. Put the Tibia files (Tibia executable, Tibia.dat, Tibia.spr, etc.) there.
2. Create a folder in this directory called "foundSprites".
3. Get a hold of all the Tibia sprites using the crawler. Then run the formatSprites script in this directory. ("./formatSprites [srcPath] [destinationPath]"). Create a "spriteDatabase" folder in this directory and place the formatted sprites there.
3. Run "bin/Debug/GraphicsLayer" from this directory in the commandline.
4. You should be able to see images being written to in real-time in the "foundSprites" folder.

