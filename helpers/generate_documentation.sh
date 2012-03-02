#!/bin/sh

cd ../docs
doxygen Doxyfile.dox
cd html
qhelpgenerator index.qhp
