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
#include <vector>

struct hash_pair {

    std::string a, b;
    uint salt;

    hash_pair(std::string t_a, std::string t_b, uint t_h) : a(t_a), b(t_b), salt(t_h) {}
};

void hash_join_bufferless(char *infile1, char *infile2, unsigned char field, block_t *buffer,
                          uint nmem_blocks, char *outfile, uint *nres, uint *nios);

void mem_hash_join (char *build_file, char *probe_file, unsigned char field, block_t *buffer,
                    uint nmem_blocks, char *outfile, uint *nres, uint *nios, uint hash_salt);

void hash_join (char *infile1, char *infile2, unsigned char field, block_t *buffer,
                uint nmem_blocks, char *outfile, uint *nres, uint *nios) ;

std::vector< hash_pair > partition(char *infile1, char *infile2,
        unsigned char field, block_t *buffer, uint nmem_blocks, size_t build_size,
        size_t probe_size, uint *nios);

std::vector<std::string> hash_partition(char *file, size_t file_size, unsigned char field,
                                        uint mem_size, block_t *buffer, uint *nios, uint hash_salt);

#endif	/* HASHJOINIMPL_H */

