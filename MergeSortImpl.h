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

typedef unsigned int uint;

void merge_sort(char *input_file, unsigned char field, block_t *buffer, uint nmem_blocks,
                char *output_file, uint *nsorted_segs, uint *npasses, uint *nios);

void mem_merge(std::ofstream &output, block_t *buffer, uint nblocks,
               unsigned char field, uint *nios);

void file_merge(char *input_file, char *output_file,
                uint nmem_blocks, unsigned char field, uint total_block_count,
                uint *nsorted_segs, uint *npasses, uint *nios);

void serialize_record(std::ofstream & outfile, block_t &block, record_t &record, uint *nios);

block_t read_block(std::ifstream &infile, uint block_id);



#endif	/* MERGESORTIMPL_H */

