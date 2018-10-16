#include <stdio.h> // printf
#include <unistd.h> // fork pipe and primitives conforming to posix e.g std read() etc
#include <stdlib.h> // standard functions for files I/O

/* Function Prototypes */
void crcTable(unsigned int*);
unsigned int getCRC(unsigned char* buff, int len, unsigned int*);
//unsigned int stdCRC(unsigned char* buff, int len); // this would be the slow way it is implmented and is
// arguably easier to understand


// For this program we comppute the CRC32 on soley the program headertable.
int main(int argc, char * argv[]) {
  if (argc < 2) {
      fprintf(stderr, "Incorrect number or arguments\n"); // print to std error fd 2
      return 1;
  }

  FILE *inFile = fopen(argv[1], "rb"); // read binary mode

  if(inFile == NULL) { // check for file availability
    printf("The file could not be read.");
    return 1;
  }

    int phOffset; // offeset to the program header table
    short e_phentsize; // size of the program header
    short e_phnum; // number of entires

    /* file pointer adjustment */
    // Clarification can be found in Kerrisk's text ch.4 on lseek()
    // NOTE:fseek() is fine, and provides more functionality, but is not the POSIX standard for universal I/O

    fseek(inFile,0x20,SEEK_SET); // Start of the program header table; this is 8 bytes
    fread(&phOffset,sizeof(phOffset),1,inFile); // get the 8 bytes; NOTE: Cool trick with sizeOf the name of my var! Thanks Scott.

    // ok now that we are back here we e_phentsize and e_phentnum
    fseek(inFile,0x36,SEEK_SET); // size offset at 36 on 64bit machines
    fread((&e_phentsize),sizeof(e_phentsize),1,inFile); // it's two bytes, so I made it a short

    fseek(inFile,0x38,SEEK_SET); // I could take the address of the other var and do +0x02
    fread((&e_phnum),sizeof(e_phnum),1,inFile);


    // fseek(inFile,0,SEEK_SET); // seek back to read the file properly; can use rewind too.
    char* crcBuff = (char*) malloc((e_phentsize * e_phnum) * sizeof(char)); // number of entires * their size for all their bytes

    fseek(inFile,phOffset,SEEK_SET); // back to the program header
    int pHeaderLen = e_phnum * e_phentsize; // the length
    fread(crcBuff, sizeof(char),pHeaderLen,inFile ); // read it all in


    unsigned int* tab = (unsigned int*) malloc(sizeof(unsigned int) * 256); // everything is unsigned
    // ^^ avoid global's which are evil
    crcTable(tab); // precompute possible XOR'd bytes
    unsigned int checksum = getCRC(crcBuff, pHeaderLen, tab); // retireve the CRC of the input data
    printf("%X\n",checksum); // X is to print out the hex (uppercase)
    if(fclose(inFile) == -1) ; // force close all files
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

//goal is work with the input byte by bte
unsigned int getCRC(unsigned char* buff, int len, unsigned int* table) {
  unsigned int crcReg =  0xFFFFFFFF;
  for(int i = 0; i < len; i++) {
    unsigned int lookUp = (crcReg ^ buff[i]) & 0xFF; // XOR in a new and logical AND it to turn it into a byte
    crcReg = (crcReg >> 8) ^ table[lookUp]; // shift out old MSB and XOR with new intermediate little endian e
  }
  return crcReg ^ 0xFFFFFFFF;
}
