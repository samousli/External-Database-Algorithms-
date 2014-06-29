#include "MergeSortImpl.h"
#include "dbtproj.h"
#include "ComparisonPredicates.h"
#include "FileOperations.h"
#include "PRNG.h"
#include "Tests.h"

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <cstddef>
#include <cstdio>
#include <algorithm>// std::sort
#include <iterator> // std::begin, std::end
#include <utility>  // make_pair


using namespace std;

void merge_sort(char *input_file, unsigned char field, block_t *buffer, uint nmem_blocks,
                char *output_file, uint *nsorted_segs, uint *npasses, uint *nios) {

    // Temp file to hold the initial pass data.
    char tmp_file[] = "temp.bin";

    ifstream input(input_file, ios::in | ios::binary);
    ofstream tmp_out(tmp_file, ios::out | ios::binary);

    if (!input.is_open()) {
        cout << "Merge sort: Invalid input file.\n";
        return;
    }

    // Find file size
    input.seekg(0, input.end);
    uint block_count = input.tellg() / sizeof (block_t); // get number of blocks in file
    input.seekg(0, input.beg);
    //  Fill the buffer and sort the blocks inside
    for (uint i = 0; i < block_count; i += nmem_blocks) {
        // Find the number of blocks that can be read
        uint read_len = nmem_blocks < block_count - i ? nmem_blocks : block_count - i;
        // Read into buffer
        input.read((char*) buffer, read_len * sizeof (block_t));
        ++(*nios);

        // Sort the buffer
        //mem_merge(tmp_out, buffer, read_len, field, nios);
        quick_sort(buffer, field, 0, 0, read_len, buffer[read_len-1].nreserved);
        tmp_out.write((char*)buffer, read_len * sizeof(block_t));
        ++(*nios);

        ++(*nsorted_segs);
    }

    input.close();
    tmp_out.close();

    // If the file could fully sorted, rename the temp into output and terminate
    // Otherwise initiate the merging
    if (block_count <= nmem_blocks)
        rename(tmp_file, output_file);
    else
        file_merge(tmp_file, output_file, buffer, nmem_blocks,
                   field, block_count, nsorted_segs, npasses, nios);
}

/**

*/
void file_merge(char *input_file, char *output_file, block_t *buffer,
                uint nmem_blocks, unsigned char field, uint block_count,
                uint *nsorted_segs, uint *npasses, uint *nios) {

    // k way merge, in other words number of sorted segments produced.
    uint ways = ceil(block_count / (float) nmem_blocks);
    // At most the size of the buffer, memory limitation.
    ways = ways > nmem_blocks ? nmem_blocks : ways;
    uint segment_len = nmem_blocks;
    //cout << "Total block count: " << block_count << endl
    //     << "Segment length: " << segment_len << endl;
    ifstream input;
    ofstream output;
    bool output_file_to_output = true;

    while (segment_len < block_count) {

        uint init_block_id = 0;
        // Ways required for merge
        uint ways_req = ceil(block_count / (float) segment_len);
        ways = ways > ways_req ? ways_req : ways;

        //cout << ways << "-way merge" << endl;

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
        memset(&output_block, 0, sizeof(block_t));
        output_block.nreserved = 0;
        output_block.blockid = init_block_id;
        output_block.next_blockid = init_block_id + 1;

        // Merge all the sorted segments that fit into memory
        while (init_block_id < block_count) {
            merge_step(input, output, buffer, output_block,
                       ways, init_block_id, block_count, segment_len, field, nios);

            init_block_id += ways * segment_len;
            ++(*nsorted_segs);
        }
        // End of the round of passes, increase seq_len and reiterate

        segment_len *= ways;
        ++(*npasses);
        //  segment_len *= 2;
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
                uint ways, uint init_block_id, uint block_count, uint sorted_seq_len,
                unsigned char field, uint *nios) {


    record_t r;
    memset(&r, 0, sizeof(record_t));

    block_t *b;
    block_comparator comp = block_comparator(field, true);

    // Read data and populate the heap
    int heap_size = -1; // Last element index
    for (uint id = init_block_id;
            id < init_block_id + ways * sorted_seq_len;
            id += sorted_seq_len) {
        if (id < block_count) {
            read_block(input, id, &buffer[++heap_size], nios);
        }
    }

    make_heap(buffer, &buffer[heap_size] + 1, comp);

    // Merge
    while (heap_size >= 0) {
        pop_heap(buffer, &buffer[heap_size] + 1, comp);
        b = &buffer[heap_size]; // Just a syntactic shortcut
        r = b->entries[b->dummy++];
        serialize_record(output, output_block, r, nios);

        if (b->dummy < b->nreserved) {
            push_heap(buffer, &buffer[heap_size] + 1, comp);
        } else if (b->next_blockid < block_count
                   && b->next_blockid % sorted_seq_len != 0) {

            // cout << "Queuing: " << b->next_blockid << endl;
            int i = b->next_blockid;
            read_block(input, i, &buffer[heap_size], nios);
            push_heap(buffer, &buffer[heap_size] + 1, comp);
        } else
            --heap_size;
    }
}

/**
 * n-way merge sort.
 */
void mem_merge(ofstream &output, block_t *buffer, uint nblocks, unsigned char field, uint *nios) {
    // Initialize output block
    block_t output_block;
    record_t rec;

    memset(&output_block, 0, sizeof(block_t));
    memset(&rec, 0, sizeof(record_t));

    output_block.nreserved = 0;
    output_block.dummy = 0;
    output_block.blockid = buffer[0].blockid;
    output_block.next_blockid = output_block.blockid + 1;

    block_t *b;
    block_comparator comp = block_comparator(field, true);
    record_comparator rcomp = record_comparator(field, false);
    // Sort each block and fill the queue with the results
    int heap_size;

    heap_size = nblocks - 1;
    for (uint i = 0; i < nblocks; ++i) {
        // C++ style sort
        record_t *first = &buffer[i].entries[0],
                  *last = &buffer[i].entries[buffer[i].nreserved - 1] + 1;
        // does not overflow: [first, last)
        sort(first, last, rcomp);
    }

    make_heap(buffer, &buffer[heap_size] + 1, comp);

    // Merge
    while (heap_size >= 0) {
        pop_heap(buffer, &buffer[heap_size] + 1, comp);
        b = &buffer[heap_size]; // Just a syntactic shortcut

        rec = b->entries[b->dummy++];
        serialize_record(output, output_block, rec, nios);

        if (b->dummy < b->nreserved) {
            push_heap(buffer, &buffer[heap_size] + 1, comp);

        } else
            --heap_size;
    }
}



// (b1, r1) are inclusive, (b2, r2) are exclusive
void quick_sort(block_t *buffer, unsigned char field, uint b1, uint r1, uint b2, uint r2) {
    if (b1 == b2) {
        cout << "Big kahuna!" << endl;
        return;
    }
    // If in same block, don't use quick_sort anymore
    if (b1 == b2 - 1) {
        if (r1 < r2 - 1) {
            // C++ style sort, faster
            record_comparator r_comp(field, false);
            record_t *first = &buffer[b1].entries[r1],
                      *last = &buffer[b1].entries[r2 - 1] + 1;
            sort(first, last, r_comp);
        }
        cout << "Same block, get out!" << endl;
    } else {

        // Partition the buffer with a random pilot and get block and record indices.
        pair<uint, uint> pivot = partition(buffer, field, b1, r1, b2, r2);
        uint bp = pivot.first, rp = pivot.second;
        cout << "Partitioned: (" << bp << ", " << rp << ")\n";
        /*
        for(uint b = b1; b < bp; ++b) {
            // If not the last block, till n_reserved, else till r2
            uint first = 0, last = buffer[b].nreserved;
            if (b == b1) first = r1;
            else if (b == bp) last = rp;

            for (uint r = first; r < last; ++r)
                cout << buffer[b].entries[r].num << "\t";
        }
        cout << endl << "Lesser than: " << buffer[bp].entries[rp].num << endl;
        //*/
        string s;
        //cin >> s;

        quick_sort(buffer, field, b1, r1, bp + 1, rp + 1);  // Inclusive, exclusive
        // Tail optimization
        quick_sort(buffer, field, bp, rp, b2, r2); // Inclusive, exclusive
    }
}

// inclusive, exclusive
// inline
pair<uint, uint> partition(block_t *buffer, unsigned char field, uint b1, uint r1, uint b2, uint r2) {

    if (b1 == b2) {
        cout << "FML" << endl;

    }

    uint pivot_b = b1 + rand(b2 - b1),
         pivot_r = rand(buffer[pivot_b].nreserved) ;
    cout << "Pivot: " << pivot_b << ", " << pivot_r << endl;

    record_comparator r_comp = record_comparator(field, false);

    uint index_b = b1, index_r = r1;

    // Swap index into the end to simplify things
    swap(buffer[pivot_b].entries[pivot_r], buffer[b2 - 1].entries[r2 - 1]);

    for (uint b = b1; b < b2; ++b) {
        // If not the last block, till n_reserved, else till r2
        uint first = 0, last = buffer[b].nreserved;
        if (b == b1) first = r1;
        else if (b == b2 - 1) last = r2 - 1;

        for(uint r = first; r < last; ++r) {
            if (r_comp(buffer[b].entries[r], buffer[pivot_b].entries[pivot_r])) {
                // Swap the 2 files
                swap(buffer[b].entries[r], buffer[index_b].entries[index_r]);

                // Increment index
                if (++index_r == buffer[b].nreserved) {
                    index_r = 0;
                    ++index_b;
                }
            }
        }
    }
    // Swap the pivot back into place
    swap(buffer[pivot_b].entries[pivot_r], buffer[index_b].entries[index_r]);

    return make_pair(index_b, index_r);
}

