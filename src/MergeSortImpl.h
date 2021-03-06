/*
 * File:   MergeSortImpl.h
 * Author: avail
 *
 * Created on May 25, 2014, 7:59 PM
 */

#ifndef MERGESORTIMPL_H
#define	MERGESORTIMPL_H

#include "dbtproj.h"
#include <iostream>
#include <queue>

typedef unsigned int uint;

void merge_sort(char *input_file, unsigned char field, block_t *buffer, uint nmem_blocks,
                char *output_file, uint *nsorted_segs, uint *npasses, uint *nios);

void mem_merge(std::ofstream &output, block_t *buffer, uint nblocks,
               unsigned char field, uint *nios);

void file_merge(char *input_file, char *output_file, block_t *buffer, block_t *output_block,
                uint nmem_blocks, unsigned char field, uint block_count,
                uint *nsorted_segs, uint *npasses, uint *nios);

void merge_step(std::ifstream &input, std::ofstream &output, block_t *buffer,
                block_t *output_block, uint ways, uint init_block_id,
                uint block_count, uint sorted_seq_len,
                unsigned char field, uint *nios);

// Merge sort isn't in place in increases the number of IO's linearly
// Replacing it with a quick sort.

void quick_sort(block_t *buffer, unsigned char field, uint b1, uint r1, uint b2, uint r2);

std::pair<uint, uint> partition(block_t *buffer, unsigned char field,
                                uint b1, uint r1, uint b2, uint r2);


#endif	/* MERGESORTIMPL_H */

