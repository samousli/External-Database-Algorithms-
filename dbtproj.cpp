#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <cstddef>
#include <queue>

#include "dbtproj.h"
#include "ComparisonPredicates.h"
#include "MergeSortImpl.h"
#include "Tests.h"

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
void MergeSort(char *infile, unsigned char field, block_t *buffer, unsigned int nmem_blocks,
               char *outfile, unsigned int *nsorted_segs, unsigned int *npasses, unsigned int *nios) {
   

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
void EliminateDuplicates(char *infile, unsigned char field, block_t *buffer,
                         unsigned int nmem_blocks, char *outfile, unsigned int *nunique, unsigned int *nios) {
    uint *sorted_segs = new uint(0),
    *passes = new uint(0);
    ifstream input("sorted.bin",ios::in | ios::binary); // temporary sorted file 
    ofstream output(outfile,ios::out | ios::binary); // output file 
    if(!input.good()){ // file not exists . Sort file and then elinimate duplicates 
      merge_sort_driver(1 << 14, 1 << 5,infile,"sorted.bin");
    }
    input.close();
    input.open("sorted.bin",ios::in | ios::binary);
     // Find file size
    input.seekg(0, input.end);
    uint block_count = input.tellg() / sizeof (block_t); // get number of blocks in file
    input.seekg(0, input.beg);
    block_t currentBlock; // current retrievedBlock
    currentBlock.blockid = 0; // initialize block id to zero 
    currentBlock.valid = false; // initialize valid to false 
    currentBlock.nreserved = 0; // initialize nreserved to zero 
    for (uint i = 0; i < block_count; i += nmem_blocks) { // read blocks while reached buffer size
        record_t currentRecord; // stores current Record for examination 
        
        // Read into buffer
        uint read_count = nmem_blocks < block_count - i ? nmem_blocks : block_count - i;
        input.read((char*) buffer, read_count * sizeof (block_t));
        ++(*nios);
        
        for(int y=0;y<nmem_blocks;y++){ // iterate buffer 
            for(int k=0;k<MAX_RECORDS_PER_BLOCK;k++){ // for each block iterate records 
                if(y== 0 && k == 0) { // we just started . Record is unique . Write it to file 
                    currentRecord = buffer[y].entries[k]; // initialize current Record 
                    serialize_record(output,currentBlock,currentRecord,nios); 
                    ++(*nunique); // found unique record 
                }
                else{
                    record_t nextRecord = buffer[y].entries[k]; // temporary storage of next record 
                    if(currentRecord.num != nextRecord.num){ // if record 
                        currentRecord = nextRecord;
                        serialize_record(output,currentBlock,currentRecord,nios);
                        ++(*nunique); // found unique record 
                    }
                }
            }
        }
    }
    if(currentBlock.nreserved != 0 ){ // last block that is not filled with Maximum Records . Write it
       output.write((char*) &currentBlock, sizeof (block_t));
    }
    output.close();
    cout << "Is sorted? " << is_sorted(outfile,1) << endl;
    cout << "Sorted segments: " << *sorted_segs << endl
         << "IO's: " << *nios << endl
         << "Unique Records: " << *nunique << endl;
    
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
void MergeJoin(char *infile1, char *infile2, unsigned char field, block_t *buffer,
               unsigned int nmem_blocks, char *outfile, unsigned int *nres, unsigned int *nios) {

    // Merge Sort Both!
    // Iterate linearly and write matching pairs to the output
    merge_sort_driver(1 << 14, 1 << 5,infile1,"output1.bin");
    merge_sort_driver(1 << 14 , 1 << 5,infile2,"output2.bin");

    
}

