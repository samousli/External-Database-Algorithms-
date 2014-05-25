/* 
 * File:   MergeSortImpl.h
 * Author: avail
 *
 * Created on May 25, 2014, 7:59 PM
 */

#ifndef MERGESORTIMPL_H
#define	MERGESORTIMPL_H


void merge_sort(char *infile, unsigned char field, block_t *buffer, uint nmem_blocks,
        char *outfile, uint *nsorted_segs, uint *npasses, uint *nios);

void mem_merge_sort(block_t *buffer, uint nblocks, unsigned char field, char *outfile, uint *nios);

void serialize_record(char *filename, block_t &block, record_t &record, uint *nios);

void write_block(char* filename, block_t &block);

block_t read_block(char *file, uint block_id);

void file_merge(char *input_file, char *output_file, unsigned char field, uint k_way,
        uint total_block_count, uint *nsorted_segs, uint *npasses, uint *nios);


#endif	/* MERGESORTIMPL_H */

