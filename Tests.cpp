/*
 * File:   Tests.cpp
 * Author: avail
 *
 * Created on May 25, 2014, 11:20 PM
 */

#include "Tests.h"
#include "ComparisonPredicates.h"
#include "FileOperations.h"
#include "MergeSortImpl.h"
#include "EliminateDuplicatesImpl.h"
#include "MergeJoinImpl.h"
#include "HashJoinImpl.h"

#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <string>
#include <iostream>
#include <fstream>
#include <math.h>
#include <time.h>
#include <queue>
#include <random>
#include <algorithm>
#include <sys/time.h>
#include <sys/resource.h>

using namespace std;

void merge_sort_driver(uint total, uint mem) {
    uint nblocks = total; // number of blocks in the file
    uint nmem_blocks = mem;
    char input_file[] = "input.bin";
    char output_file[] = "sorted.bin";

    // Create test input file.
    create_test_file(input_file, nblocks);

    // Create a buffer with the given block count and
    // pass them as arguments for the sorting to take place
    block_t *buffer = new block_t[nmem_blocks];

    uint *sorted_segs = new uint(0),
    *passes = new uint(0),
    *ios = new uint(0);

    merge_sort(input_file, 1, buffer, nmem_blocks,
               output_file, sorted_segs, passes, ios);

    bool sorted =is_sorted(output_file, 1);
    cout << "Is sorted? " << boolalpha << sorted  << endl
         << "Sorted segments: " << *sorted_segs << endl
         << "IO's: " << *ios << endl
         << "Passes: " << *passes << endl;

    // print_file_contents(output_file);

    delete[] buffer;
    delete sorted_segs;
    delete passes;
    delete ios;
}

void eliminate_duplicates_driver(uint total, uint mem) {

    uint nblocks = total; // number of blocks in the file
    uint nmem_blocks = mem;
    char input_file[] = "input.bin";
    char output_file[] = "output.bin";

    // Create test input file.
    create_test_file(input_file, nblocks);

    // Create a buffer with the given block count and
    // pass them as arguments for the sorting to take place
    block_t *buffer = new block_t[nmem_blocks];

    uint *uniques = new uint(0),
    *ios = new uint(0);

    eliminateDuplicatesImpl(input_file, 1, buffer,
                            nmem_blocks, output_file, uniques, ios);

    bool elim = test_duplicates(output_file, 1);
    cout << "Duplicates eliminated? " << boolalpha << elim << endl
         << "IO's: " << *ios << endl
         << "Uniques: " << *uniques << endl;



    delete[] buffer;
    delete uniques;
    delete ios;

}

void merge_join_driver(uint total, uint mem) {
    uint nblocks = total; // number of blocks in the file
    uint nmem_blocks = mem;
    char input1_file[] = "input1.bin";
    char input2_file[] = "input2.bin";
    char output_file[] = "output.bin";

    // Create test input file.
    //  create_test_file(input1_file, nblocks);

    // Create test input file.
    // create_test_file(input2_file, nblocks);

    // Create a buffer with the given block count and
    // pass them as arguments for the sorting to take place
    block_t *buffer = new block_t[nmem_blocks];

    uint *nres = new uint(0),
    *ios = new uint(0);

    mergeJoinImpl(input1_file, input2_file, 1, buffer, nmem_blocks, output_file, nres, ios);

    cout << "Merge-Join Output File : " << endl;
    cout<<"output file block_count : "<<file_block_count(output_file)<<endl;
    cout << "Is sorted? " << is_sorted(output_file,1) << endl;
    cout << "IO's: " << *ios << endl
         << "Pairs : " << *nres << endl;
    // print_file_contents(output_file);
    print_contents_to_file(output_file,(char*)"mergejoin.txt");

    delete[] buffer;
    delete nres;
    delete ios;
}

void hash_join_driver(uint total1, uint total2, uint mem) {

    system("rm -f *.bin*");

    char input1[] = "input1.bin",
                    input2[] = "input2.bin";



    // Create a buffer with the given block count and
    // pass them as arguments for the sorting to take place
    block_t *buffer = new block_t[mem];

    char output_h[] = "output_h.bin";
    char output_m[] = "output_m.bin";
    uint *nios_h = new uint(0), *nres_h = new uint(0),
    *nios_m = new uint(0), *nres_m = new uint(0);

    // uint resilience = 0;
    // while (*nres_h == *nres_m) {

    // Create test input files.
    create_test_file(input1, total1);
    create_test_file(input2, total2);

    double time = get_cpu_time();
    hash_join       (input1, input2, 1, buffer, mem, output_h, nres_h, nios_h);
    cout << get_cpu_time() - time << " hash time" << endl;
    time = get_cpu_time();
    mergeJoinImpl   (input1, input2, 1, buffer, mem, output_m, nres_m, nios_m);
    cout << get_cpu_time() - time << " merge time" << endl;
//    ++resilience;
//        if (resilience % 1000 == 0) {
//            cout << "Resilience: " << resilience << endl;
//            string s;
//            cin >> s;
//        }
//    }
//    cout << "Resilience: " << resilience << endl;

    convert_to_txt_file(output_h, (char*) "output_h.txt", *nres_h / 100 + 1);
    convert_to_txt_file(output_m, (char*) "output_m.txt", *nres_m / 100 + 1);

    // print_file_contents(output_h);
    // cout << "======================================================================\n";
    // print_file_contents(output_m);

    cout << "Hash IOs: \t" << *nios_h << "\tPairs: " << *nres_h << endl;
    cout << "Merge IOs:\t" << *nios_m << "\tPairs: " << *nres_m << endl;

    delete[] buffer;
    delete nres_h;
    delete nres_m;
    delete nios_h;
    delete nios_m;
}


/**
 *
 */
void create_test_file(char *filename, uint nblocks) {
    // generate a file
    ofstream outfile(filename, ios::out | ios::binary);

    block_t block;
    record_t record;

    // Because of padding there were uninitialized bytes left behind.
    memset(&block, 0, sizeof(block_t));
    memset(&record, 0, sizeof(record_t));

    uint recid = 0;
    // Seed the pseudo-random generator
    // srand(time(NULL));

    // C PRG does not randomize well due to the time(NULL) seed
    // Using TR1 Mersenne twister
    random_device rd;
    mt19937 gen(rd());


    for (uint b = 0; b < nblocks; ++b) { // for each block

        block.blockid = b;
        for (int r = 0; r < MAX_RECORDS_PER_BLOCK; ++r) { // for each record

            // prepare a record
            record.recid = recid++;
            record.num = gen() % 10000;
            strcpy(record.str, "hello\0"); // put the same string to all records
            record.valid = true;
            //block.entries[r] = record;
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
    uint block_count = (uint) infile.tellg() / sizeof (block_t);
    infile.seekg(0, infile.beg);
    //  nblocks = block_count;
    printf("Size matches block count: %d\n", block_count);

    // Assuming that the file is properly formatted.
    for (uint b = 0; b < block_count; ++b) {
        infile.read((char*) &block, sizeof (block_t)); // read block from file
        //uint *x = (uint*) calloc(1, sizeof (uint));
        //read_block(infile, b, &block, x);
        //free(x);
        print_block_data(block);
    }


    infile.close();
}


void print_contents_to_file(char *filename,char *output) {
    uint block_count = file_block_count(filename);
    ifstream input(filename,ios::in | ios::binary);
    ofstream output1(output,ios::out );
    block_t block;
    for(int i=0; i<block_count; i++) {
        input.read((char*)&block,sizeof(block_t));
        output1<<"block : "<<block.blockid<<endl;
        for(int y=0; y<block.nreserved; y++) {
            output1<<block.entries[y].recid<<" "<<block.entries[y].num<<endl;
        }
        output1<<endl;
    }
    input.close();
    output1.close();
}


void convert_to_txt_file(char *input, char *output, uint block_count) {
    ifstream is(input, ios::in | ios::binary);
    ofstream os(output, ios::out);
    block_t b;
    uint n = 0;
    for (uint i = 0; i < block_count; ++i) {
        read_block(is, i, &b, &n);
        if (!b.valid) continue;
        for(uint r = 0; r < b.nreserved; ++r) {
            record_t rec = b.entries[r];
            if(rec.valid)
                os << rec.recid << "\t" << rec.num << "\t"
                   << rec.str << "\t" << rec.valid << endl;
        }
    }
    is.close();
    os.close();
}

bool test_duplicates(char *infile, unsigned char field) {
    block_t block;
    record_t record;
    record_t nextRecord;
    ifstream input(infile, ios::in | ios::binary);
    input.seekg(0, input.end);
    uint block_count = (uint) input.tellg() / sizeof (block_t);
    input.seekg(0, input.beg);


    for (uint i = 0; i < block_count; i++) { // for each block
        input.read((char*) &block, sizeof (block_t)); // read block
        for (uint y = 0; y < block.nreserved; y++) { // for each record
            if (i == 0 && y == 0) { // if first record initialize record variable
                record = block.entries[0];
            } else { // not first record
                nextRecord = block.entries[y];
                int compare = compareRecords(record, nextRecord, field);
                if (compare == 0) { // duplicate found. return false
                    cout << "ERROR: " <<  record.num << " matches " << nextRecord.num << endl;
                    cout << "Indexes: " << i << ", " << y << endl;
                    return false;
                } else {
                    record = nextRecord; // initialize current record
                }

            }
        }
    }
    return true; // Duplicates elinimated
}



void print_block_data(block_t &block) {
    int invalid_records = 0;
    record_t record;
    for (uint r = 0; r < block.nreserved; ++r) {
        record = block.entries[r];
        if (!record.valid) {
            ++invalid_records;
            cout << "Invalid record" << endl;
        } else {
            print_record(record);
        }
    }
}

void print_record(record_t &record) {
    cout << record.recid << ") <" << record.num << ", " << record.str << ">\n";
}

bool is_sorted(char *filename, unsigned char field) {

    ifstream infile(filename, ios::in | ios::binary);

    infile.seekg(0, infile.end);
    uint block_count = (uint) infile.tellg() / sizeof (block_t);
    infile.seekg(0, infile.beg);
    block_t block;
    record_t pr, nr;

    record_comparator cmp(field, false);

    // Assuming that the file is properly formatted.
    for (uint b = 0; b < block_count; ++b) {
        infile.read((char*) &block, sizeof (block_t)); // read block from file
        uint end = block.nreserved; // end of each block
        for (uint r = 0; r < end; ++r) {
            nr = block.entries[r];
            if (b != 0 && r != 0) {
                if (!cmp(pr, nr)) {
                    return false;
                }
            }
            pr = nr;
        }
    }

    infile.close();
    return true;
}


double get_cpu_time(void) {
    timeval tim;
    rusage ru;
    getrusage(RUSAGE_SELF, &ru);
    tim = ru.ru_utime;
    double t = (double) tim.tv_sec + (double) tim.tv_usec / 1000000.0;
    tim = ru.ru_stime;
    t += (double) tim.tv_sec + (double) tim.tv_usec / 1000000.0;
    return t;
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
