#!/bin/bash

# build the project
cd build/
cmake ..
make

# run tests
./run_tests
cd ..