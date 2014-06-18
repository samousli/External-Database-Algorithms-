/*
 * File:   MergeJoinImpl.h
 * Author: avail
 *
 * Created on June 15, 2014, 3:12 AM
 */

#ifndef HASHJOINIMPL_H
#define	HASHJOINIMPL_H

#include "dbtproj.h"
#include <fstream>

void hash_join (char *infile1, char *infile2, unsigned char field, block_t *buffer,
                uint nmem_blocks, char *outfile, uint *nres, uint *nios) ;

#endif	/* HASHJOINIMPL_H */

