#!/bin/bash

# TODO: pick some number of cores based on /proc/cpuinfo or so
(cd extern/premake-4.3/build/gmake.unix && make -j5)
PREMAKE=./bin/release/premake4

# set up the build env now
$PREMAKE gmake

# set up our submodules
git submodule init
git submodule update
(cd extern/glfw3 && cmake .)
: ${PYTHON:=$(type -p python2)} ${PYTHON:=$(type -p python)} ${PYTHON:?python interpreter not found}
(cd extern/glxw && "$PYTHON" glxw_gen.py)