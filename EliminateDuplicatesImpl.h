/*
 * File:   EliminateDuplicatesImpl.h
 * Author: geohar
 *
 * Created on June 15, 2014, 7:25 PM
 */

#ifndef ELIMINATEDUPLICATESIMPL_H
#define	ELIMINATEDUPLICATESIMPL_H

#include "dbtproj.h"
#include <fstream>

void eliminateDuplicatesImpl(char *infile, unsigned char field, block_t *buffer,
        unsigned int nmem_blocks, char *outfile, unsigned int *nunique, unsigned int *nios);

int testRecordsForDuplicates(block_t *buffer, int read_count,
        record_t currentRecord, block_t currentBlock, std::ofstream &output,
        unsigned int *nunique, unsigned int *nios, unsigned char field);


#endif	/* ELIMINATEDUPLICATESIMPL_H */

