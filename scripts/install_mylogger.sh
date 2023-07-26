#!/bin/bash

# Default installation path
TMP_DIR=~/mylogger
INSTALL_DIR=
if [ -n "$1" ]; then
    INSTALL_DIR=$1
else
    INSTALL_DIR=$TMP_DIR
fi

make logger

echo "Installing MyLogger to $INSTALL_DIR ..."
mkdir -p $INSTALL_DIR
cp ./libmylogger.a $INSTALL_DIR/
cp -R ./include/ $INSTALL_DIR
rm libmylogger.a
rm src/mylogger.o

echo "DONE"