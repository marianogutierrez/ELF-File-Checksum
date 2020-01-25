# CS530Assignment1: ELF File Verification via CRC32
Mariano Gutierrez
margutierrez75@gmail.com

## prog1_1: 
   This program takes in an **Elf File** and runs the CRC32 algorithim 
   on all the **entire file**. It then **outputs** the checksum.
   The CRC32 for this program and the rest are implemented with a CRC32 table 
   construction for faster results. The slow result code is also present except
   on the last program to save space.
    
## prog1_2:
   This progam also takes in an **Elf File** and computes the CRC32 soley on the
   program header table portion. This is easily done using fseek() and fread().
   It **ouputs** the checksum of this portion.
    
## prog1_3:
   This program makes use of **<elf.h>** in order to save a great amount of time 
   with struct construction. It  takes in **an elf file, and the section header you want to CRC32.** 
   With the header file one can simply seek into the
   proper section header and read the section and correctly fill in **ALL** 
   the fields of each struct within the section header table. This can be used to
   retrieve the location of the offset to the string table, and then take that
   to be able to retrieve the snames for comparison. A CRC32 checksum is produced
   if their are multiple section headers with the same name e.g. .gnu
   
## Compilation:
   Compilation: '''prog1_(numberOfProgram) -o prog1_(numberOfProgram)'''
   
## Execution commands:
- prog1_1: '''./prog1_1 *target file* '''
- prog1_2: '''./prog1_2 *target file* ''' 
- prog1_3 '''./prog1_3 *target file*  *ELF file section e.g. .gnu *'''

## Helpful Sources
- Regarding CRC: http://www.sunshine2k.de/articles/coding/crc/understanding_crc.html 
- Regarding ELF File headers: https://linux.die.net/man/5/elf 
- More on ELF Files: https://en.wikipedia.org/wiki/Executable_and_Linkable_Format
