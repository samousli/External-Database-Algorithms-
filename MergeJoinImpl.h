
#ifndef MERGEJOINIMPL_H
#define	MERGEJOINIMPL_H

#include "dbtproj.h"
#include <fstream>

void mergeJoinImpl(char *infile1, char *infile2, unsigned char field, block_t *buffer,
        unsigned int nmem_blocks, char *outfile, unsigned int *nres, unsigned int *nios);

int retrieveRecord(block_t *file_buffer,block_t *block, record_t *record, std::ifstream &input,
        unsigned int *nios, unsigned int *iterator,uint *buffer_iterator,uint *buffer_size, uint block_count);


#endif	/* MERGEJOINIMPL_H */
