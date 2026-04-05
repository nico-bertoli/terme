#!/bin/bash

# ====================================================== run example
if [ -f build/Release/terme_example ]; then
    ./build/Release/terme_example
elif [ -f build/terme_example ]; then
    ./build/terme_example
else
    echo "ERROR: build/terme_example executable not found"
    exit 1
fi
