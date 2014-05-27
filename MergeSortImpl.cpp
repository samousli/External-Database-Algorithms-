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
    while (!input.eof()) {
        // Read into buffer
        input.read((char*) buffer, nmem_blocks * sizeof (block_t));
        ++(*nios);
        // Sort the buffer
        mem_merge(tmp_out, buffer, nmem_blocks, field, nios);
        ++(*npasses);
    }

    // Using a C style buffer becomes impractical.
    // C++ style containers such as vector will store data in heap automatically
    // The only difference being that the address space may not match the buffers
    // initial space. Such find tuned control over the execution does not seem to be
    // a requirement of the assignment though.
    free(buffer);
    buffer = NULL;

    input.close();
    tmp_out.close();

    if (block_count <= nmem_blocks) {
        rename(tmp_file, output_file);
        return;
    }
    file_merge(tmp_file, output_file, nmem_blocks, field, block_count,
            nsorted_segs, npasses, nios);
}

/**
 *  Let P = a priority queue of the sorted lists, sorted by the smallest element in each list
 *  While P is not empty:
 *      Let L = remove the minimum element from P
 *      Remove the first element from L and put it to the output
 *      If L is not empty, add L to P
 */
void file_merge(char *input_file, char *output_file,
        uint nmem_blocks, unsigned char field, uint block_count,
        uint *nsorted_segs, uint *npasses, uint *nios) {

    priority_queue<block_t, std::vector<block_t>, block_comparator> pq(block_comparator(field, true));

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

    (*nsorted_segs) = ways;

    block_t b;
    record_t r;
    ifstream input;
    ofstream output;

    uint init_block_id = 0;
    bool output_file_to_output = true;
    while (sorted_seq_len < block_count) {
        // Files should swap each time
        // And if in the the temp file is the final output, it should be renamed.
        // Can't read and write simultaneously from and to a single file
        // in a care free manner as the file could corrupt unless great care is taken.
        if (output_file_to_output) {
            input.open(input_file, ios::in | ios::binary);
            output.open(output_file, ios::trunc | ios::binary);
        } else {
            input.open(output_file, ios::in | ios::binary);
            output.open(input_file, ios::trunc | ios::binary);
        }
        // Next time the input will be the output
        output_file_to_output = !output_file_to_output;

        while (init_block_id < block_count) {

            // Initialize output block
            block_t output_block;
            output_block.nreserved = 0;
            output_block.blockid = init_block_id;
            output_block.next_blockid = init_block_id + 1;

            // Read data and populate the priority queue
            for (uint i = init_block_id; i < init_block_id + ways; ++i) {
                ++(*nios);
                b = read_block(input, sorted_seq_len * i);
                b.dummy = 0;
                pq.push(b);
            }

            // Merge
            while (!pq.empty()) {
                b = pq.top();
                r = b.entries[b.dummy];
                pq.pop();
                ++b.dummy;

                serialize_record(output, output_block, r, nios);

                // If the chain contains more elements add them to the heap.
                if (b.dummy < b.nreserved) {
                    pq.push(b);

                } else if (b.next_blockid % sorted_seq_len != 0) {
                    // Min block full
                    // if the next block id belongs to the sorted sequence add it to the queue
                    // chain check: id should be less than the first id of the next chain or the total size
                    // next_block_id % M != 0 meets both criteria
                    ++(*nios);
                    block_t new_b = read_block(input, b.next_blockid);
                    new_b.dummy = 0;
                    pq.push(new_b);
                }
            }

            // Output remaining records
            if (output_block.nreserved != 0) {
                output_block.valid = true;
                output_block.dummy = 0;
                output.write((char*) &output_block, sizeof (block_t));
            }

            init_block_id += ways * sorted_seq_len;
        }
        sorted_seq_len *= ways;
        uint ways_req = ceil(block_count / (float) sorted_seq_len);
        ways = ways > ways_req ? ways_req : ways;
        if (ways == 1) {
            cerr << "Ways equals 1..omg" << endl;
            break;
        }
        input.close();
        output.close();
        string s;
        cout << "Input:" << endl;
        print_file_contents(input_file, block_count);
        cin >> s;
        cout << "Output:" << endl;
        print_file_contents(output_file, block_count);
        cin >> s;

    }

    // If temp is the output,
    if (output_file_to_output) {
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

    // Use priority queue to extract the minimum elements
    priority_queue<block_t, vector<block_t>, block_comparator> pq(block_comparator(field, true));

    // Sort each block and fill the queue with the results
    for (uint i = 0; i < nblocks; ++i) {
        if (!buffer[i].valid) cerr << "INVALID BLOCK!";
        sort(buffer[i].entries, buffer[i].entries + buffer[i].nreserved, record_comparator(field, false));
        pq.push(buffer[i]);
    }

    // Merge
    while (!pq.empty()) {
        block_t b = pq.top();
        record_t r = b.entries[b.dummy];
        pq.pop();
        ++b.dummy;

        serialize_record(output, output_block, r, nios);

        if (b.dummy < b.nreserved)
            pq.push(b);
    }
}

/**
 *
 * @param file input file name.
 * @param block_id the id of the block that should be read.
 * @return the allocated block
 */
block_t read_block(ifstream &input, uint block_id) {
    // Find block offset.
    // first block is stored from 0 to size of block_t
    // ..n-th from (n - 1) * sizeof(block_t) to n * sizeof(block_t)
    // seek the distance from the beginning
    ifstream::streamoff offset = sizeof (block_t) * (block_id);
    block_t block;

    input.seekg(offset, input.beg);
    if (!input.eof())
        input.read((char*) &block, sizeof (block_t));
    else
        cerr << "Bad block_id" << endl;
    return block;
}

void serialize_record(ofstream& outfile, block_t &block, record_t &record, uint *nios) {

    block.entries[block.nreserved++] = record;
    // If block is full, write to file.
    if (block.nreserved == MAX_RECORDS_PER_BLOCK) {
        block.valid = true;
        block.dummy = 0;
        print_block_data(block);
        outfile.write((char*) &block, sizeof (block_t));
        ++block.blockid;
        ++block.next_blockid;
        block.nreserved = 0;
        block.valid = false;
    }
}



