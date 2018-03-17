#!/usr/bin/env python3
'''

Miguel Sánchez de León Peque

This script helps removing duplicated maze files.
For each group of duplicated files their names will be displayed in a list:
    0 - first_maze
    1 - second_maze
    2 - third_maze
You will be prompted to choose which files should be removed. Simply write
the number of the file you want to remove. If there is more than one file,
then write multiple numbers separated by a space.
Hit enter (without writing any number) if you want to keep all of them.
'''
from collections import defaultdict
from hashlib import sha1
from pathlib import Path


path = Path('mazefiles/binary')

# Calculate all verification sums
table = defaultdict(list)
for fname in path.glob('*.maz'):
    digest = sha1(fname.read_bytes()).hexdigest()
    table[digest].append(fname)

# Iterate over duplicates and remove those selected
for digest, files in table.items():
    if len(files) == 1:
        continue
    print('----------------------------------')
    for i, fname in enumerate(files):
        print(' {} - {}'.format(i, fname))
    remove = input('Remove (separate by spaces if multiple): ')
    for number in remove.split(' '):
        if not number:
            continue
        fname = files[int(number)]
        fname.unlink()
        print('Removed {}'.format(fname))
