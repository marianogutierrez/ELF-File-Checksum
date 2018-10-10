#include <stdio.h> // printf
#include <unistd.h> // fork pipe and primitives conforming to posix e.g std read() etc
#include <stdlib.h> // standard functions for files I/O

/* Function Prototypes */
void crcTable();
int getCRC(char* buff, int len);
int stdCRC(char* buff, int len);


/* Program Description */
/* This program takes in a binary file, reads it in it's entirety, prints the CRC32 checksum as a hex # 8 digits
 using the CRC Polynomial Method: x32 + x26 + x23 + x22 + x16 + x12 + x11 + x10 + x8 + x7 + x5 + x4 + x2 + x + 1 */

int main(int argc, char * argv[]) {
  if (argc < 2) {
      fprintf(stderr, "Incorrect number or arguments\n"); // print to std error fd 2
      return 1;
  }
  printf("Assignment #1-1, Mariano Gutierrez, margutierrez75@gmail.com\n");

  FILE *inFile = fopen(argv[1], "rb"); // read binary mode

  if(inFile == NULL) { // check for file availability
    printf("stderr,The file could not be read.");
    return 1;
  }

  /* file pointer adjustment */
  // Clarification can be found in Kerrisk's text ch.4 on lseek()
  // NOTE:fseek() is fine, and provides more functionality, but is not the POSIX standard for universal I/O
  fseek(inFile,0,SEEK_END); // go to end
  long fileSize = ftell(inFile); // get my file size, returns file offset NOTE: returns one more [0,0,0,0,0,0... EOF, HERE]
  fseek(inFile,0,SEEK_SET); // seek back to read the file properly

  char* crcBuff = (char*) malloc((fileSize) * sizeof(char));
  // NOTE: for a loop of the above will need to use < less than

  fread(crcBuff, sizeof(char),fileSize ,inFile);
  //crcTable(); // precompute possible XOR'd bytes
  //getCRC(crcBuff, fileSize); // retireve the CRC of the input data
  int checksum = stdCRC(crcBuff,fileSize) ^ 0xFFFFFFFF; // inorder to get the same as the Linux impleentation
  printf("%X\n",checksum); // X is to print out the hex (uppercase)
  if(fclose(inFile) == -1) fprintf(stderr,"Failed to close file\n");  ; // force close all files
  free(crcBuff);
  return 0;
}

int stdCRC(char* buff, int len) {
  const unsigned int genPoly = 0x04C11DB7; // some guy did a lot of math to find this
  unsigned int crcReg = 0xFFFFFFFF; // unsigned to get full range and also linux way

  for(int i = 0; i < len; i++) { // for each byte...
    // done to align the byte into the MSB
    // and XOR to update the xor the next byte with curr crc value
    crcReg = crcReg ^ (buff[i] << 24);
    // below is the standard shifting done for each byte
    for(int j = 0; j < 8; j++) { // shift untill we hit the MSB
      if((crcReg & 0x80000000) != 0)  { // i.e 2^37 is what it is, that means the MSB is set
        crcReg = (crcReg << 1) ^ genPoly; // do the divison. NOTE: shift once to rid the irrelevant byte
        // and of course do the XOR divison
      }
      else {
        // keep shifting till MSbit is set
        crcReg = crcReg << 1;
      }
    } // first for
  } // 2nd for
  return crcReg;
}

// hex dump for part 2??? or readl elf.. I think hd will be better
