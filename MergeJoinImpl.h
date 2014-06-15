/* 
 * File:   MergeJoinImpl.h
 * Author: avail
 *
 * Created on June 15, 2014, 3:12 AM
 */

#ifndef MERGEJOINIMPL_H
#define	MERGEJOINIMPL_H

#include "dbtproj.h"
#include <fstream>
#include "queue"

void mergeJoinImpl(char *infile1, char *infile2, unsigned char field, block_t *buffer,
        unsigned int nmem_blocks, char *outfile, unsigned int *nres, unsigned int *nios);

int compareField(record_t rec1, record_t rec2, unsigned char field);

int retrieveRecord(block_t *block, record_t *record, ifstream &input,
        unsigned int *nios, unsigned int *iterator,uint block_count);



#endif	/* MERGEJOINIMPL_H */

