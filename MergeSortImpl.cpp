#include "dbtproj.h"
#include "ComparisonPredicates.h"
#include "MergeSortImpl.h"
#include "Tests.h"

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <cstddef>
#include <cstdio>
#include <queue>
#include <algorithm>
#include <array>
#include <iterator>
#include <math.h>

using namespace std;

void merge_sort(char *input_file, unsigned char field, block_t *buffer, uint nmem_blocks,
                char *output_file, uint *nsorted_segs, uint *npasses, uint *nios) {

    // Temp file to hold the initial pass data.
    char tmp_file[] = "temp.bin";

    ifstream input(input_file, ios::in | ios::binary);
    ofstream tmp_out(tmp_file, ios::out | ios::binary);

    if (!input.is_open()) {
        fprintf(stderr, "Merge sort: Invalid input file.\n");
        return;
    }

    // Find file size
    input.seekg(0, input.end);
    uint block_count = input.tellg() / sizeof (block_t); // get number of blocks in file
    input.seekg(0, input.beg);

    //  Fill the buffer and sort the blocks inside
    for (uint i = 0; i < block_count; i += nmem_blocks) {
        // Read into buffer
        input.read((char*) buffer, nmem_blocks * sizeof (block_t));
        ++(*nios);
        // Sort the buffer
        mem_merge(tmp_out, buffer, nmem_blocks, field, nios);
        ++(*npasses);
    }

    input.close();
    tmp_out.close();

    print_file_contents(tmp_file, 0);
    string s;
    cin >> s;
    if (block_count <= nmem_blocks) {
        rename(tmp_file, output_file);
        return;
    }

    file_merge(tmp_file, output_file, buffer, nmem_blocks, field, block_count,
               nsorted_segs, npasses, nios);
}

/**
 *  Let P = a priority queue of the sorted lists, sorted by the smallest element in each list
 *  While P is not empty:
 *      Let L = remove the minimum element from P
 *      Remove the first element from L and put it to the output
 *      If L is not empty, add L to P
 */
void file_merge(char *input_file, char *output_file, block_t *buffer,
                uint nmem_blocks, unsigned char field, uint block_count,
                uint *nsorted_segs, uint *npasses, uint *nios) {

    // length of each input sequence in blocks.
    // At max nmem_blocks as only they are sequentially sorted.
    // Passes required is equal to chain length
    // which is the length of each input sequence in blocks.
    // At first only nmem_blocks as only they are sorted.

    // nmem_blocks == length of sorted sequences
    // ways = count of sorted sequences that fit in to memory

    // k way merge, in other words number of sorted segments produced.
    uint ways = ceil(block_count / (float) nmem_blocks);
    // At most the size of the buffer, memory limitation.
    ways = ways > nmem_blocks ? nmem_blocks : ways;

    uint sorted_seq_len = nmem_blocks;

    cout << "Total block count: " << block_count << endl
         << "Blocks in memory: " << nmem_blocks << endl
         << "Chain length: " << sorted_seq_len << endl
         << ways << "-way merge" << endl;

    block_t b;
    record_t r;
    ifstream input;
    ofstream output;
    block_comparator comp = block_comparator(field, true);


    bool output_file_to_output = true;

    while (sorted_seq_len < block_count) {

        uint init_block_id = 0;

        uint ways_req = ceil(block_count / (float) sorted_seq_len);
        ways = ways > ways_req ? ways_req : ways;

        // Files should swap each time
        // And if in the the temp file is the final output, it should be renamed.
        // Can't read and write simultaneously from and to a single file
        // in a care free manner as the file could corrupt unless great care is taken.
        if (output_file_to_output) {
            input.open(input_file, ios::in | ios::binary);
            output.open(output_file, ios::out | ios::binary);
        } else {
            input.open(output_file, ios::in | ios::binary);
            output.open(input_file, ios::out | ios::binary);
        }

        // Initialize output block
        block_t output_block;
        output_block.nreserved = 0;
        output_block.blockid = init_block_id;
        output_block.next_blockid = init_block_id + 1;

        // For each sequence
        while (init_block_id < block_count) {

            //priority_queue<block_t, std::vector<block_t>, block_comparator> pq(block_comparator(field, true));
            int heap_size = 0;  // Last element index
            // Read data and populate the heap
            for (uint i = init_block_id; i < init_block_id + ways*sorted_seq_len; i+= sorted_seq_len) {
                b = read_block(input, i);
                b.next_blockid = i + 1;
                b.dummy = 0;
                memcpy(&buffer[heap_size++], &b, sizeof(block_t));
                //cout << "Correct input: " << (b.blockid == i)
                //     << "\tAsked for: " << i
                //     << "\tGot: " << b.blockid << endl;
                ++(*nios);
            }
            make_heap(&buffer[0], &buffer[heap_size], comp) ;

            // Merge
            while (heap_size > 0) {
                pop_heap(&buffer[0], &buffer[heap_size--], comp);
                memcpy(&b, &buffer[heap_size], sizeof(block_t));
                r = b.entries[b.dummy++];

                serialize_record(output, output_block, r, nios);

                // If the sequence contains more elements add them to the heap.
                if (b.dummy < b.nreserved) {
                    memcpy(&buffer[heap_size++], &b, sizeof(block_t));
                    push_heap(&buffer[0], &buffer[heap_size], comp);

                } else if (b.next_blockid % sorted_seq_len != 0) {
                    int i = b.next_blockid;
                    memcpy(&buffer[heap_size++], &b, sizeof(block_t));
                    push_heap(&buffer[0], &buffer[heap_size], comp);
                    ++(*nios);
                }
            }
            if (output_block.nreserved != 0) {
                cerr << "HOLA?" << endl;
                string s;
                cin >> s;
            }

            init_block_id += ways * sorted_seq_len;
        }
        // End of the round of passes, increase seq_len and reiterate

        sorted_seq_len *= ways;

        input.close();
        output.close();

        // Next time the current output will be the input
        output_file_to_output = !output_file_to_output;

        cout << "Full pass" << endl;
    }

    // If temp is the output, remove the other file and rename temp.
    // Else just remove the temp file.
    if (!output_file_to_output) {
        remove(input_file);
    } else {
        remove(output_file);
        rename(input_file, output_file);
    }
}

/**
 * Memory resident n-way merge sort.
 */
void mem_merge(ofstream &output, block_t *buffer, uint nblocks,
               unsigned char field, uint *nios) {

    // Initialize output block
    block_t output_block;
    output_block.nreserved = 0;
    output_block.dummy = 0;
    output_block.blockid = buffer[0].blockid;
    output_block.next_blockid = output_block.blockid + 1;

    record_t r;
    block_t *b;
    block_comparator comp = block_comparator(field, true);
    record_comparator rcomp = record_comparator(field, false);

    // Sort each block and fill the queue with the results
    int heap_size = nblocks - 1;
    for (uint i = 0; i < nblocks; ++i) {
        record_t *first = &buffer[i].entries[0],
                  *last = &buffer[i].entries[buffer[i].nreserved-1];
        sort(first, last, rcomp);
    }

    make_heap(&buffer[0], &buffer[heap_size], comp) ;

    // Merge
    while (heap_size > 0) {
        pop_heap(&buffer[0], &buffer[heap_size--], comp);
        b = &buffer[heap_size];  // Just a syntactic shortcut
        memcpy(&r, &b->entries[b->dummy++], sizeof(record_t));
        serialize_record(output, output_block, r, nios);

        if (b->dummy < b->nreserved)
            push_heap(&buffer[0], &buffer[++heap_size], comp);
    }
}


block_t read_block(ifstream &input, uint block_id) {
    // Find block offset.
    // first block is stored from 0 to size of block_t
    // ..n-th from (n - 1) * sizeof(block_t) to n * sizeof(block_t)
    // seek the distance from the beginning
    ifstream::streamoff offset = sizeof (block_t) * (block_id);
    block_t block;
    input.seekg(0, input.end);
    int size = input.tellg();
    input.seekg(offset, input.beg);
    if (offset < size)
        input.read((char*) &block, sizeof (block_t));
    else {
        cerr << "Bad block id" << endl;
        cerr << "Block count: " << size / sizeof(block_t) << endl;
        print_block_data(block);
        string s;
        cin >> s;
    }
    block.next_blockid = block_id + 1;
    block.dummy = 0;
    return block;
}

void serialize_record(ofstream& outfile, block_t &block, record_t &record, uint *nios) {

    memcpy(&block.entries[block.nreserved], &record, sizeof(record_t));
    ++block.nreserved;
    // If block is full, write to file.
    if (block.nreserved == MAX_RECORDS_PER_BLOCK) {
        block.valid = true;
        block.dummy = 0;
        outfile.write((char*) &block, sizeof (block_t));
        ++block.blockid;
        ++block.next_blockid;
        block.nreserved = 0;
        block.valid = false;
    }
}



