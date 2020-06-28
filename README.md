## NOTE: This implementation is out of date and will be updated. 
There is a major bug that occurrs when non-square windows are created (height != width). This bug has been fixed by padding the root quadtree node to make the dimenions equal and of a power of 2.

# Author: 
Chris Eckhardt
# Description: 
This is my implementation of a linear quadtree. It's intended purpose is to be used in a GUI window management application to handle z-order of overlapping windows. 

# Glossary:
    # window.c / window.h:
        contains quadtree implementaion
    # test.c:
        contains main function and CUnit test
    # data.csv:
        contains values used for testing

# build:
    CUnit must be linked to the program after compilation, to get CUnit working on mac using brew type "brew install cunit" then "brew link cunit"

Make commands:
    make - compiles the program and links cunit
    make clean - removes .o files and run executable

# run:
    ./run

# to review test results:
    see 'TestResults-Results.xml' for CUnit testing
