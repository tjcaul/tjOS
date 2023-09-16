#!/bin/zsh

thisdir=$(dirname $(basename $0))

cd $thisdir
gcc *.c -o ../bin/mkfont $@
