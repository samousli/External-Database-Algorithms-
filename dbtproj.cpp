#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <cstddef>
#include <queue>

#include "dbtproj.h"
#include "ComparisonPredicates.h"
#include "MergeSortImpl.h"
#include "MergeJoinImpl.h"
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
    nunique = new uint(0);
    nios = new uint(0);

    char sortedfile[] = "sorted.bin";    // Needs random name

    ifstream input(sortedfile, ios::in | ios::binary); // temporary sorted file
    ofstream output(outfile, ios::out | ios::binary); // output file

    if (!input.good()) { // file not exists . Sort file and then elinimate duplicates
        merge_sort(infile, field, buffer, nmem_blocks, outfile, sorted_segs, passes, nios);
    }
    input.close();

    uint block_count = file_block_count(sortedfile);
    input.open(sortedfile, ios::in | ios::binary);

    block_t currentBlock; // current retrievedBlock
    currentBlock.blockid = 0; // initialize block id to zero
    currentBlock.valid = false; // initialize valid to false
    currentBlock.nreserved = 0; // initialize nreserved to zero
    record_t currentRecord; // stores current Record for examination

    input.read((char*) buffer, nmem_blocks * sizeof (block_t));
    ++(*nios);

    currentRecord = buffer[0].entries[buffer[0].dummy++];
    int startIteration = 1; //start iteration from second record
    serialize_record(output, currentBlock, currentRecord, nios);
    ++(*nunique); // found unique record
    uint read_count = nmem_blocks;
    for (uint i = nmem_blocks; i <= block_count; i += read_count) { // read blocks while reached buffer size
        int result = testRecordsForDuplicates(startIteration, buffer, read_count, currentRecord, currentBlock, output, nunique, nios, field);
        if (result == 0) {
            return; // problem
        }
        if (i < block_count) { // read next blocks to buffer
            // Read into buffer
            read_count = nmem_blocks < block_count - i ? nmem_blocks : block_count - i;
            input.read((char*) buffer, read_count * sizeof (block_t));
            ++(*nios);
        }
    }
    if (currentBlock.nreserved != 0) { // last block that is not filled with Maximum Records . Write it
        currentBlock.valid = true;
        output.write((char*) &currentBlock, sizeof (block_t));
        ++(*nios);
    }
    input.close();
    output.close();

    cout << "Is sorted? " << is_sorted(outfile, 1) << endl;
    cout << "Sorted segments: " << *sorted_segs << endl
            << "IO's: " << *nios << endl
            << "Unique Records: " << *nunique << endl;
    uint block_count_output = file_block_count(outfile);
    bool duplicates = test_duplicates(outfile, block_count_output, field);
    cout << "Elinimated Duplicates ?? " << duplicates << endl;

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

    mergeJoinImpl(infile1, infile2, field, buffer, nmem_blocks, outfile, nres, nios);
}
