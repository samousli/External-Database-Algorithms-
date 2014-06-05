/*
 * File:   Tests.cpp
 * Author: avail
 *
 * Created on May 25, 2014, 11:20 PM
 */

#include "Tests.h"
#include "ComparisonPredicates.h"
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
#include <sys/time.h>
#include <sys/resource.h>


using namespace std;

void merge_sort_driver(uint total, uint mem,char *input_file,char *output_file) {
    int nblocks = 1024; // number of blocks in the file
    int nmem_blocks = 32;
    cout << nblocks << endl;
   // char input_file[] = "input.bin";
   // char output_file[] = "sorted.bin";

    // Create test input file.
    create_test_file(input_file, nblocks);

    // Create a buffer with the given block count and
    // pass them as arguments for the sorting to take place
    block_t *buffer = new block_t[nmem_blocks];
    //block_t *buffer =(block_t*) calloc(nmem_blocks, sizeof(block_t));
    uint *sorted_segs = new uint(0),
    *passes = new uint(0),
    *ios = new uint(0);

    merge_sort(input_file,1, buffer,
               nmem_blocks, output_file,
               sorted_segs, passes, ios);

   // cout << "Is sorted? " << is_sorted(output_file,1) << endl;
  //  cout << "Sorted segments: " << *sorted_segs << endl
    //     << "IO's: " << *ios << endl
      //   << "Passes: " << *passes << endl;

   // print_file_contents(output_file);
    
    delete[] buffer;
    delete sorted_segs;
    delete passes;
    delete ios;
}


void create_test_file(char *filename, uint nblocks) {
    // generate a file
    ofstream outfile(filename, ios::out | ios::binary);
    block_t block;
    record_t record;
    uint recid = 0;
    // Seed the pseudo-random generator
    srand(time(NULL));

    for (uint b = 0; b < nblocks; ++b) { // for each block

        block.blockid = b;
        for (int r = 0; r < MAX_RECORDS_PER_BLOCK; ++r) { // for each record

            // prepare a record
            record.recid = recid++;
            record.num = rand() % 1000000;
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
    infile.open(filename, ios::in | ios::binary);

    infile.seekg(0, infile.end);
    uint block_count = (uint)infile.tellg() / sizeof(block_t);
    infile.seekg(0, infile.beg);
  //  nblocks = block_count;
    printf("Size matches block count: %d\n", block_count);

    int invalid_blocks = 0;
    // Assuming that the file is properly formatted.
    for (uint b = 0; b < block_count; ++b) {
        //infile.read((char*) &block, sizeof (block_t)); // read block from file
        uint *x = (uint*)calloc(1, sizeof(uint));
        read_block(infile, b, &block, x);
        free(x);
        if (block.valid) {
            print_block_data(block);
        } else {
            ++invalid_blocks;
        }
    }


    infile.close();
    printf("Invalid blocks: %d\n", invalid_blocks);
    printf("Block count: %d\n", block_count);
    cout << "Is sorted?" << is_sorted(filename,1) << endl;
}
bool test_duplicates (char *infile,int nblocks,unsigned char field){
    block_t block;
    record_t record;
    record_t nextRecord;
    ifstream input(infile,ios::in | ios::binary);
    for(int i=0;i<nblocks;i++){ // for each block
        input.read((char*)&block,sizeof(block_t)); // read block
        for(int y=0;y<block.nreserved;y++){ // for each record 
            if(i == 0 && y == 0){ // if first record initialize record variable 
                record = block.entries[0];
            }
            else{ // not first record 
                nextRecord = block.entries[y];
                int compare = compareField(record,nextRecord,field);
                if(compare == 0){ // duplicate founded . return false 
                    return false; 
                }
                else{
                    record = nextRecord; // initialize current record 
                }
                    
            }
        }
    }
    return true; // Duplicates elinimated 
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
    uint first, last;
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

bool is_sorted(char *filename,unsigned char field) {

    ifstream infile(filename, ios::in | ios::binary);

    infile.seekg(0, infile.end);
    uint block_count = (uint) infile.tellg() / sizeof(block_t);
    infile.seekg(0, infile.beg);
    block_t block;
    record_t pr, nr;
    bool sorted = true;
    // Assuming that the file is properly formatted.
    for (uint b = 0; b < block_count; ++b) {
        infile.read((char*) &block, sizeof (block_t)); // read block from file
        int end  = block.nreserved; // end of each block 
        for (uint r = 0; r < end ; ++r) {
            nr = block.entries[r];
            if (b != 0 && r != 0) {
                if(field == 0){ // comparator = recid
                    if(nr.recid < pr.recid){
                        sorted = false;
                        break;
                    }
                }
                else if(field == 1){ // comparator = num 
                   if(nr.num < pr.num){
                        sorted = false;
                        break;
                    }
                }
                else if(field == 2){ // comparator = str 
                    if(strcmp(nr.str,pr.str) == -1){
                        sorted = false;
                        break;
                    }
                }
                // << nr.num << " had to be greater than " << pr.num << endl;
            }
            pr = nr;
            if(!sorted) break;
        }
    }

    infile.close();
    return sorted ;
}


uint file_block_count(char *file) {
    ifstream s(file, ios::in | ios::ate);
    uint c = (uint) s.tellg() / sizeof(block_t);
    s.close();
    return c;
}


double get_cpu_time(void) {
    timeval tim;
    rusage ru;
    getrusage(RUSAGE_SELF, &ru);
    tim=ru.ru_utime;
    double t=(double)tim.tv_sec + (double)tim.tv_usec / 1000000.0;
    tim=ru.ru_stime;
    t+=(double)tim.tv_sec + (double)tim.tv_usec / 1000000.0;
    return t;
}

int testRecordsForDuplicates(int startIteration,block_t *buffer,int read_count,record_t currentRecord,
                             block_t currentBlock,ofstream &output,unsigned int *nunique,unsigned int *nios,unsigned char field){
    
    for(int y=0;y<read_count;y++){ // iterate buffer 
            for(int k=startIteration;k<buffer[y].nreserved;k++){ // for each block iterate records 
                    record_t nextRecord = buffer[y].entries[buffer[y].dummy++]; // temporary storage of next record 
                    int compare = compareField(currentRecord,nextRecord,field);
                    if(compare == -1 || compare == 1){ // if records are not equal 
                        if(compare == 1){
                            cout<<"Error with File Sorting . Founded record value bigger than next record value .Exit !! "<<endl;
                            return 0;
                        }
                        currentRecord = nextRecord; // make current record the next record that retrieved before 
                        serialize_record(output,currentBlock,currentRecord,nios);
                        ++(*nunique); // found unique record 
                    }
                
            }
            startIteration = 0; // start from first record 
        }
    return 1;
}

int compareField(record_t rec1,record_t rec2,unsigned char field){
    switch(field){
        case 0 :
            if(rec1.recid > rec2.num) return 1;
            else if(rec1.recid == rec2.recid) return 0;
            else return -1; 
        case 1 :
            if(rec1.num > rec2.num) return 1;
            else if(rec1.num == rec2.num) return 0;
            else return -1;
        case 2 :
            if(strcmp(rec1.str,rec2.str) == 1) return 1;
            else if(strcmp(rec1.str,rec2.str) == 0) return 0;
            else return -1;
        case 3 : 
            if(rec1.num > rec2.num) return 1;
            else if(rec1.num == rec2.num)
                if(strcmp(rec1.str,rec2.str) == 1) return 1;
                else if(strcmp(rec1.str,rec2.str) == 0) return 0;
                else return -1;
            else return -1;
    }
}
   int retrieveRecord(block_t *block,record_t *record,ifstream &input,unsigned int *nios,unsigned int *iterator){
         if(block->dummy < block->nreserved){ 
                *record = block->entries[block->dummy++]; // go to next record 
            }
            else { // change block from file 
                if(input.eof()){
                    return 0 ; // if end of file , break;
                }
                input.read((char*)block,sizeof(block_t));
                *record = block->entries[block->dummy++];
                ++(*nios);
            }
         ++(*iterator);
      return 1;      
}
