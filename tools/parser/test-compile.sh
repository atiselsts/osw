#!/bin/sh

DIR=build
OSW_PATH=../../..
ARCH=telosb

cd $DIR
for f in *.c
do
    cat > ./Makefile <<EOF
SOURCES = $f
APPMOD = App
PROJDIR = \$(CURDIR)
ifndef OSW
  OSW = \$(PROJDIR)/$OSW_PATH
endif
include \${OSW}/src/make/Makefile
EOF
    make clean && make $ARCH
done