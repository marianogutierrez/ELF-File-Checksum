# CS530Assignment1
Mariano Gutierrez
margutierrez75@gmail.com

prog1_1: 
    This program takes in an **Elf File** and runs the CRC32 algorithim 
    on all the entire file. It then **outputs** the checksum.
    The CRC32 for this program and the rest are implemented with a CRC32 table 
    construction for faster results. The slow result code is also present except
    on the last program to save space.
    
prog1_2:
    This progam also takes in an **Elf File** and computes the CRC32 soley on the
    program header table portion. This is easily done using fseek() and fread().
    It **ouputs** the checksum of this portion.
    
prog1_3:
    This program makes use of **<elf.h>** in order to save a great amount of time 
    with struct construction. It  takes in **an elf file, and the section header you want to CRC32 ** 
    With the header file one can simply seek into the
    proper section header and read the section and correctly fill in **ALL** 
    the fields of each struct within the section header table. This can be used to
    retrieve the location of the offset to the string table, and then take that
    to be able to retrieve the snames for comparison. A CRC32 checksum is produced
    if their are multiple with the same name e.g. .gnu
