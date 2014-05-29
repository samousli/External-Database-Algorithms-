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
using namespace std;

void merge_sort(char *input_file, unsigned char field, block_t *buffer, uint nmem_blocks,
                char *output_file, uint *nsorted_segs, uint *npasses, uint *nios);

void mem_merge(ofstream &output, block_t *buffer, uint nblocks,
               unsigned char field, uint *nios);

void file_merge(char *input_file, char *output_file, block_t *buffer,
                uint nmem_blocks, unsigned char field, uint block_count,
                uint *nsorted_segs, uint *npasses, uint *nios);

void serialize_record(ofstream &outfile, block_t &block, record_t &record, uint *nios);

void read_block(ifstream &input, uint block_id, block_t &output_block, uint *nios);



#endif	/* MERGESORTIMPL_H */

