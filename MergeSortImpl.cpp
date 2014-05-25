#include "dbtproj.h"
#include "ComparisonPredicates.h"
#include "HelpingMethods.h"
#include "MergeSortImpl.h"

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

void merge_sort(char *infile, unsigned char field, block_t *buffer, uint nmem_blocks,
        char *outfile, uint *nsorted_segs, uint *npasses, uint *nios) {

    ifstream input;
    input.open(infile, ios::in | ios::binary);
    if (!input.is_open()) {
        fprintf(stderr, "Merge sort: not a valid input file.\n");
        return;
    }

    // Find block count
    input.seekg(input.end);
    uint size = input.tellg();
    uint block_count = size / sizeof (block_t);
    input.seekg(input.beg);

    // k way merge, in other words number of sorted segments produced.
    uint k = block_count / nmem_blocks;
    (*nsorted_segs) = k;

    //  Separate file into chunks
    char tmp_file[] = "temp.bin";
    //  For each chunk sort and write to temp file
    while (!input.eof()) {
        // Read into buffer
        input.read((char*) buffer, nmem_blocks * sizeof (block_t));
        (*nios)++;

        // Sort the buffer
        mem_merge_sort(buffer, nmem_blocks, field, tmp_file, nios);
        (*npasses)++;
    }

    // Merge the chunks loading each block on demand.
    file_merge(tmp_file, outfile, field, k, block_count, nsorted_segs, npasses, nios);
    //remove(tmp_file);
}

// Memory resident n-way merge sort.

void mem_merge_sort(block_t *buffer, uint nblocks, unsigned char field, char *outfile, uint *nios) {

    // Initialize output block
    block_t out_block;
    out_block.nreserved = 0;
    out_block.blockid = buffer[0].blockid;
    out_block.dummy = 0;

    // Use priority queue to extract the minimum elements
    priority_queue<block_t, vector<block_t>, block_comparator> pq(block_comparator(field, true));

    // Sort each block and fill the queue with the results
    for (uint i = 0; i < nblocks; ++i) {
        sort(buffer[i].entries, buffer[i].entries + buffer[i].nreserved, record_comparator(field, false));
        // for(uint j = 0; j < buffer[i].nreserved; ++j) 
        //      printf("%d\n", buffer[i].entries[j].num);
        // string tm;
        // cin >> tm;
        pq.push(buffer[i]);
    }

    // Merge
    while (!pq.empty()) {
        block_t b = pq.top();
        record_t r = b.entries[b.dummy++];
        pq.pop();

        serialize_record(outfile, out_block, r, nios);

        if (b.dummy < b.nreserved)
            pq.push(b);
    }
}

/**
 *  Let P = a priority queue of the sorted lists, sorted by the smallest element in each list
 *  While P is not empty:
 *      Let L = remove the minimum element from P
 *      Remove the first element from L and put it to the output
 *      If L is not empty, add L to P
 * 
 * @param input_file
 * @param output_file
 * @param field
 * @param k_way
 * @param total_block_count
 * @param nsorted_segs
 * @param npasses
 * @param nios
 */
void file_merge(char *input_file, char *output_file, unsigned char field, uint k_way,
        uint total_block_count, uint *nsorted_segs, uint *npasses, uint *nios) {


    priority_queue<block_t, std::vector<block_t>, block_comparator>
            pq(block_comparator(field, true));

    uint chain_length = ceil(total_block_count / (float) k_way);

    block_t min_block;
    block_t output_block;
    output_block.blockid = 0;
    output_block.dummy = 0;
    record_t min_record;

    // Fill the heap
    for (uint i = 0; i < k_way; ++i) {
        (*nios)++;
        pq.push(read_block(input_file, chain_length * i));
    }

    while (!pq.empty()) {

        min_block = pq.top();
        pq.pop();
        min_record = min_block.entries[min_block.dummy++];

        // Put file to the output buffer once the block fills,
        // It will be written to the output file.
        serialize_record(output_file, output_block, min_record, nios);

        // If the chain contains more elements add them to the heap.
        if (min_block.dummy < min_block.nreserved) {
            pq.push(min_block);

        } else if (min_block.next_blockid % chain_length != 0) {
            // Min block full1
            // if min_block.next_blockid belongs to the chain add it
            // chain check: id should be less than the first id of the next chain or the total size
            // next_block_id % M != 0 meets both criteria
            (*nios)++;
            min_block = read_block(input_file, min_block.next_blockid);
            min_block.dummy = 0;
            pq.push(min_block);
        }
    }
}

/**
 * 
 * @param file input file name.
 * @param block_id the id of the block that should be read.
 * @return the allocated block 
 */
block_t read_block(char *filename, uint block_id) {
    // Find block offset.
    // first block is stored from 0 to size of block_t
    // ..n-th from (n - 1) * sizeof(block_t) to n * sizeof(block_t)
    // seek the distance from the beginning
    ifstream::streamoff offset = sizeof (block_t) * block_id;
    block_t block;

    // Open file, seek position, read block and return it
    // Corresponds to 1 IO! nios++;
    ifstream infile;
    infile.open(filename, ios::in | ios::binary);
    infile.seekg(offset, infile.beg);
    infile.read((char*) &block, sizeof (block_t));
  
    infile.close();
    
    
    

    return block;
}

// Keeping streams open may boost performance but doing this for now to simplify things.

void write_block(char* filename, block_t &block) {
    ofstream outfile;
    outfile.open(filename, ios::out | ios::binary);
    outfile.seekp(outfile.end);
    outfile.write((char*) &block, sizeof (block_t));
    outfile.close();
}

void serialize_record(char *filename, block_t &block, record_t &record, uint *nios) {

    memcpy(&block.entries[block.nreserved], &record, sizeof (record_t));
    block.nreserved++;

    // If block is full, write to file.
    if (block.nreserved == MAX_RECORDS_PER_BLOCK) {
        cout << "HELLOOOO!" << endl;
        block.valid = true;
        block.next_blockid = block.blockid + 1;
        (*nios)++;
        write_block(filename, block);
        block.blockid++;
        block.nreserved = 0;
        block.valid = false;
    }

}


