#!/bin/bash

THIS_DIR=`readlink -f "${BASH_SOURCE[0]}" 2>/dev/null||echo $0`
DIR=`dirname "${THIS_DIR}"`

# Default installation path
TMP_DIR=~/mylogger
INSTALL_DIR=
if [ ! -z "$1" ]; then
    INSTALL_DIR=$1
else
    INSTALL_DIR=$TMP_DIR
fi

cd "$DIR"
cd ../
make logger

echo "Installing MyLogger to $INSTALL_DIR ..."
mkdir -p $INSTALL_DIR
cp ./libmylogger.a $INSTALL_DIR/
cp -R ./include/ $INSTALL_DIR
rm libmylogger.a
rm src/mylogger.o

echo "DONE"