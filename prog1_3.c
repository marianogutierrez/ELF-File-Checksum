#include <stdio.h> // printf
#include <unistd.h> // fork pipe and primitives conforming to posix e.g std read() etc
#include <stdlib.h> // standard functions for files I/O
#include <string.h> // get string functions
#include <elf.h> // built in structs to fill up

/* Function Prototypes */
void crcTable(unsigned int*);
unsigned int getCRC(unsigned char* buff, int len, unsigned int*);
//unsigned int stdCRC(unsigned char* buff, int len); // this would be the slow way it is also implmented and makes more sense



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

    Elf64_Shdr section; // from elf.h has it all built in, the size of this is

    // ELF file constants
    char sectionName[];
    strcmp(sectionName,argv[2]); // what we need to find

    // var's to get from the elf section
    int e_shoff; // by offset to the section header table
    short e_shentsize; // size of !an! entry in section header all entries are the same
    short e_shnum; // number of entries in the section header
    // NOTE:  e_shentsize * e_shnum = the total bytes of the section header table
    short e_shstrndx; // holds the index of the name associated with the section name string table

   // grabbing the offset for e_shoff the start of the Section header table
   fseek(inFile,0x28,SEEK_SET); //
   fread(&e_shoff,sizeof(e_shentsize),1,inFile); // get the 8 bytes; NOTE: Cool trick with sizeOf the name of my var! Thanks Scott.
   fseek(inFile,0,SEEK_SET); // reset because we need to start form the file header

   //e_shentsize
   fseek(inFile,0x3A,SEEK_SET); // size of !an! entry all entries are the same size...
   fread(&e_shentsize,sizeof(e_shentsize),1,inFile);
   fseek(inFile,0,SEEK_SET); // reset because we need to start form the file header

   //e_shnum
   fseek(inFile,0x3C,SEEK_SET); // num entries in the setion header table
   fread(&e_shnum,sizeof(e_shnum),1,inFile);
   fseek(inFile,0,SEEK_SET); // reset because we need to start form the file header

   //e_shstrndx
   fseek(inFile,0x3E,SEEK_SET); //
   fread(&e_shstrndx,sizeof(e_shstrndx),1,inFile);
   fseek(inFile,0,SEEK_SET); // reset because we need to start form the file header


   // now that we have everything
   /* Go to the first section, then take the the index of the
   section header table's entry assoicated with the string table * by the size of a sect header
   this will take me to the beginning point of the SHTable.
   */
   fseek(inFile , e_shoff + (e_shstrndx * (e_shentsize), SEEK_SET); // @ SHTable
   fread(&section, sizeof(sectHdr), 1, inFile); // the struct now holds all the data
   // it is auto copied

   Char* sectNames = (char*) malloc(sectHdr.sh_size); // access the newly assigned var
   fseek(inFile, sectHdr.sh_offset, SEEK_SET); // now go to where the section begins
   fread(sectNames, 1, sectHdr.sh_size, inFile); // read 


   for(int unsigned i = 0; i < e_shnum; i++) { // loop through that section header table
      fread(&,e_shoff + i * sizeof(e_shentsize),SEEK_SET); // So, the the general offset + the number we are on * the size of each one
      fseek();
   } //strncmp lets you compare only a portion








    unsigned int* tab = (unsigned int*) malloc(sizeof(unsigned int) * 256); // everything is unsigned
    // ^^ avoid global's which are evil
    crcTable(tab); // precompute possible XOR'd bytes
    unsigned int checksum =  getCRC(crcBuff, pHeaderLen, tab); // retireve the CRC of the input data
    //unsigned int checksum = stdCRC(crcBuff,fileSize); // inorder to get the same as the Linux implementation
    printf("%X\n",checksum); // X is to print out the hex (uppercase)
    if(fclose(inFile) == -1) ; // force close all files
    free(crcBuff);
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
// the above is intended to extend the char into a but, im pretty sure it auto casts when it is XOR'd
// for some reason.. failed with a pointer
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
