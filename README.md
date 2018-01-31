# Micromouse Maze Files and mazetool

This repository contains a set of micromouse classic maze files that I have collected over the years. There are more than 400 at last count. Many of them are from past contests. The file name is generally a clue as to which contest and what year. However, none of them should be considered definitive or correct. 

There are a couple of apparent duplicates where the same maze seems to have been used in more than one contest.

Some of the mazes are just test configurations and so are not representative of actual contest mazes. Some of ythese tests mazes may not offer a route tothe classic goal area.

All the maze are for the classic contest and so are 16x16 in size.

## File Formats

There are three types of maze file here, each in their own subdirectory.

### Binary
Binary mazes have the file extension `.maz` are exactly 256 bytes in size. Each byte holds the wall information for a single cell. Each wall is represented by one bit using the following masks:
```
    #define NORTH         (uint8_t)0x01
    #define EAST          (uint8_t)0x02
    #define SOUTH         (uint8_t)0x04
    #define WEST          (uint8_t)0x08
```
Thus, a cell with walls in the WEST and SOUTH directions would have the value 0x0C.

Note that internal walls are stored twice. If a cell has a wall to the East, the neighbour in that direction has a wall to the West. This redundancy greatly simplifies the use of the information and can be used to validate the maze data. When adding or removing a wall from the maze array, care must be taken to ensure both 'sides' get updated.

Conventionally, the start cell in a classic maze is in the bottom left corner with a single exit heading North. In the binary maze format, this is cell 0, the next byte is cell 1, the cell immediately to the North of the start cell. The cell to the East of the start cell is numbered cell 16.

There are other formats in use. If you come across a maze file from another source, take care to examine it for its layout. In a valid maze, using the definitions above the start cell always has the value 0x0E.

A fairly common alternative places the start cell in the top left with the exit facing East. For tese files, the first cell might contain 0x0D. There is no guarantee, however, that the bit values for each direction are the same. Generate a hex dump of the file and look at the values. It should be possible to figure out the format without too much effort.  

A hex dump of a typical binary maze file, using the 'standard' format, looks like this:

```
 0E 0A 09 0C 0A 0A 0A 0A 0A 0A 08 0A 0A 0A 08 09
 0C 09 05 06 08 0A 0A 0A 0A 0B 06 0A 0A 0A 03 05
 05 05 05 0C 02 0B 0E 08 0A 0A 08 0A 08 08 09 05
 05 04 01 06 08 0A 09 04 0A 0A 00 0A 03 05 05 05
 05 05 04 09 06 09 05 04 0A 0A 02 0A 0B 05 05 05
 05 04 03 06 0A 02 03 06 0A 0A 0A 0A 09 05 05 05
 05 05 0D 0D 0D 0C 08 0A 0A 0A 0A 09 05 05 05 05
 06 03 04 01 04 01 05 0C 09 0C 08 01 05 05 05 05
 0C 08 01 06 01 05 04 02 03 05 05 05 05 05 05 05
 05 05 05 0D 06 01 05 0C 0A 01 05 05 05 05 05 05
 05 05 05 04 09 06 03 06 0A 02 00 03 05 04 03 05
 05 04 03 05 05 0C 0A 0A 08 09 04 0A 01 05 0D 05
 05 05 0D 05 05 04 0A 08 03 05 06 0A 03 05 04 01
 05 05 04 01 04 03 0C 02 0B 06 08 0A 0A 03 05 05
 05 06 01 07 06 08 02 0A 0A 0B 06 08 0A 0A 00 01
 06 0A 02 0A 0A 02 0B 0E 0A 0A 0A 02 0A 0A 03 07
```


### Text
Text mazes have the extension `.txt`. They are a directly printable view of the mazestored as ASCII text. For convenience, they have the start cell in the lower left corner. Printing a maze with a single character for each wall makes it very difficult to read because of the aspect ratio of typical printed text. The text files in this repository all use three characters for horizontal walls. The files all have at least 2178 characters. Optionally, it would be possible to store descriptive text after the maze walls. that is not done in these files.

A text version of the maze data above looks like this:

```
o---o---o---o---o---o---o---o---o---o---o---o---o---o---o---o---o
|                                                               |
o   o---o---o---o---o---o---o---o---o---o---o---o   o---o   o---o
|       |                                   |                   |
o   o   o   o---o---o---o---o---o---o---o   o---o---o---o   o   o
|   |   |                                               |   |   |
o   o   o   o---o---o---o---o---o---o---o---o---o---o   o   o   o
|   |   |       |   |                               |   |   |   |
o   o   o   o   o   o   o---o---o---o---o---o   o   o   o   o   o
|   |   |   |   |   |   |                   |   |   |   |       |
o   o   o   o   o   o   o   o   o---o---o   o   o   o   o   o   o
|       |           |   |   |                       |       |   |
o   o---o   o   o   o   o   o   o---o---o   o---o---o   o---o   o
|   |   |   |   |   |   |   |               |           |   |   |
o   o   o   o   o   o   o   o---o---o   o   o   o---o---o   o   o
|   |   |   |   |   |   |   |       |   |   |       |   |   |   |
o   o   o   o   o   o   o   o   o   o   o   o   o   o   o   o   o
|   |   |               |   |       |       |   |       |   |   |
o   o   o   o---o---o---o   o---o   o---o---o   o   o   o   o---o
|   |   |   |           |                   |   |   |       |   |
o   o   o---o   o---o   o   o---o---o---o   o   o   o---o   o   o
|   |   |   |   |       |                   |           |       |
o   o   o   o   o   o   o---o   o---o   o---o---o---o   o   o   o
|   |       |       |   |               |                   |   |
o   o   o   o   o---o   o---o---o   o---o   o---o---o---o---o   o
|       |       |       |           |                       |   |
o---o---o---o---o   o---o---o   o---o---o---o---o---o   o---o   o
|                       |                       |               |
o   o---o---o   o---o   o---o---o   o---o---o   o---o---o   o   o
|   |                           |                           |   |
o   o   o---o---o---o---o---o   o   o---o---o---o---o---o---o   o
|   |                           |                               |
o---o---o---o---o---o---o---o---o---o---o---o---o---o---o---o---o
```


### C definitions
To use the maze data in a program, it would be inconvenient to have to read it in from a file. For convenience, each maze is also available as a C language file. The folder `cfiles` holds all the mazes in this form. You can include the file directly into code or add one or more of the cfiles to your program build.

The data is stored as an array of unsigned characters which should be 8 bits wide on most systems. The byte order is exactly the same as it the corresponding binary maze file. that is, the first byte is the start cell, the next is the cell immediately to the North. The array name is derived from the name of the original binary maze.

The cfile version includes a text representation in a comment so that you can easily visualise the data. here is the same maze data again, as found in the file `1stworld.c`

```CPP
/*  text version of maze 'mazefiles/binary/1stworld.maz':
o---o---o---o---o---o---o---o---o---o---o---o---o---o---o---o---o
|                                                               |
o   o---o---o---o---o---o---o---o---o---o---o---o   o---o   o---o
|       |                                   |                   |
o   o   o   o---o---o---o---o---o---o---o   o---o---o---o   o   o
|   |   |                                               |   |   |
o   o   o   o---o---o---o---o---o---o---o---o---o---o   o   o   o
|   |   |       |   |                               |   |   |   |
o   o   o   o   o   o   o---o---o---o---o---o   o   o   o   o   o
|   |   |   |   |   |   |                   |   |   |   |       |
o   o   o   o   o   o   o   o   o---o---o   o   o   o   o   o   o
|       |           |   |   |                       |       |   |
o   o---o   o   o   o   o   o   o---o---o   o---o---o   o---o   o
|   |   |   |   |   |   |   |               |           |   |   |
o   o   o   o   o   o   o   o---o---o   o   o   o---o---o   o   o
|   |   |   |   |   |   |   |       |   |   |       |   |   |   |
o   o   o   o   o   o   o   o   o   o   o   o   o   o   o   o   o
|   |   |               |   |       |       |   |       |   |   |
o   o   o   o---o---o---o   o---o   o---o---o   o   o   o   o---o
|   |   |   |           |                   |   |   |       |   |
o   o   o---o   o---o   o   o---o---o---o   o   o   o---o   o   o
|   |   |   |   |       |                   |           |       |
o   o   o   o   o   o   o---o   o---o   o---o---o---o   o   o   o
|   |       |       |   |               |                   |   |
o   o   o   o   o---o   o---o---o   o---o   o---o---o---o---o   o
|       |       |       |           |                       |   |
o---o---o---o---o   o---o---o   o---o---o---o---o---o   o---o   o
|                       |                       |               |
o   o---o---o   o---o   o---o---o   o---o---o   o---o---o   o   o
|   |                           |                           |   |
o   o   o---o---o---o---o---o   o   o---o---o---o---o---o---o   o
|   |                           |                               |
o---o---o---o---o---o---o---o---o---o---o---o---o---o---o---o---o
*/
int 1stworld_maz[] ={
 0x0E, 0x0A, 0x09, 0x0C, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x08, 0x0A, 0x0A, 0x0A, 0x08, 0x09,
 0x0C, 0x09, 0x05, 0x06, 0x08, 0x0A, 0x0A, 0x0A, 0x0A, 0x0B, 0x06, 0x0A, 0x0A, 0x0A, 0x03, 0x05,
 0x05, 0x05, 0x05, 0x0C, 0x02, 0x0B, 0x0E, 0x08, 0x0A, 0x0A, 0x08, 0x0A, 0x08, 0x08, 0x09, 0x05,
 0x05, 0x04, 0x01, 0x06, 0x08, 0x0A, 0x09, 0x04, 0x0A, 0x0A, 0x00, 0x0A, 0x03, 0x05, 0x05, 0x05,
 0x05, 0x05, 0x04, 0x09, 0x06, 0x09, 0x05, 0x04, 0x0A, 0x0A, 0x02, 0x0A, 0x0B, 0x05, 0x05, 0x05,
 0x05, 0x04, 0x03, 0x06, 0x0A, 0x02, 0x03, 0x06, 0x0A, 0x0A, 0x0A, 0x0A, 0x09, 0x05, 0x05, 0x05,
 0x05, 0x05, 0x0D, 0x0D, 0x0D, 0x0C, 0x08, 0x0A, 0x0A, 0x0A, 0x0A, 0x09, 0x05, 0x05, 0x05, 0x05,
 0x06, 0x03, 0x04, 0x01, 0x04, 0x01, 0x05, 0x0C, 0x09, 0x0C, 0x08, 0x01, 0x05, 0x05, 0x05, 0x05,
 0x0C, 0x08, 0x01, 0x06, 0x01, 0x05, 0x04, 0x02, 0x03, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
 0x05, 0x05, 0x05, 0x0D, 0x06, 0x01, 0x05, 0x0C, 0x0A, 0x01, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
 0x05, 0x05, 0x05, 0x04, 0x09, 0x06, 0x03, 0x06, 0x0A, 0x02, 0x00, 0x03, 0x05, 0x04, 0x03, 0x05,
 0x05, 0x04, 0x03, 0x05, 0x05, 0x0C, 0x0A, 0x0A, 0x08, 0x09, 0x04, 0x0A, 0x01, 0x05, 0x0D, 0x05,
 0x05, 0x05, 0x0D, 0x05, 0x05, 0x04, 0x0A, 0x08, 0x03, 0x05, 0x06, 0x0A, 0x03, 0x05, 0x04, 0x01,
 0x05, 0x05, 0x04, 0x01, 0x04, 0x03, 0x0C, 0x02, 0x0B, 0x06, 0x08, 0x0A, 0x0A, 0x03, 0x05, 0x05,
 0x05, 0x06, 0x01, 0x07, 0x06, 0x08, 0x02, 0x0A, 0x0A, 0x0B, 0x06, 0x08, 0x0A, 0x0A, 0x00, 0x01,
 0x06, 0x0A, 0x02, 0x0A, 0x0A, 0x02, 0x0B, 0x0E, 0x0A, 0x0A, 0x0A, 0x02, 0x0A, 0x0A, 0x03, 0x07
};
/* end of mazefile */
```

## mazetool

The mazetool program is used to read, check and convert maze files. It will read the binary maze format described above and it will try to read text format maze files with up to 3 characters used for horizontal walls. No attempt is made to read the C definition files.

The name of one maze file must be provided on the command line. If wildcards are used, only the first named file is processed.

Its default behaviour is to read the file and convert to text format, sending the output to stdout. So you could use a line like 

```
mazetool mazefiles/binary/1stworld.maz
```

to display a binary maze on the terminal in human-readable, printable format. 

Command line options are used to determine the output format:

```
-t      generate a text file on stdout
-i      display information about the file format, including the SHA1 hash
-c      generate a C definition on stdout
-x      generate a hex dump on stdout
-b filename
        create a binary maze file and write it to the supplied filename
        
```


Because output is sent to stdout, it can be redirected to a file. For example:

```
mazetool -c mazefiles/text/1stworld.txt > mazefiles/cfiles/1stworld.c
```

will read the text maze and create a C definition file.

Creating binary files requires an output file name since dumping raw binary to the terminal is rarely a good thing. To generate a binary maze from a text maze, you might use:

```
mazetool -b mazefiles/binary/1stworld.maz mazefiles/text/1stworld.txt
```

### Other operations (NOT IMPLEMENTED)
**TODO:** mazetool can also inspect the data in a maze file to try and determine its validity and display some other information about the maze. Currently, the inspection verifies that 

* there are walls around the edge of the maze
* the start cell has one exit to the North
* the goal area is not walled in
* every wall is present from both sides

### finding duplicate mazes
If you have a collection of maze files and wish to check for duplicates, you do not need mazetool. The simplest way to do this, if your operating system has the appropriate tool installed, is to generate a SHA1 checksum for each maze file, sort the results on the checksum and look for duplicate checksum values.

On linux and mac systems you might use:

```
shasum mazefiles/binary/* | sort
```

A partial output from that, on my system gives:

```
13d9035023538b74e41fb05733e84bb638597b9c  mazefiles/binary/minos03f.maz
144aefc9f0d1262289f1698549c2af9447fa2e0c  mazefiles/binary/japan1996ef.maz
144b8ced68322e53c0255da33b83a9c7e454bf0e  mazefiles/binary/japan2017ef.maz
14be949a83653c8a4e1d9c02c3539dcd2ce2a2bd  mazefiles/binary/show.maz
14e3c45a40154039f3b0b90845dc83db9a97fb3d  mazefiles/binary/nagoya2002.maz
150530555c60a9505c11cd83cf6ec45229b0fdc8  mazefiles/binary/japan2007ef.maz
150530555c60a9505c11cd83cf6ec45229b0fdc8  mazefiles/binary/uk2011f.maz
151d1ad1e40c345820149a334439aae5d38536ca  mazefiles/binary/museum.maz
15265c4c10b211d9b2c087caf32ebb4aeddec3ae  mazefiles/binary/shiba.maz
```

It is clear that the japan2007ef and uk2011f mazes are the same.

You can't fool shasum. If they are different by even one bit, the sha1 checksum will be different.

I will need suggestions from others for a suitable method on Windows systems.

## Bulk Conversions
The bash script `convert.sh` will process all the binary maze files in a folder and convert them to both text and cfile format.

The destination folder names are hard-coded into the script and will need editing before use. Windows users will, I am afraid have to make their own arrangements. Anyone reading this who would like to contribute Windows scripts should feel free to do so.
