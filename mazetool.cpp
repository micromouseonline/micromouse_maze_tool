/***********************************************************************
 * Created by Peter Harrison on 28 Jan 2018.
 * Copyright (c) 2018 Peter Harrison
 *
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without l> imitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 **************************************************************************/


#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <getopt.h>
#include <libgen.h>
#include <iostream>
#include "sha1.h"


#define NORTH         (uint8_t)0x01
#define EAST          (uint8_t)0x02
#define SOUTH         (uint8_t)0x04
#define WEST          (uint8_t)0x08


typedef enum {
  MAZE_TYPE_OTHER,
  MAZE_TYPE_BINARY,
  MAZE_TYPE_TEXT_2_CHARS,
  MAZE_TYPE_TEXT_3_CHARS,
  MAZE_TYPE_TEXT_4_CHARS,
  MAZE_TYPE_CDECL,
  MAZE_TYPE_UNKNOWN,
} MAZE_TYPE;

typedef enum {
  OUTPUT_TEXT_MAZE,
  OUTPUT_BINARY_MAZE,
  OUTPUT_CDECL_MAZE,
  OUTPUT_HEX_MAZE,
  OUTPUT_INFO,
  OUTPUT_NONE,
} OUTPUT_TYPE;

typedef enum {
  E_NOERROR = 0,
  E_ERROR,
  E_FILE_NOT_FOUND,
  E_UNEXPECTED_END,
  E_BAD_FORMAT,
} ERROR_TYPE;

struct Options {
  OUTPUT_TYPE outputFormat;
  char *infileName;
  char *outfileName;

  void dump() {
    printf("----option settings-------\n");
    printf("outputFormat: %d\n", outputFormat);
    printf(" infilename: %s\n", infileName);
    printf("outfilename: %s\n", outfileName);
    printf("--------------------------\n");
  }
} options;

MAZE_TYPE inputFormat = MAZE_TYPE_UNKNOWN;

char hash[128];

const int MAZE_WIDTH = 16;
const int MAZE_CELLS = MAZE_WIDTH * MAZE_WIDTH;
uint8_t maze[MAZE_CELLS];

static uint8_t emptyClassicMaze[MAZE_CELLS] = {
    0x0e, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x09,
    0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
    0x06, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x03
};

uint8_t walls(uint8_t *maze, int cell) {
  return maze[cell];
}

uint8_t walls(uint8_t *maze, int col, int row) { // (x,y)
  int cell = row + MAZE_WIDTH * col;
  return walls(maze, cell);
}

/* sets a maze to its original, unknown state */
void initMaze(uint8_t *source) {
  for (int i = 0; i < MAZE_CELLS; i++) {
    if (source == nullptr) {
      maze[i] = 0;
    } else {
      maze[i] = source[i];
    }
  };
}

void addWall(int col, int row, int direction) {
  int cell = col * MAZE_WIDTH + row;
  switch (direction) {
    case NORTH :
      maze[cell] |= NORTH;
      if (row < MAZE_WIDTH) {
        maze[(cell + 1) % MAZE_CELLS] |= SOUTH;
      }
      break;
    case EAST :
      maze[cell] |= EAST;
      if (col < MAZE_WIDTH) {
        maze[(cell + MAZE_WIDTH) % MAZE_CELLS] |= WEST;
      }
      break;
    case SOUTH :
      maze[cell] |= SOUTH;
      if (row > 0) {
        maze[(cell + MAZE_CELLS - 1) % MAZE_CELLS] |= NORTH;
      }
      break;
    case WEST :
      maze[cell] |= WEST;
      if (col > 0) {
        maze[(cell + MAZE_CELLS - MAZE_WIDTH) % MAZE_CELLS] |= EAST;
      }
  }
}

void sanitize(char *s) {
  for (int i = 0; i < strlen(s); i++) {
    if (!isalnum(s[i])) {
      s[i] = '_';
    }
  }
}

void usage(void) {
  printf("\nUSAGE: mazetool [-tcxiv] [-b outfile]  file ...\n\n");
}

void help(void) {
  usage();
  printf("\t-t, --text\n\t\tgenerates a text file with three spaces per cell. This is the default.\n"
             "\t-b<filename>, --binary filename\n\t\tgenerates a binary file. Cannot be used with stdout.\n"
             "\t-c, --cdecl\n\t\tgenerates a text file containing a c declaration.\n"
             "\t-x, --hex\n\t\tgenerates a hexadeimal dump of the maze.\n"
             "\t-i, --info\n\t\tdisplays information about the input file.\n"
  );
  exit(EXIT_SUCCESS);
}

void process_args(long argc, char **argv) {
  if (argc < 2) {
    usage();
    exit(EXIT_SUCCESS);
  }


  options.infileName = nullptr;
  options.outfileName = nullptr;
  options.outputFormat = OUTPUT_TEXT_MAZE;

  while (1) {

    int c = getopt(argc, argv, "tib:cxh");
    if (c == -1) break;
    switch (c) {
      case 't':
        options.outputFormat = OUTPUT_TEXT_MAZE;
        break;
      case 'i':
        options.outputFormat = OUTPUT_INFO;
        break;
      case 'b':
        options.outfileName = optarg;
        options.outputFormat = OUTPUT_BINARY_MAZE;
        break;
      case 'c':
        options.outputFormat = OUTPUT_CDECL_MAZE;
        break;
      case 'x':
        options.outputFormat = OUTPUT_HEX_MAZE;
        break;
      case ':':
        printf( "%s: option `-%c' requires an argument\n", argv[0], optopt);
        break;
      case 'h':
      case '?': // getopt_long() returns '?' if it finds an error. No need to make it an option.
        help();
        break;
      default:
        exit(EXIT_FAILURE);
        break;
    }
  }
  options.infileName = argv[optind];  // optind is a global set by getopt_lobg();
}

void printNorthWalls(uint8_t *maze, uint16_t row) {
  for (uint16_t col = 0; col < MAZE_WIDTH; col++) {
    printf("o");
    if (walls(maze, col, row) & NORTH) {
      printf("---");
    } else {
      printf("   ");
    }
  }
  printf("o\n");
}

void printSouthWalls(uint8_t *maze, uint16_t row) {
  for (uint16_t col = 0; col < MAZE_WIDTH; col++) {
    printf("o");
    if (walls(maze, col, row) & SOUTH) {
      printf("---");
    } else {
      printf("   ");
    }
  }
  printf("o\n");
}

void printPlain(uint8_t *maze) {
  for (int row = MAZE_WIDTH - 1; row >= 0; row--) {
    printNorthWalls(maze, row);
    for (int col = 0; col < MAZE_WIDTH; col++) {
      if (walls(maze, col, row) & WEST) {
        printf("|");
      } else {
        printf(" ");
      }
      printf("   ");
    }
    if (walls(maze, MAZE_WIDTH - 1, row) & EAST) {
      printf("|");
    } else {
      printf(" ");
    }
    printf("\n");
  }
  printSouthWalls(maze, 0);
}

void printHex(uint8_t *maze) {
  for (int col = 0; col < MAZE_WIDTH; col++) {
    for (int row = 0; row < MAZE_WIDTH; row++) {
      printf(" %02X", walls(maze, col, row));
    }
    printf("\n");
  }
}

// write out a maze as a C declaration
void writeCdeclMaze(char *fileName) {
  char mazeName[80];
  // start with a c comment showing a text maze
  printf("\n/*  text version of maze '%s' \n", fileName);
  printf("    generated by mazetool (c) Peter Harrison 2018 \n");
  printPlain(maze);
  printf("*/\n");
  strncpy(mazeName, basename(fileName), 80);
  sanitize(mazeName);
  printf("int %s[] ={\n", mazeName);

  /* we do the last line a little different to eliminate a comma */
  for (int x = 0; x < MAZE_WIDTH - 1; x++) {
    for (int y = 0; y < MAZE_WIDTH; y++) {
      printf(" 0x%02X,", walls(maze, x, y));
    }
    printf("\n");
  }
  /* this is the last column */
  for (int y = 0; y < MAZE_WIDTH; y++) {
    printf(" 0x%02X,", walls(maze, MAZE_WIDTH - 1, y));
  }
  printf("\n};\n/* end of mazefile */\n");

}

// binary mazes have 256 bytes, starting with the start cell and
// proceeding up the first column, then the next and so on
void writeBinaryMaze(char *fname) {
  // binary files cannot be sent out to stdout
  FILE *outfile;
  outfile = fopen(fname, "wb");
  if (outfile != NULL) {
    printf("writing %s . . .", fname);
    fwrite(maze, 1, MAZE_CELLS, outfile);
    printf("done\n");
    fclose(outfile);
  } else {
    fprintf(stderr,"unable to create output file:");
  }

}


/****************************************************************************/
/* A text maze can be stored in several formats.
 * most common is to have a single 'o' or '+' for each post
 * followed by a '-' for a horizontal wall, a space for
 * no wall and a '|' for a vertical wall.
 * Alternatively, better printing can be achieved with
 * three '-' characters for a horizontal wall.
 * Here, the first character in the file is assumed to represent a post.
 * A text maze may use a single (different) character to represent both posts and walls.
 *
 * Assuming the text file is stored in the normal orientation
 * with the start square bottom left ready to print out,
 * this function will attempt to read all these formats.
 */
ERROR_TYPE readTextMaze(FILE *infile) {
  int const maxLineLength = 80;
  char nsWalls[maxLineLength], weWalls[maxLineLength];
  char *result; // not needed but handy when debugging to display the line read
  char postchar;
  char hwallchar;
  char vwallchar;
  int row, col, width;

  rewind(infile);
  // read in the first two lines to see what the format might be
  // an ASCII maze file must have at least 32 characters on the first line
  result = fgets(nsWalls, maxLineLength, infile); /* north walls */
  if (result == NULL) {
    return E_UNEXPECTED_END;
  }
  // there must be at least 2 characters per cell plus the last post
  if (strlen(nsWalls) < (MAZE_WIDTH * 2 + 1)) {
    return E_BAD_FORMAT;
  }

  // those are the obvious errors sorted out
  // the first character we see is the post
  postchar = nsWalls[0];
  // the next character after the post is a horizontal wall
  // they are all assumed to be the same and cannot be a space
  hwallchar = nsWalls[1];
  if (hwallchar == ' ') {
    return E_BAD_FORMAT;
  }
  // where we next see postchar tells us how many characters per cell
  // although we are interested only in 2, 3 or 4
  if (nsWalls[2] == postchar) {
    width = 2; /* we have one character per wall */
    inputFormat = MAZE_TYPE_TEXT_2_CHARS;
  } else if (nsWalls[3] == postchar) {
    width = 3; /* we have two character per wall */
    inputFormat = MAZE_TYPE_TEXT_3_CHARS;
  } else if (nsWalls[4] == postchar) {
    width = 4; /* we have three character per wall */
    inputFormat = MAZE_TYPE_TEXT_4_CHARS;
  } else {
    return E_BAD_FORMAT;
  }

  result = fgets(weWalls, maxLineLength, infile); /* east-west walls */
  if (result == NULL) {
    inputFormat = MAZE_TYPE_UNKNOWN;
    return E_UNEXPECTED_END;
  }
  vwallchar = weWalls[0];
  if (vwallchar == ' ') {
    inputFormat = MAZE_TYPE_UNKNOWN;
    return E_BAD_FORMAT;
  }

  // now we just assume the rest of the file makes sense in the same pattern
  // until an error occurs
  rewind(infile);
  // a text maze starts top left and every row takes up two lines of text
  row = MAZE_WIDTH - 1;
  while (row >= 0) {
    result = fgets(nsWalls, maxLineLength, infile); /* north walls */
    if (result == NULL) {
      return E_UNEXPECTED_END;
    }
    result = fgets(weWalls, maxLineLength, infile); /* east-west walls */
    if (result == NULL) {
      return E_UNEXPECTED_END;
    }
    // just go through the lines grabbing wall information
    // look at the west side because the last cell always has an east wall
    // walls are not stored twice in a text maze, unlike binary mazes
    for (col = 0; col < MAZE_WIDTH; col++) {
      if (nsWalls[1 + width * col] != ' ') addWall(col, row, NORTH);
      if (weWalls[width * col] != ' ') addWall(col, row, WEST);
    }
    // and fill in the East wall at the end of the row;
    if (weWalls[width * (MAZE_WIDTH)] != ' ') {
      addWall(MAZE_WIDTH - 1, row, EAST);
    }
    row--;
  }
  // there should be one row of text left which is the south walls of the maze
  result = fgets(nsWalls, maxLineLength, infile); /* north walls */
  if (result == NULL) {
    return E_UNEXPECTED_END;
  }
  row = 0;
  for (col = 0; col < MAZE_WIDTH; col++) {
    if (nsWalls[1 + width * col] != ' ') addWall(col, row, SOUTH);
  }
  printf("\n");
  return E_NOERROR;
}

/**
 * Trivial function to read a binary format maze file
 * directly into the wall data array
 *
 * A binary maze file is stored as 256 sequential bytes
 * in 'natural' order. That is, the first byte in the file
 * is cell 0,0 followed by the cells in the left-most column, col 0.
 * Next will be col 1 and so on. Each cell is just the raw wall data
 * as defined for internal maze storage.
 * Since the first cell read is the start cell, it must have walls
 * to its West, South and East giving it a value of 0x0E. This is the
 * only fixed-value cell in a maze
 */
ERROR_TYPE readBinaryMaze(FILE *infile) {
  rewind(infile);
  if (fread(maze, 1, MAZE_CELLS, infile) < MAZE_CELLS) {
    return E_BAD_FORMAT;
  };
  return E_NOERROR;
}

/**
 * Open a file and determine what kind of maze this represents.
 * Closes the file after the test
 *
 * If successful, the function reads the file, interprets the contents
 * and fills the global maze variable with the wall data. Additionally,
 * the global variable inputFileType holds a MAZE_TYPE value describing
 * the data found in the file.
 *
 * If the opening, reading or translation of the file fails for any
 * reason the maze data is left blank
 * Returns the maze type
 */
ERROR_TYPE readMazeFile(char *fname) {
  FILE *infile;
  ERROR_TYPE fileOpResult;
  uint8_t buffer[32];
  initMaze(nullptr);
  inputFormat = MAZE_TYPE_UNKNOWN;
  // first lets see if there is a file there at all
  infile = fopen(fname, "r");
  if (infile == NULL) {
    return E_FILE_NOT_FOUND;
  }
  // the file is there and we got it open so see how big it is
  fseek(infile, 0, SEEK_END);
  long size = ftell(infile);
  rewind(infile);
  if (size < 256) {
    fclose(infile);
    return E_UNEXPECTED_END;
  }

  // OK, so we have a file with stuff in it that is big enough...
  // let's take a look at the first few bytes of it
  fread(buffer, 1, 32, infile);

  // first, see if it could be a binary maze file. These are very distinct.
  if (size == 256 && buffer[0] == 0x0e) {
    fileOpResult = readBinaryMaze(infile);
    if (fileOpResult == E_NOERROR) {
      inputFormat = MAZE_TYPE_BINARY;
    }
  } else {
    // text files could be ASCII representations and could start in a couple of ways
    fileOpResult = readTextMaze(infile);
  }
  fclose(infile);
  return fileOpResult;
}

void makeHash(uint8_t *maze, char *hash) {
  Sha1Digest computed;
  computed = Sha1_get(maze, 256);
  Sha1Digest_toStr(&computed, hash);
}

int processFile(char *fname) {
  ERROR_TYPE err = readMazeFile(fname);
  if (err != E_NOERROR) {
    if (err == E_BAD_FORMAT) {
      fprintf(stderr, "%s - Unable to read maze file format.\n", fname);
    } else if (err == E_FILE_NOT_FOUND) {
      fprintf(stderr, "%s - File not found.\n", fname);
    } else if (err == E_UNEXPECTED_END) {
      fprintf(stderr, "%s - Unexpected end of file\n", fname);
    } else {
      fprintf(stderr, "%s - unknown error (%d).\n", fname, err);
    }
    return EXIT_FAILURE;
  }

  switch (options.outputFormat) {
    case OUTPUT_NONE:
      break;
    case OUTPUT_TEXT_MAZE:
      printPlain(maze);
      break;
    case OUTPUT_BINARY_MAZE:
      writeBinaryMaze(options.outfileName);
      break;
    case OUTPUT_CDECL_MAZE:
      writeCdeclMaze(fname);
      break;
    case OUTPUT_HEX_MAZE:
      printHex(maze);

      printf(" %s\n", basename(fname));
      break;
    case OUTPUT_INFO:
      switch (inputFormat) {
        case MAZE_TYPE_BINARY:
          printf("%s - Binary maze 16x16 ", basename(fname));
          break;
        case MAZE_TYPE_TEXT_2_CHARS:
          printf("%s = Text maze, two characters per cell ", basename(fname));
          break;
        case MAZE_TYPE_TEXT_3_CHARS:
          printf("%s = Text maze, three characters per cell ", basename(fname));
          break;
        case MAZE_TYPE_TEXT_4_CHARS:
          printf("%s = Text maze, four characters per cell ", basename(fname));
          break;
        case MAZE_TYPE_CDECL:
          printf("%s = C declaration for a maze ", basename(fname));
          break;
        case MAZE_TYPE_OTHER:
        case MAZE_TYPE_UNKNOWN:
        default:
          printf("%s = Unknown File Type ", basename(fname));
          break;
      }
      makeHash(maze, hash);
      printf("SHA1 = %s\n", hash);
      break;
    default:
      printf("Unknown output format.\n");
      help();
      break;
  }
  return EXIT_SUCCESS;
}

/******************************************************************************/
int main(int argc, char **argv) {
  process_args(argc, argv);
  // display the rest of the command line
  //  for(int ind = optind; ind < argc; ind++){
  //    printf("%d  %s\n", ind, argv[ind]);
  //  }
  if (options.infileName == nullptr) {
    fprintf(stderr, "No input file provided.\n");
    exit(EXIT_FAILURE);
  }
  // if globbing fails to match, bash leaves the glob pattern untouched
  // so it has the wildcard characters still in it.
  // the following is not a robust test but will do for now
  // not really sure if this is a real problem
  if ((strchr(options.infileName, '*') == NULL) &&
      (strchr(options.infileName, '?') == NULL) &&
      (strchr(options.infileName, '[') == NULL)) {
    processFile(options.infileName);
  }
  return EXIT_SUCCESS;
}



