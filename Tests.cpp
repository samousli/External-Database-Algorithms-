/*
 * File:   Tests.cpp
 * Author: avail
 *
 * Created on May 25, 2014, 11:20 PM
 */

#include "Tests.h"
#include "ComparisonPredicates.h"
#include "HelpingMethods.h"
#include "MergeSortImpl.h"

#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <string>
#include <iostream>
#include <fstream>
#include <math.h>
#include <time.h>
#include <queue>
#include <algorithm>


using namespace std;

void merge_sort_driver() {
    int nblocks = 2; // number of blocks in the file
    int nmem_blocks = 2;
    char input_file[] = "input.bin";
    char output_file[] = "output.bin";

    // Create test input file.
    create_test_file(input_file, nblocks);

    // Create a buffer with the given block count and
    // pass them as arguments for the sorting to take place
    block_t *buffer = (block_t*) calloc(nmem_blocks, sizeof (block_t));

    uint *sorted_segs = (uint*) calloc(1, sizeof (uint)),
          *passes = (uint*) calloc(1, sizeof (uint)),
           *ios = (uint*) calloc(1, sizeof (uint));

    merge_sort(input_file, 1, buffer,
               nmem_blocks, output_file,
               sorted_segs, passes, ios);

    print_file_contents(output_file, nblocks);

    cout << "Sorted segments: " << *sorted_segs << endl
         << "IO's: " << *ios << endl
         << "Passes: " << *passes << endl;

    //free(buffer);
    free(sorted_segs);
    free(ios);
    free(passes);
}


void create_test_file(char *filename, uint nblocks) {
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
        block.next_blockid = b + 1;


        outfile.write((char*) &block, sizeof (block_t)); // write the block to the file
    }

    outfile.close();
}

void print_file_contents(char *filename, uint nblocks) {
    ifstream infile;
    block_t block;
    record_t record;
    record_t prev_rec;
    infile.open(filename, ios::in | ios::binary);

    infile.seekg(0, infile.end);
    int block_count = infile.tellg() / sizeof(block_t);
    infile.seekg(0, infile.beg);
    printf("Size matches block count: %d\n", nblocks == block_count);

    int invalid_blocks = 0;
    // Assuming that the file is properly formatted.
    for (uint b = 0; b < block_count; ++b) {
        infile.read((char*) &block, sizeof (block_t)); // read block from file
        if (block.valid) {
            print_block_data(block);
        } else {
            ++invalid_blocks;
        }
    }

    printf("Invalid blocks: %d\n", invalid_blocks);
    printf("Block count: %d\n", block_count);
    infile.close();
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


    priority_queue<block_t, vector<block_t>, block_comparator> pq(block_comparator(1, true));
    // Sort each block
    for (uint i = 0; i < nblocks; ++i) {
        sort(blocks[i].entries, blocks[i].entries + blocks[i].nreserved, record_comparator(1, false));

        // Debug
        for (uint j = 0; j < blocks[i].nreserved; ++j)
            printf("%d\n", blocks[i].entries[j].num);
        string tm;
        cin >> tm;

        // Add the sorted block to queue for merging
        pq.push(blocks[i]);
    }

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

void print_block_data(block_t &block) {
    bool sorted = true;
    int invalid_records = 0;
    int first, last;
    record_t record, prev_rec;
    for (uint r = 0; r < block.nreserved; ++r) {
        record = block.entries[r];
        if (!record.valid)
            ++invalid_records;
        if (r != 0 && record.num < prev_rec.num) {
            sorted = false;
            break;
        }
        prev_rec = record;
    }
    first = block.entries[0].num;
    last = block.entries[99].num;
    printf("Block id: %d\tSorted: %d\tValues: (%d, %d)\n", block.blockid, sorted, first, last);
    cout << "Nreserved: " << block.nreserved << "\tDummy: "
         << block.dummy << "\tNext block id: " << block.next_blockid << "\tInvalid records: " << invalid_records << endl;
}

char *is_sorted(char *filename) {

    ifstream infile(filename, ios::in | ios::binary);

    infile.seekg(0, infile.end);
    int block_count = infile.tellg() / sizeof(block_t);
    infile.seekg(0, infile.beg);
    printf("Block count: %d\n", block_count);
    block_t block;
    record_t pr, nr;
    bool sorted = true;
    // Assuming that the file is properly formatted.
    for (int b = 0; b < block_count; ++b) {
        infile.read((char*) &block, sizeof (block_t)); // read block from file
        for (uint r = 0; r < MAX_RECORDS_PER_BLOCK; ++r) {
            nr = block.entries[r];
            if (b != 0 && r != 0 && nr.num < pr.num) {
                sorted = false;
                break;
            }
            pr = nr;
            if(!sorted) break;
        }
    }

    infile.close();
    string s;
    if(sorted) s = "TRUE!"; else s = "FALSE!";
    return (char*)s.c_str();
}

