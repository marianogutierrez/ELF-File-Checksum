#include <stdio.h> // printf
#include <unistd.h> // fork pipe and primitives conforming to posix e.g std read() etc
#include <stdlib.h> // standard functions for files I/O
#include <string.h> // get string functions
#include <elf.h> // built in structs to fill up

/* Function Prototypes */
void crcTable(unsigned int*);
unsigned int getCRC(unsigned char* buff, int len, unsigned int*);
// removed slow way for save line space, but's in 1 and 2

// For this program we comppute the CRC32 on soley the program headertable.
int main(int argc, char * argv[]) {
  if (argc < 3) {
      fprintf(stderr, "Incorrect number or arguments\n"); // print to std error fd 2
      return 1;
  }
  printf("Assignment #1-3, Mariano Gutierrez, margutierrez75@gmail.com\n");
  FILE *inFile = fopen(argv[1], "rb"); // read binary mode

  if(inFile == NULL) { // check for file availability
    printf("The file could not be read.");
    return 1;
  }

    Elf64_Shdr secHead; // from elf.h has it all built in, the size of this is 64 bytes
    // use readelf -a binary. This struct is the Section header one that it details

    // ELF file constants
    char sectionName[256];
    strcpy(sectionName,argv[2]); // what we need to find
    int givenSecLen = strlen(argv[2]); // for use in strncmp
    //NOTE was able to retreive correctly with print statement

    // var's to get from the elf section
    long long e_shoff; // by offset to the section header table
    short e_shentsize; // size of !an! entry in section header all entries are the same
    short e_shnum; // number of entries in the section header
    // NOTE:  e_shentsize * e_shnum = the total bytes of the section header table
    short e_shstrndx; // holds the index of the name associated with the section name string table in the SHTable

   // grabbing the offset for e_shoff the start of the Section header table
   fseek(inFile,0x28,SEEK_SET); //
   fread(&e_shoff,sizeof(e_shoff),1,inFile); // get the 8 bytes; NOTE: Cool trick with sizeOf the name of my var! Thanks Scott.

   //e_shentsize
   fseek(inFile,0x3A,SEEK_SET); // size of !an! entry all entries are the same size...
   fread(&e_shentsize,sizeof(e_shentsize),1,inFile);

   //e_shnum
   fseek(inFile,0x3C,SEEK_SET); // num entries in the setion header table
   fread(&e_shnum,sizeof(e_shnum),1,inFile);

   //e_shstrndx where the string table is for each section table
   fseek(inFile,0x3E,SEEK_SET); //
   fread(&e_shstrndx,sizeof(e_shstrndx),1,inFile);


  unsigned char* crcBuff = (unsigned char*) malloc(sizeof(char) * 64 * 3); // 64 bytes per section and at most 3 with .gnu
  char nameBuff[265 * 29]; // size will fit all 29 possible names no doubt s

  fseek(inFile, e_shoff + (e_shstrndx * sizeof(secHead)), SEEK_SET);  // go to the SHTable using e_shstrndx
  // works bc we e-e_shstrndx tells us the index in the table where it is
  fread(&secHead,sizeof(secHead),1,inFile); // get the fields of the section header table

//NOTE: sh_offset is a byte number so I can just stick in w/o sizeof() 1st mistake
  fseek(inFile,secHead.sh_offset, SEEK_SET); // the actual offset of the section with the strings
  // NOTE: DO NOT DO SEEK_CUR the offset is legit the offset of 8 bytes for the address!!! Mistake that was made #2
  fread(nameBuff,secHead.sh_size,1,inFile); // read in the size (in bytes) of the items NOTE: no need to use sizeof
  // here because we want to copy everything not just 8 bytes
  unsigned int* tab = (unsigned int*) malloc(sizeof(unsigned int) * 256); // everything is unsigned
  // ^^ avoid global's which are evil
  crcTable(tab); // precompute possible XOR'd bytes

  for(int i = 0; i < e_shnum; i++) { // go thorugh all the section headers (of which there are 29 in this case)
    fseek(inFile,e_shoff + (i * sizeof(secHead)),SEEK_SET); // Reads all of them for the idx of sh_name
    // start at zero to read the first!
    char * actualName; // pointer to the idx of the name
    fread(&secHead,sizeof(secHead),1,inFile); // read in the current section headers
    // Get the address of where the string begins in our array
    actualName = nameBuff + secHead.sh_name; // pointer to the name + the offset (Pointer Arithmetic)
    if(strncmp(sectionName,actualName,givenSecLen) == 0)  { // use strn, because there could be mutiple e.g .rodata and .rodata1
      fseek(inFile,e_shoff + i * sizeof(secHead),SEEK_SET); // start of the one we were on NOTE: CRIT bc fread moves the file ptr
      fread(crcBuff, 64,1,inFile); // GRAB IT!
      unsigned int checksum =  getCRC(crcBuff, 64, tab); // retireve the CRC of the input data
      printf("%X\n",checksum); // X is to print out the hex (uppercase)
    }
}
    if(fclose(inFile) == -1) ; // force close all files
    free(tab);
    free(crcBuff);
    //free(nameBuff);
    return 0;
}

// Little - Endian implementation ONLY
void crcTable(unsigned int* table) { // could also return a local char array
  const unsigned int genPoly = 0xEDB88320; // if big endian would use 0x04...
  for(int i = 0; i < 256; i++) {
    unsigned int crcReg = i;
    for(int j = 0; j < 8; j++) { // shift untill we hit the MSB
      if(crcReg & 1)  {
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
