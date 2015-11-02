#ifndef MERGEJOINIMPL_H
#define	MERGEJOINIMPL_H

#include "dbtproj.h"
#include <fstream>
#include <vector>

void mergeJoinImpl(char *infile1, char *infile2, unsigned char field, block_t *buffer,
                   unsigned int nmem_blocks, char *outfile, unsigned int *nres, unsigned int *nios);

int retrieveRecord(block_t *file_buffer,block_t *block, record_t *record, std::ifstream &input,
                   unsigned int *nios, unsigned int *iterator,uint *buffer_iterator,uint buffer_size, uint block_count);



void merge_join(char *infile1, char *infile2, unsigned char field,
                block_t *buffer, uint nmem_blocks, char *outfile,
                uint *nres, uint *nios);

// Increment the indices, swap buffer if required, return true on swap
bool advance(std::ifstream &input, uint total, uint mem,
             const block_t *buffer, uint &b, uint &r, uint *nios);

// Returns true if there is another block in front
bool is_last_record(const block_t *buffer, uint b, uint r, uint total, uint mem, uint swaps);

#endif	/* MERGEJOINIMPL_H */
