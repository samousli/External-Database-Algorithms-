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
#include <queue>
#include "ComparisonPredicates.h"
#include "HelpingMethods.h"
#include "MergeSortImpl.h"

using namespace std;

void create_test_file(std::string filename, uint nblocks);
void print_file_contents(string filename, uint nblocks);
void merge_sort_driver();
void heap_test(char *filename, uint nblocks);

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
    for (uint b = 0; b < nblocks; ++b) { // for each block

        block.blockid = b;
        for (int r = 0; r < MAX_RECORDS_PER_BLOCK; ++r) { // for each record

            // prepare a record
            record.recid = recid++;
            record.num = rand() % 1000;
            strcpy(record.str, "hello\0"); // put the same string to all records
            record.valid = true;

            memcpy(&block.entries[r], &record, sizeof (record_t)); // copy record to block
        }
        block.dummy = 0;
        block.nreserved = MAX_RECORDS_PER_BLOCK;
        block.valid = true;

        // Set next block id
        b + 1 != nblocks ? block.next_blockid = b + 1 : block.next_blockid = 0;


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
    for (uint b = 0; b < nblocks; ++b) {
        infile.read((char*) &block, sizeof (block_t)); // read block from file 
        if (block.valid) {
            for (uint r = 0; r < block.nreserved; ++r) {
                if (block.valid) {
                    record = block.entries[r];
                    printf("this record id: %d, num: %d, str: '%s' belongs to block %d\n",
                            record.recid, record.num, record.str, block.blockid);
                }
            }
        } else {
            cerr << "Invalid block!" << endl;
        }

    }
    printf("That's all folks!\n");
    infile.close();
}

void merge_sort_driver() {
    int nblocks = 16; // number of blocks in the file
    //record_t record;
    //block_t block;
    //uint recid = 0;
    //ifstream infile;
    //ofstream outfile;
    //unsigned char field = '1';
    char input_file[] = "input.bin";
    char output_file[] = "output.bin";

    // Create test input file.
    create_test_file(input_file, nblocks);
    //heap_test(input_file, nblocks);
    //print_file_contents(input_file, nblocks);

    // Create a buffer with the given block count and 
    // pass them as arguments for the sorting to take place
    int nblocks_buffer = 4;
    block_t *buffer = (block_t*) malloc(nblocks_buffer * sizeof (block_t));


    uint *sorted_segs = (uint*) calloc(1, sizeof (uint)),
            *passes = (uint*) calloc(1, sizeof (uint)),
            *ios = (uint*) calloc(1, sizeof (uint));

    cout << "---------" << endl;
    
    merge_sort(input_file, 1, buffer,
            nblocks_buffer, output_file,
            sorted_segs, passes, ios);

    print_file_contents("temp.bin", nblocks);

    free(buffer);
    free(sorted_segs);
    free(ios);
    free(passes);
}

void heap_test(char *filename, uint nblocks) {

    ifstream infile;
    block_t block;
    vector<block_t> blocks;
    // Load files into vector
    infile.open(filename, ios::in | ios::binary);
    // Assuming that the file is properly formatted.
    for (uint b = 0; b < nblocks; ++b) {
        infile.read((char*) &block, sizeof (block_t)); // read block from file 
        blocks.push_back(block);
    }
    infile.close();


    // Sort each block
    for (uint i = 0; i < blocks.size(); ++i)
        qsort(blocks[i].entries, 100, sizeof (record_t), compare);


    // Fill queue 
    priority_queue<block_t, vector<block_t>, block_comparator> pq(block_comparator(1, true));
    for (uint i = 0; i < blocks.size(); ++i)
        pq.push(blocks[i]);


    // Merge
    while (!pq.empty()) {
        block_t b = pq.top();
        record_t r = b.entries[b.dummy];
        pq.pop();
        b.dummy++;

        printf("this record id: %d, num: %d, str: '%s'\n",
                r.recid, r.num, r.str);


        if (b.dummy < b.nreserved)
            pq.push(b);
    }
}


