/*
 * File:   EliminateDuplicatesImpl.cpp
 * Author: geohar
 *
 * Created on June 15, 2014, 7:25 PM
 */

#include "EliminateDuplicatesImpl.h"
#include "Tests.h"
#include "MergeSortImpl.h"
#include "dbtproj.h"
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <cstring>


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
    nunique = new uint(0);
    nios = new uint(0);
    uint read_count;

    char sortedfile[] = "sorted.bin";    // Needs random name

    ifstream input(sortedfile, ios::in | ios::binary); // temporary sorted file
    ofstream output(outfile, ios::out | ios::binary); // output file

    if (!input.good()) { // file not exists . Sort file and then elinimate duplicates
        merge_sort(infile, field, buffer, nmem_blocks,sortedfile, sorted_segs, passes, nios);
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
    serialize_record(output, currentBlock, currentRecord, nios);
    ++(*nunique); // found unique record
    ++(*nios);

    read_count = nmem_blocks;
    for (uint i = nmem_blocks; i <= block_count; i += read_count) { // read blocks while reached buffer size
        int result = testRecordsForDuplicates(buffer, read_count, currentRecord, currentBlock, output, nunique, nios, field);
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
    if (currentBlock.nreserved != 0 && currentBlock.nreserved != 1) { // last block that is not filled with Maximum Records . Write it
        currentBlock.valid = true;
        output.write((char*) &currentBlock, sizeof (block_t));
        ++(*nios);
    }
    input.close();
    output.close();

}


int testRecordsForDuplicates(block_t *buffer, int read_count, record_t currentRecord,
                             block_t currentBlock, ofstream &output, unsigned int *nunique, unsigned int *nios, unsigned char field) {
    for (int y = 0; y < read_count; y++) { // iterate buffer
        while(buffer[y].dummy < buffer[y].nreserved) {
            record_t nextRecord = buffer[y].entries[buffer[y].dummy++]; // temporary storage of next record
            int compare = compareRecords(currentRecord, nextRecord, field);
            currentRecord = nextRecord; // make current record the next record that retrieved before
            if (compare == -1 || compare == 1) { // if records are not equal
                if (compare == 1) {
                    cout << "Error with File Sorting . Founded record value bigger than next record value .Exit !! " << endl;
                    return 0;
                }

                serialize_record(output, currentBlock, currentRecord, nios);
                ++(*nunique); // found unique record
                ++(*nios); //
            }


        }
    }
    return 1;
}



int compareRecords(record_t rec1, record_t rec2, unsigned char field) {
    switch (field) {
    case 0:
        if (rec1.recid > rec2.recid) return 1;
        else if (rec1.recid == rec2.recid) return 0;
        else return -1;
    case 1:
        if (rec1.num > rec2.num) return 1;
        else if (rec1.num == rec2.num) return 0;
        else return -1;
    case 2:
        if (strcmp(rec1.str, rec2.str) == 1) return 1;
        else if (strcmp(rec1.str, rec2.str) == 0) return 0;
        else return -1;
    case 3:
        if (rec1.num > rec2.num) return 1;
        else if (rec1.num == rec2.num)
            if (strcmp(rec1.str, rec2.str) == 1) return 1;
            else if (strcmp(rec1.str, rec2.str) == 0) return 0;
            else return -1;
        else return -1;
    default:
        cerr << "Bad field input" << endl;
        return 0;
    }
}
