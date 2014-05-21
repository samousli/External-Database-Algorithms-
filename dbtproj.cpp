#include <iostream>
#include "dbtproj.h"
using namespace std;

/* ----------------------------------------------------------------------------------------------------------------------
   infile: the name of the input file
   field: which field will be used for sorting: 0 is for recid, 1 is for num, 2 is for str and 3 is for both num and str
   buffer: pointer to memory buffer
   nmem_blocks: number of blocks in memory
   outfile: the name of the output file
   nsorted_segs: number of sorted segments produced (this should be set by you)
   npasses: number of passes required for sorting (this should be set by you)
   nios: number of IOs performed (this should be set by you)
   ----------------------------------------------------------------------------------------------------------------------
*/
void MergeSort (char *infile, unsigned char field, block_t *buffer, unsigned int nmem_blocks, char *outfile, unsigned int *nsorted_segs, unsigned int *npasses, unsigned int *nios) {
    FILE *outputFile = fopen(outfile,"w"); // open output file 
    block_t blockForOutput; // block for output mode
    int currentSizeOfBlock  = 0; // counter block size ( maximum = MAX_RECORDS_PER_BLOCK )
    for(int i=0;i<nmem_blocks-1;i = i+2){
        int counterOne = 0; // counter of first block
        int counterTwo = 0; // counter of second block
        while((counterOne < buffer[i].nreserved) && (counterTwo < buffer[i+1].nreserved)){ 
            if(buffer[i].entries[counterOne].num <= buffer[i].entries[counterTwo].num){ // bigger value from first block or same value
                if(currentSizeOfBlock < MAX_RECORDS_PER_BLOCK){
                   memcpy(&blockForOutput.entries[currentSizeOfBlock],&buffer[i].entries[counterOne],sizeof(record_t));	// write the record to the block
                   currentSizeOfBlock++; // added record_t to block_t
                }
                else{
                   blockForOutput.nreserved = MAX_RECORDS_PER_BLOCK;
                   blockForOutput.valid = true;
                   fwrite(&blockForOutput, 1, sizeof(block_t), outputFile);	// write the block to the file
                   currentSizeOfBlock = 0; // reset size of block 
                }
                counterOne++; // go to next record of first block
            }
            else if(buffer[i].entries[counterOne].num > buffer[i].entries[counterTwo].num){ // bigger value from second Block
                if(currentSizeOfBlock < MAX_RECORDS_PER_BLOCK){
                   memcpy(&blockForOutput.entries[currentSizeOfBlock],&buffer[i+1].entries[counterTwo],sizeof(record_t));	// write the record to the block
                   currentSizeOfBlock++; //added record_t to block_t
                }
                else{
                   blockForOutput.nreserved = MAX_RECORDS_PER_BLOCK;
                   blockForOutput.valid = true;
                   fwrite(&blockForOutput, 1, sizeof(block_t), outputFile);	// write the block to the file
                   currentSizeOfBlock = 0;
                }
               counterTwo++;    // go to next record of second block 
            }
        }
       if(counterOne < buffer[i].nreserved){ // not empty block (first block )
            if(currentSizeOfBlock < MAX_RECORDS_PER_BLOCK){
                   memcpy(&blockForOutput.entries[currentSizeOfBlock],&buffer[i].entries[counterOne],sizeof(record_t));	// write the record to the block
                   currentSizeOfBlock++; // added record_t to block_t
                }
                else{
                   blockForOutput.nreserved = MAX_RECORDS_PER_BLOCK;
                   blockForOutput.valid = true;
                   fwrite(&blockForOutput, 1, sizeof(block_t), outputFile);	// write the block to the file
                   currentSizeOfBlock = 0; // reset size of block 
                }
                counterOne++; // go to next record of first block
       } 
       else if(counterTwo < buffer[i+1].nreserved){ // not empty block (second block)
           if(currentSizeOfBlock < MAX_RECORDS_PER_BLOCK){
                   memcpy(&blockForOutput.entries[currentSizeOfBlock],&buffer[i+1].entries[counterTwo],sizeof(record_t));	// write the record to the block
                   currentSizeOfBlock++; //added record_t to block_t
                }
                else{
                   blockForOutput.nreserved = MAX_RECORDS_PER_BLOCK;
                   blockForOutput.valid = true;
                   fwrite(&blockForOutput, 1, sizeof(block_t), outputFile);	// write the block to the file
                   currentSizeOfBlock = 0;
                }
               counterTwo++;    // go to next record of second block 
       }
        
    }
    
    fclose(outputFile);
}


/* ----------------------------------------------------------------------------------------------------------------------
   infile: the name of the input file
   field: which field will be used for sorting: 0 is for recid, 1 is for num, 2 is for str and 3 is for both num and str
   buffer: pointer to memory buffer
   nmem_blocks: number of blocks in memory
   outfile: the name of the output file
   nunique: number of unique records in file (this should be set by you)
   nios: number of IOs performed (this should be set by you)
   ----------------------------------------------------------------------------------------------------------------------
*/
void EliminateDuplicates (char *infile, unsigned char field, block_t *buffer, unsigned int nmem_blocks, char *outfile, unsigned int *nunique, unsigned int *nios) {


}

/* ----------------------------------------------------------------------------------------------------------------------
   infile1: the name of the first input file
   infile2: the name of the second input file
   field: which field will be used for the join: 0 is for recid, 1 is for num, 2 is for str and 3 is for both num and str
   buffer: pointer to memory buffer
   nmem_blocks: number of blocks in memory
   outfile: the name of the output file
   nres: number of pairs in output (this should be set by you)
   nios: number of IOs performed (this should be set by you)
   ----------------------------------------------------------------------------------------------------------------------
*/
void MergeJoin (char *infile1, char *infile2, unsigned char field, block_t *buffer, unsigned int nmem_blocks, char *outfile, unsigned int *nres, unsigned int *nios) {


}
