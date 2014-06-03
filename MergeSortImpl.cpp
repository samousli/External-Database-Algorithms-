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
        uint read_count = nmem_blocks < block_count - i ? nmem_blocks : block_count - i;
        input.read((char*) buffer, read_count * sizeof (block_t));
        ++(*nios);
        
        // Sort the buffer
        mem_merge(tmp_out, buffer, nmem_blocks, field, nios);
        ++(*npasses);
    }
    
    input.close();
    tmp_out.close();
   // print_file_contents(tmp_file);

    if (block_count <= nmem_blocks) {
        rename(tmp_file, output_file);
        return;
    }


    file_merge(tmp_file, output_file, buffer, nmem_blocks, field, block_count,
               nsorted_segs, npasses, nios);
           
}



void file_merge(char *input_file, char *output_file, block_t *buffer,
                uint nmem_blocks, unsigned char field, uint block_count,
                uint *nsorted_segs, uint *npasses, uint *nios) {

    // k way merge, in other words number of sorted segments produced.
    uint ways = ceil(block_count / (float) nmem_blocks);
    // At most the size of the buffer, memory limitation.
    ways = ways > nmem_blocks ? nmem_blocks : ways;
    uint sorted_seq_len = nmem_blocks;
    cout << "Total block count: " << block_count << endl
         << "Blocks in memory: " << nmem_blocks << endl
         << "Chain length: " << sorted_seq_len << endl
         << ways << "-way merge" << endl;

    ifstream input;
    ofstream output;
    bool output_file_to_output = true;

    while (sorted_seq_len < block_count) {

        uint init_block_id = 0;
        // Ways required for merge
        uint ways_req = ceil(block_count / (float) sorted_seq_len);
        ways = ways > ways_req ? ways_req : ways;

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

        // Initialize output block
        block_t output_block;
        output_block.nreserved = 0;
        output_block.blockid = init_block_id;
        output_block.next_blockid = init_block_id + 1;
        while (init_block_id < block_count) {

            merge_step(input, output, buffer, output_block,
                       ways, init_block_id,block_count, sorted_seq_len, field, nios);
            init_block_id += ways * sorted_seq_len;
            
        }
        // End of the round of passes, increase seq_len and reiterate

        sorted_seq_len *= ways;
      //  sorted_seq_len *= 2;
        input.close();
        output.close();
     //   print_file_contents(output_file);
        // Next time the current output will be the input
        output_file_to_output = !output_file_to_output;
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

void merge_step(ifstream &input, ofstream &output, block_t *buffer, block_t &output_block,
                uint ways, uint init_block_id, uint block_count, uint sorted_seq_len, unsigned char field, uint *nios) {
    // Read data and populate the heap
    // runs ways times
    record_t r;
    block_t *b;
    block_comparator comp = block_comparator(field, true);

    int heap_size = -1;  // Last element index
    for (uint id = init_block_id;
            id < init_block_id + ways*sorted_seq_len;
            id+= sorted_seq_len) {
        if(id < block_count){
      //  cout << "Reading: " << id << endl;
        read_block(input, id, &buffer[++heap_size], nios);
        }
    }
    make_heap(buffer, &buffer[heap_size] + 1, comp);

    // Merge
    while (heap_size >= 0) {
        pop_heap(buffer, &buffer[heap_size] + 1, comp);
        b = &buffer[heap_size];  // Just a syntactic shortcut
        r = b->entries[b->dummy++];
        serialize_record(output, output_block, r, nios);

        if (b->dummy < b->nreserved) {
            push_heap(buffer, &buffer[heap_size] + 1, comp);
        } else if ((b->next_blockid % sorted_seq_len != 0 && (b->blockid < block_count)) ) {
            
        //    cout << "Queuing: " << b->next_blockid <<" Block_count : "<<block_count<< endl;
            int i = b->next_blockid;
            read_block(input, i, &buffer[heap_size], nios);
            push_heap(buffer, &buffer[heap_size] + 1, comp);
        } else
            --heap_size;
    }
}

/**
 * Memory resident n-way merge sort.
 */
void mem_merge(ofstream &output, block_t *buffer, uint nblocks,unsigned char field, uint *nios) {
    // Initialize output block
    block_t output_block;
   
    output_block.nreserved = 0;
    output_block.dummy = 0;
    output_block.blockid = buffer[0].blockid;
    output_block.next_blockid = output_block.blockid + 1;

    record_t r;
    block_t *b;
    block_t block;
    block_comparator comp = block_comparator(field, true);
    record_comparator rcomp = record_comparator(field, false);
      // Sort each block and fill the queue with the results
      int heap_size;
    
       heap_size = nblocks-1;
      for (uint i = 0; i < nblocks; ++i) {
        record_t *first = &buffer[i].entries[0],
                  *last = &buffer[i].entries[buffer[i].nreserved - 1] + 1;
        // does not overflow: [first, last)
        sort(first, last, rcomp);
      }
    
        heap_size = nblocks-1;
    
    make_heap(buffer, &buffer[heap_size] + 1, comp) ;

    // Merge
    while (heap_size >= 0) {
        pop_heap(buffer, &buffer[heap_size] + 1, comp);
        b = &buffer[heap_size];  // Just a syntactic shortcut
        
        r = b->entries[b->dummy++];
          //cout<<"write : "<<b->blockid<<" with "<<r.num<<"\t";
        
        int addNewBlock = serialize_record(output, output_block, r, nios);
        
        
        if (b->dummy < b->nreserved){
            push_heap(buffer, &buffer[heap_size] + 1,comp);
            
        }
        else
            --heap_size;
    }
}


void read_block(ifstream &input, uint block_id, block_t *output_block, uint *nios) {
    
        input.seekg(block_id*sizeof(block_t));
    
        input.read((char*) output_block, sizeof(block_t));
        output_block->dummy = 0;
        output_block->next_blockid = block_id + 1;
        ++(*nios);
  
}

int serialize_record(ofstream &outfile, block_t &block, record_t &record, uint *nios) {

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
        ++(*nios);
        return 1;

    }
    return -1;
}



