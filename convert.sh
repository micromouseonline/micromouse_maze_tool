#! /bin/bash
# Convert all the binary mazes listed on the command line
# into text mazes and mazes in C files
# TODO generate a C header file for the c mazes

TEXT_DIR=mazefiles/text
CFILE_DIR=mazefiles/cfiles

if [ ! -d "$TEXT_DIR" ]; then
    mkdir "$TEXT_DIR"
fi
if [ ! -d "$CFILE_DIR" ]; then
    mkdir "$CFILE_DIR"
fi

rm  "$TEXT_DIR/"*
rm  "$CFILE_DIR/"*

for file in "$@" ; do
    echo $file
   cmake-build-debug/mazetool -t "$file" > "$TEXT_DIR/$(basename "$file" .maz)".txt
   cmake-build-debug/mazetool -c "$file" > "$CFILE_DIR/$(basename "$file" .maz)".c
done
