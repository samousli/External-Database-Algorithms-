/*
 * File:   MergeJoinImpl.h
 * Author: avail
 *
 * Created on June 15, 2014, 3:12 AM
 */

#ifndef HASHJOINIMPL_H
#define	HASHJOINIMPL_H

#include "dbtproj.h"
#include <iostream>
#include <fstream>
#include <climits>
#include <algorithm>
#include <vector>

// A functor that returns the hash of the given field once called.
// The hash depends on the salt given and a modulo can be defined.
struct recordHash {
    unsigned char _field = 0;
    uint _salt;
    uint _modulus = INT_MAX;

    recordHash(unsigned char field, uint salt, uint modulus)
        : _field(field), _salt(salt), _modulus(modulus) {}

    size_t operator()( record_t const& r) const {

        switch (_field) {
        case 0:
            return (std::hash<uint>()(r.recid) ^ _salt) % _modulus;
        case 1:
            return (std::hash<uint>()(r.num) ^ _salt) % _modulus;
        case 2:
            return (std::hash<std::string>()(std::string(std::begin(r.str), std::end(r.str)-1))
                    ^ _salt) % _modulus;
        case 3:
            size_t h1 = std::hash<uint>()(r.num);
            size_t h2 = std::hash<std::string>()(std::string(std::begin(r.str), std::end(r.str)-1));
            return (h1 ^ h2 ^ _salt) % _modulus; // XOR them
        }

        std::cerr << "Invalid field in hash function" << std::endl;
        exit(2);
    }
};

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

