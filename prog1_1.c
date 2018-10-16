#include <stdio.h> // printf
#include <unistd.h> // fork pipe and primitives conforming to posix e.g std read() etc
#include <stdlib.h> // standard functions for files I/O

/* Function Prototypes */
void crcTable(unsigned int*);
unsigned int getCRC(unsigned char* buff, int len, unsigned int*);
//unsigned int stdCRC(unsigned char* buff, int len); // this would be the slow way it is implmented
// it also is arguably easier to understand


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

  unsigned char* crcBuff = (unsigned char*) malloc((fileSize) * sizeof(char));
  // NOTE: for a loop of the above will need to use < less than

  unsigned int* tab = (unsigned int*) malloc(sizeof(unsigned int) * 256); // everything is unsigned
  // ^^ avoid global's which are evil
  fread(crcBuff, sizeof(char),fileSize ,inFile);
  crcTable(tab); // precompute possible XOR'd bytes
  unsigned int checksum =  getCRC(crcBuff, fileSize, tab); // retireve the CRC of the input data
  //unsigned int checksum = stdCRC(crcBuff,fileSize); // inorder to get the same as the Linux implementation
  printf("%X\n",checksum); // X is to print out the hex (uppercase)
  if(fclose(inFile) == -1) fprintf(stderr,"Failed to close file\n");
  free(tab); // never forget
  free(crcBuff);
  return 0;
}


// Little - Endian implementation ONLY
void crcTable(unsigned int* table) { // could also return a local char array
  const unsigned int genPoly = 0xEDB88320; // if big endian would use 0x04...
  for(int i = 0; i < 256; i++) {
    unsigned int crcReg = i;
    for(int j = 0; j < 8; j++) { // shift untill we hit the MSB
      if(crcReg & 1)  { // check if the sign bit is ready to be pushed out
        crcReg = (unsigned int) (crcReg >> 1) ^ genPoly; // do the divison. NOTE: shift once to rid the irrelevant bit
        // and of course do the XOR divison
      }
      else {
        crcReg = (crcReg >> 1); // keep shifting till MSbit is set
      }
    } // first for (the bit by bit)
    table[i] = crcReg;
  }
  return;
}

//goal is work with the input byte by byte
unsigned int getCRC(unsigned char* buff, int len, unsigned int* table) {
  unsigned int crcReg =  0xFFFFFFFF;
  for(int i = 0; i < len; i++) {
    unsigned int lookUp = (crcReg ^ buff[i]) & 0xFF; // XOR in a new and logical AND it to turn it into a byte
    crcReg = (crcReg >> 8) ^ table[lookUp]; // shift out old MSB and XOR with new intermediate little endian e
  }
  return crcReg ^ 0xFFFFFFFF;
}


/*
//NOTE: This implementation is the slow CRC32 for little Endian
 unsigned int stdCRC(unsigned char* buff, int len) {
  const unsigned int genPoly = 0xEDB88320; // some guy did a lot of math to find this
  unsigned int crcReg =  0xFFFFFFFF; // unsigned to get full range and also linux way

  for(int i = 0; i < len; i++) { // for each byte...
    // done to align the byte into the MSB
    // and XOR to update the xor the next byte with curr crc value
    crcReg = (crcReg ^ (buff[i] & 0xFF));
    // below is the standard shifting done for each byte
    for(int j = 0; j < 8; j++) { // shift untill we hit the MSB
      if(crcReg & 1)  {
        crcReg = (unsigned int) (crcReg >> 1) ^ genPoly; // do the divison. NOTE: shift once to rid the irrelevant byte
        // and of course do the XOR divison
      }
      else {
        crcReg = (crcReg >> 1); // keep shifting till MSbit is set
      }
    } // first for (the bit by bit)
  } // 2nd for
  return crcReg ^ 0xFFFFFFFF ;
}
*/