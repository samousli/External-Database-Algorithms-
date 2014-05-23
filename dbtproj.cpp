#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
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
void MergeSort(char *infile, unsigned char field, block_t *buffer, unsigned int nmem_blocks,
    char *outfile, unsigned int *nsorted_segs, unsigned int *npasses, unsigned int *nios) {

    ifstream inputFile; // stream for input file 
    inputFile.open(infile, ios::in | ios::binary); // open input file 
    int numOfBlocksForSplit = 0;
    int numOfFile = 1;
    block_t retrievedBlock; // temporary block_t for storage from retrieval 
    while (!inputFile.eof()) { // while end-of-file has not been reached ... 

        // Read blocks and sort each block.
        // 
        for (int i = 0; i < nmem_blocks; i++) {
            if (inputFile.eof()) {
                retrievedBlock.valid = false;

            } else {
                inputFile.read((char*) &retrievedBlock, sizeof (block_t)); // read block from file 
                std::qsort(&retrievedBlock.entries, MAX_RECORDS_PER_BLOCK, sizeof (record_t), compare);
            }
            buffer[numOfBlocksForSplit] = retrievedBlock; // put block to buffer
            numOfBlocksForSplit++;
        }

        std::string outputPathString("output"); // string with output file path
        outputPathString.append(to_string(numOfFile)); // append number of output file
        numOfFile++; // next output file id 
        outputPathString.append(".bin"); // append extension of output file 
        char *outputPath = (char*) outputPathString.c_str(); // convert string to char array

        SortRecords(buffer, nmem_blocks, outputPath, field); // call method with full buffer to sort records and save them to file

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
void EliminateDuplicates(char *infile, unsigned char field, block_t *buffer,
    unsigned int nmem_blocks, char *outfile, unsigned int *nunique, unsigned int *nios) { }

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
    unsigned int nmem_blocks, char *outfile, unsigned int *nres, unsigned int *nios) { }






// Sah tests
#include <queue>
#include "ComparisonPredicates.h"


void serialize_record(block_t block, record_t record, char *filename);
void write_block(char* filename, block_t *block);
block_t read_block(char *file, uint block_id);
void merge(char *input_file, char *output_file, unsigned char field, uint k_way,
    uint total_block_count, uint *nsorted_segs, uint *npasses, uint *nios);

void merge_sort(char *infile, unsigned char field, block_t *buffer, uint nmem_blocks,
    char *outfile, uint *nsorted_segs, uint *npasses, uint *nios);

void merge_sort(char *infile, unsigned char field, block_t *buffer, uint nmem_blocks,
    char *outfile, uint *nsorted_segs, uint *npasses, uint *nios) {

    //  Separate file into chunks
    //  For each chunk
    //      Sort chunks
    //      Write inplace


    //  Merge on file, Load into memory on demand using fseek().
    //  merge(char *output_file, char *input_file, uint ways, 
    //      block_t *buffer, uint buffer_size)
}

void merge(char *input_file, char *output_file, unsigned char field, uint k_way,
    uint total_block_count, uint *nsorted_segs, uint *npasses, uint *nios) {


    /*
    Let P = a priority queue of the sorted lists, sorted by the smallest element in each list
    Let O = an empty output list
    While P is not empty:
        Let L = remove the minimum element from P
        Remove the first element from L and add it to O
        If L is not empty, add L to P
     */

    // set Compare parameters
    setComparator(field, false);

    priority_queue<block_t, std::vector<block_t>, compare_block_min> heap;
    uint chain_length = total_block_count / k_way;

    block_t min_block;
    block_t output_block;
    uint block_id = 0, fullness = 0;
    record_t min_record;

    while (!heap.empty()) {
        // (Re)initialize output block
        output_block.blockid = block_id;

        min_block = heap.top();
        heap.pop();
        min_record = min_block.entries[min_block.dummy++];
        
        
        if (min_block.dummy < min_block.nreserved) {
            heap.push(min_block);
        } else if (min_block.next_blockid % chain_length != 0) {
            // if min_block.next_blockid belongs to the chain add it
            // chain check: id should be less than the first id of the next chain or the total size
            // next_block_id % M != 0 meets both criteria
            min_block = read_block(input_file, min_block.next_blockid);
            min_block.dummy = 0;
            heap.push(min_block);
        }
        //output_block.

    }

}

/**
 * 
 * @param file input file name.
 * @param block_id the id of the block that should be read.
 * @return the allocated block 
 */
block_t read_block(char *filename, uint block_id) {
    // Find block offset.
    // first block = 0..from 0 to size of block_t
    // seek the distance from the beginning
    ifstream::streamoff offset = sizeof (block_t) * block_id;
    block_t block;

    // Open file, seek position, read block and return it
    // Corresponds to 1 IO! nios++;
    ifstream infile;
    infile.open(filename, ios::in | ios::binary);
    infile.seekg(offset, infile.beg);
    infile.read((char*) &block, sizeof (block_t));
    infile.close();

    return block;
}

// Keeping streams open may boost performance but doing this for now to simplify things.

void write_block(char* filename, block_t *block) {
    ofstream outfile;
    outfile.open(filename, ios::out | ios::binary | ios::app);
    outfile.write((char*) &block, sizeof (block_t));
    outfile.close();
}

void serialize_record(block_t block, record_t record, char *filename) {
    if(block.dummy == block.)
    
}