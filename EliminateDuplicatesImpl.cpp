
#include "EliminateDuplicatesImpl.h"
#include "ComparisonPredicates.h"
#include "MergeSortImpl.h"
#include "Tests.h"
#include "dbtproj.h"
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <cstring>

using namespace std;

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
void eliminateDuplicatesImpl(char *infile, unsigned char field, block_t *buffer,
                             unsigned int nmem_blocks, char *outfile, unsigned int *nunique, unsigned int *nios) {

    uint *sorted_segs = new uint(0),
    *passes = new uint(0);
    uint read_count;

    char sortedfile[] = "sorted.bin";    // Needs random name

    ifstream input(sortedfile,ios::in | ios::binary); // temporary sorted file
    ofstream output(outfile, ios::out | ios::binary); // output file

    // File does not exist. Sort file and then eliminate duplicates
    if (!input) {
        merge_sort(infile, field, buffer, nmem_blocks, sortedfile, sorted_segs, passes, nios);
    }
    delete sorted_segs;
    delete passes;


    input.close(); // close sorted file
    uint block_count = file_block_count(sortedfile); // get number of blocks (sorted file )
    input.open(sortedfile, ios::in | ios::binary); // open sorted file again

    // current retrieved block
    block_t outputBlock;
    memset(&outputBlock, 0, sizeof(block_t));
    outputBlock.blockid = 0;
    outputBlock.valid = false;
    outputBlock.nreserved = 0;

    // stores current Record for examination
    record_t currentRecord;

    input.read((char*)buffer, nmem_blocks * sizeof (block_t)); // fill buffer from file
    ++(*nios);

     currentRecord = buffer[0].entries[buffer[0].dummy++]; // set currentRecord

    read_count = nmem_blocks; // initialize read_count value
    for (uint i = read_count; i < block_count; i += read_count) { // read blocks while reached buffer size
            //call method to compare values from buffer
            testRecordsForDuplicates(buffer, read_count,&currentRecord,
                                              outputBlock, output, nunique, nios, field);

            // Read into buffer
            read_count = nmem_blocks < block_count - i ? nmem_blocks : block_count - i; // number of blocks to read
            input.read((char*)buffer, read_count * sizeof (block_t));
            ++(*nios);
    }
    if (outputBlock.nreserved != 0) { // last block that is not filled with Maximum Records . Write it
        outputBlock.valid = true;
        output.write((char*) &outputBlock, sizeof (block_t));
        ++(*nios);
    }
    input.close();
    output.close();
    remove(sortedfile); // remove temporary sorted file

}

void testRecordsForDuplicates(block_t *buffer, int read_count, record_t *currentRecord,
                             block_t currentBlock, ofstream &output, unsigned int *nunique,
                             unsigned int *nios, unsigned char field) {
    record_t nextRecord ;
    for (int y = 0; y < read_count; y++) { // iterate buffer
        while(buffer[y].dummy < buffer[y].nreserved-1) { // while all records visited from block
            nextRecord = buffer[y].entries[buffer[y].dummy++]; // temporary storage of next record
            int compare = compareRecords(*currentRecord, nextRecord, field);// compare records
            if (compare == -1 || compare == 1) { // if records are not equal
                serialize_record(output, currentBlock,*currentRecord, nios);
                ++(*nunique); // found unique record
            }
            // make current record the next record that retrieved before
           * currentRecord = nextRecord;

        }
    }
}

