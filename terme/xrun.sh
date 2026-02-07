#!/bin/bash

# ====================================================== build with test executable
./xbuild.sh --test

# ====================================================== run test
cd build
if [ -f Release/terme_test ]; then
    ./Release/terme_test
elif [ -f terme_test ]; then
    ./terme_test
else
    echo "ERROR: terme_test executable not found"
    exit 1
fi
