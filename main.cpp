#include "dbtproj.h"
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <stdlib.h>     /* atoi */

#include <string>
#include <iostream>
#include <fstream>
#include <math.h>
#include <time.h>

using namespace std;

void create_test_file(std::string filename, uint nblocks);
void print_file_contents(string filename, uint nblocks);
void merge_sort_driver();

int main(int argc, char** argv) {

    merge_sort_driver();

    return 0;
}

void create_test_file(std::string filename, uint nblocks) {
    // generate a file 
    ofstream outfile;
    block_t block;
    record_t record;
    uint recid = 0;
    // Seed the pseudo-random generator
    srand(time(NULL));

    outfile.open(filename, ios::out | ios::binary); // open input file 
    for (int b = 0; b < nblocks; ++b) { // for each block

        block.blockid = b;
        for (int r = 0; r < MAX_RECORDS_PER_BLOCK; ++r) { // for each record

            // prepare a record
            record.recid = recid++;
            record.num = rand() % 1000;
            strcpy(record.str, "hello"); // put the same string to all records
            record.valid = true;

            memcpy(&block.entries[r], &record, sizeof (record_t)); // copy record to block
        }

        block.nreserved = MAX_RECORDS_PER_BLOCK;
        block.valid = true;

        // Set next block id
        b != nblocks - 1 ? block.next_blockid = b + 1 : block.next_blockid = -1;


        outfile.write((char*) &block, sizeof (block_t)); // write the block to the file
    }
    outfile.close();
}

void print_file_contents(string filename, uint nblocks) {
    ifstream infile;
    block_t block;
    record_t record;
    infile.open(filename, ios::in | ios::binary);
    // Assuming that the file is properly formatted.
    for (int b = 0; b < nblocks; ++b) {
        infile.read((char*) &block, sizeof (block_t)); // read block from file 

        for (int r = 0; r < block.nreserved; ++r) {
            if (block.valid) {
                record = block.entries[r];
                printf("this record id: %d, num: %d, str: '%s' belongs to block %d\n",
                        record.recid, record.num, record.str, block.blockid);
            }
        }
    }
    printf("That's all folks!\n");
    infile.close();
}

void merge_sort_driver() {
    int nblocks = 20; // number of blocks in the file
    record_t record;
    block_t block;
    uint recid = 0;
    ifstream infile;
    ofstream outfile;
    unsigned char field = '1';
    string file_name = "f.bin";

    // Create test input file.
    create_test_file(file_name, nblocks);
    print_file_contents(file_name, nblocks);

    // Create a buffer with the given block count and 
    // pass them as arguments for the sorting to take place
    int nblocks_buffer = 5;
    block_t *buffer = (block_t*) malloc(nblocks_buffer * sizeof (block_t));


    // Copy std:string to c_str
    char *path = const_cast<char *> (file_name.c_str());

    char resultsFile[] = "results.bin";
    uint *sorted_segs, *passes, *ios;

    
    //    MergeSort(path, field, buffer,
    //        nblocks_buffer, resultsFile,
    //        sorted_segs, passes, ios);

    free(buffer);
}



