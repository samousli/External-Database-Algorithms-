#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <cstddef>
#include <queue>

#include "dbtproj.h"
#include "ComparisonPredicates.h"
#include "HelpingMethods.h"
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
    ifstream input(infile, ios::binary | ios::ate); // open input file
    ofstream output("output1.bin",ios::out | ios::binary);
    int numOfBlocks = input.tellg() / sizeof (block_t); // get number of blocks in file
    input.close();
    ifstream inputFile; // stream for input file
    inputFile.open(infile, ios::in | ios::binary); // open input file
    int numOfBlocksForSplit = 0;
    int retrievedBlocks = 0;
    int numOfFile = 1;
    block_t retrievedBlock; // temporary block_t for storage from retrieval
    record_t record;
    while (retrievedBlocks < numOfBlocks) {
        cout << "yes fucker !!!!!" << endl;
        // Read blocks and sort each block.
        //
        for (int i = 0; i < nmem_blocks; i++) {
            if (retrievedBlocks < numOfBlocks) {
                inputFile.read((char*) &retrievedBlock, sizeof (block_t)); // read block from file
                retrievedBlocks++;
                if (field == '0') { // sort each block by recid . field = 0
                    std::qsort(&retrievedBlock.entries, MAX_RECORDS_PER_BLOCK, sizeof (record_t), compare);
                } else if (field == '1' || field == '3') { // sort each block by num . field = 1 or field = 3
                    std::qsort(&retrievedBlock.entries, MAX_RECORDS_PER_BLOCK, sizeof (record_t), compare1);
                } else if (field == '2') { // sort each block by str  . field = 2
                    std::qsort(&retrievedBlock.entries, MAX_RECORDS_PER_BLOCK, sizeof (record_t), compare2);
                }
                for (int r = 0; r < retrievedBlock.nreserved; ++r) {
                    record = retrievedBlock.entries[r];
                    printf("this record id: %d, num: %d, str: '%s' belongs to block %d\n",
                            record.recid, record.num, record.str, retrievedBlock.blockid);
                }
                cout << "----------" << endl;
                buffer[numOfBlocksForSplit] = retrievedBlock; // put block to buffer
                numOfBlocksForSplit++;
            } else { // empty block . Make it valid = false
                buffer[numOfBlocksForSplit].valid = false;
                numOfBlocksForSplit++;
            }
        }
        cout << "### END ----- " << endl;
       // std::string outputPathString("output1.bin"); // string with output file path
        //outputPathString.append(to_string(numOfFile)); // append number of output file
        //numOfFile++; // next output file id
       // outputPathString.append(".bin"); // append extension of output file
        //char *outputPath = (char*) outputPathString.c_str(); // convert string to char array
        SortRecords(buffer, nmem_blocks, output, field); // call method with full buffer to sort records and save them to file

        numOfBlocksForSplit = 0;
    }


    inputFile.close(); // close stream
    output.close();
    char temp_output[] = "output1.bin";

    print_file_contents(temp_output, numOfBlocks);
    string s;
    cin >> s;

    free(buffer);
    buffer = NULL;

    if (numOfBlocks <= nmem_blocks) {
        rename(temp_output, outfile);
        return;
    }
    file_merge(temp_output, outfile, nmem_blocks, field, numOfBlocks,
            nsorted_segs, npasses, nios);
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

    // Merge Sort!
    // Linear scan
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
}

