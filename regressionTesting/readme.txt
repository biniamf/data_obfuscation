This script is intended to run regression testing on the transformation.
The script relies on some naming conventions and directory structure.


1. FILE "regression.properties"

This file contains the environment where to run the tests, it defines the following variables:

REGRESSION_TRANSFORM the name of the program to run to transform source code. It assumes that a transformation overwrites the input code;

REGRESSION_DIRS: the (space separated) names of the directories where to find programs to transform, they are the test cases.

The script needs that each test directory contains a file describing the environment for the particular test case. This file is named "testcase.properties"


2. FILE  "testcase.properties"

This file defines several variables, specific for this directory:

TEST_SOURCES: the (space separated) list of source files to transform

TEST_BUILD: the command to run to build (compile) the test case

TEST_PROGRAM: the name of the executable to run (with optional flags), this is the result of the compilation

TEST_DATA: the file containing the input data to run the test case. For multiple runs, one input per line


3. ASSUMPTIONS

The testing script makes the following assumptions

- Test programs read input from the command line
- Test output is written in the console
- A return value different than 0 means that the test program encountered and error
