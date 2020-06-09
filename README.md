Glossary:
    window.c / window.h:
        contains quadtree implementaion
    test.c:
        contains main function and CUnit test
    data.csv:
        contains values used for testing

To build:
    CUnit must be linked to the program after compilation, to get CUnit working on mac using brew type "brew install cunit" then "brew link cunit"

    Make commands: 
        make - compiles the program and links cunit
        make clean - removes .o files and run executable

to run:
    ./run

to review test results:
    see 'TestResults-Results.xml' for CUnit testing