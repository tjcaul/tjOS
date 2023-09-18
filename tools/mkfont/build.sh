#!/bin/zsh

cd "$(dirname $(realpath $0))"

gcc *.c -o ../bin/mkfont $@
