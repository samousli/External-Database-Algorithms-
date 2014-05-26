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
#include <iterator>
#include <array>


using namespace std;

void merge_sort(char *input_file, unsigned char field, block_t *buffer, uint nmem_blocks,
        char *out_file, uint *nsorted_segs, uint *npasses, uint *nios) {

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
    int block_count = input.tellg() / sizeof (block_t); // get number of blocks in file 
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

    input.close();
    tmp_out.close();
    
    print_file_contents(tmp_file, block_count);
    
    string s;
    cin >> s;

    ifstream tmp_in(tmp_file, ios::in | ios::binary);
    ofstream output(out_file, ios::out | ios::binary);

    // k way merge, in other words number of sorted segments produced.
    uint k = ceil(block_count / float(nmem_blocks));
    // At most the size of the buffer, memory limitation.
    k = k > nmem_blocks ? nmem_blocks : k;

    cout << "total block count: " << block_count << endl
            << "blocks in memory: " << nmem_blocks << endl
            << k << "-way merge" << endl
            << "passes required: " << ceil(block_count / (float) nmem_blocks) << endl;
    (*nsorted_segs) = k;
    // Passes required is equal to chain length
    // which is the length of each input sequence in blocks.
    // At first only nmem_blocks as only they are sorted.
    // first, last?
    file_merge(tmp_in, output, buffer, nmem_blocks, field, k,
            block_count, nsorted_segs, npasses, nios);
    // Remove temp.
    remove(tmp_file);
}

/**
 *  Let P = a priority queue of the sorted lists, sorted by the smallest element in each list
 *  While P is not empty:
 *      Let L = remove the minimum element from P
 *      Remove the first element from L and put it to the output
 *      If L is not empty, add L to P
 */
void file_merge(std::ifstream &input, std::ofstream &output, block_t *buffer,
        uint buffer_size, unsigned char field, uint ways,
        uint block_count, uint *nsorted_segs, uint *npasses, uint *nios) {

    // Initialize output block
    block_t *output_block = (block_t*) calloc(1, sizeof (block_t));
    output_block->nreserved = 0;
    output_block->dummy = 0;
    output_block->blockid = buffer[0].blockid;
    output_block->next_blockid = output_block->blockid + 1;

    // length of each input sequence in blocks.
    // At max nmem_blocks as only they are sequentially sorted.
    uint chain_length = ceil(block_count / (float) ways);
    //cout << "Chain length: " << chain_length << endl;

    priority_queue<block_t, std::vector<block_t>, block_comparator>
            pq(block_comparator(field, true));

    // Sort each block and fill the queue with the results
    for (uint i = 0; i < ways; ++i) {
        ++(*nios);
        block_t b = read_block(input, chain_length * i);
        b.dummy = 0;
        pq.push(b);
    }

    // Merge
    while (!pq.empty()) {
        block_t b = pq.top();
        record_t r = b.entries[b.dummy];
        pq.pop();
        ++b.dummy;

        serialize_record(output, *output_block, r, nios);

        // If the chain contains more elements add them to the heap.
        if (b.dummy < b.nreserved) {
            pq.push(b);

        } else if (b.next_blockid % chain_length != 0) {
            // Min block full1
            // if min_block.next_blockid belongs to the chain add it
            // chain check: id should be less than the first id of the next chain or the total size
            // next_block_id % M != 0 meets both criteria
            ++(*nios);
            b = read_block(input, b.next_blockid);
            b.dummy = 0;
            pq.push(b);
        }
    }

    // Output remaining records
    if (output_block->nreserved != 0) {
        output_block->valid = true;
        output_block->dummy = 0;
        output.write((char*) output_block, sizeof (block_t));

    }
}

/**
 * Memory resident n-way merge sort. 
 * @param output
 * @param buffer
 * @param nblocks
 * @param field
 * @param nios
 */
void mem_merge(ofstream &output, block_t *buffer, uint nblocks,
        unsigned char field, uint *nios) {

    // Initialize output block
    block_t *output_block = (block_t*) calloc(1, sizeof (block_t));
    output_block->nreserved = 0;
    output_block->dummy = 0;
    output_block->blockid = buffer[0].blockid;
    output_block->next_blockid = output_block->blockid + 1;

    // Use priority queue to extract the minimum elements
    priority_queue<block_t, vector<block_t>, block_comparator> pq(block_comparator(field, true));

    // Sort each block and fill the queue with the results
    for (uint i = 0; i < nblocks; ++i) {
        sort(buffer[i].entries, buffer[i].entries + buffer[i].nreserved, record_comparator(field, false));
        pq.push(buffer[i]);
    }

    // Merge
    while (!pq.empty()) {
        block_t b = pq.top();
        record_t r = b.entries[b.dummy];
        pq.pop();
        ++b.dummy;

        serialize_record(output, *output_block, r, nios);

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
    ifstream::streamoff offset = sizeof (block_t) * block_id;
    block_t block;

    input.seekg(offset, input.beg);
    input.read((char*) &block, sizeof (block_t));

    return block;
}

void serialize_record(ofstream& outfile, block_t &block, record_t &record, uint *nios) {

    block.entries[block.nreserved++] = record;
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


