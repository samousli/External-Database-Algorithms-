#include <iostream>
#include <fstream>
#include <cstdlib>
#include "dbtproj.h"
#include "HelpingMethods.h"
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
void MergeSort (char *infile, unsigned char field, block_t *buffer, unsigned int nmem_blocks, 
    char *outfile, unsigned int *nsorted_segs, unsigned int *npasses, unsigned int *nios) {
    
    ifstream inputFile; // stream for input file 
    inputFile.open(infile,ios::in | ios::binary); // open input file 
    int numOfBlocksForSplit = 0;
    int numOfFile = 1;
    block_t retrievedBlock; // temporary block_t for storage from retrieval 
    while (!inputFile.eof()) { // while end-of-file has not been reached ... 
        
        // Read blocks and sort each block.
        // 
        for(int i=0;i<nmem_blocks;i++){
            if(inputFile.eof()){
                retrievedBlock.valid = false;
                
            }
            else{
              inputFile.read((char*)&retrievedBlock,sizeof(block_t)); // read block from file 
              std::qsort(&retrievedBlock.entries,MAX_RECORDS_PER_BLOCK,sizeof(record_t),compare);
            }
            buffer[numOfBlocksForSplit] = retrievedBlock; // put block to buffer
            numOfBlocksForSplit++;      
        }
       
        std::string outputPathString("output"); // string with output file path
        outputPathString.append(to_string(numOfFile)); // append number of output file
        numOfFile++; // next output file id 
        outputPathString.append(".bin"); // append extension of output file 
        char *outputPath = (char*)outputPathString.c_str(); // convert string to char array
        SortRecords(buffer,nmem_blocks,outputPath,field); // call method with full buffer to sort records and save them to file
        numOfBlocksForSplit = 0;
        
            
    }
    inputFile.close(); // close stream
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
