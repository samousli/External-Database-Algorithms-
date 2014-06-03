/*
 * File:   Tests.h
 * Author: avail
 *
 * Created on May 25, 2014, 11:20 PM
 */

#ifndef TESTS_H
#define	TESTS_H

#include "dbtproj.h"
#include <iostream>

typedef unsigned int uint;

void create_test_file(char *filename, uint nblocks);

void print_file_contents(char *filename, uint nblocks = 0);

void heap_test(char *filename, uint nblocks);

void merge_sort_driver(uint total, uint mem,char *input_file,char *output_file);

void print_block_data(block_t &block);

bool is_sorted(char *filename,unsigned char field);

uint file_block_count(char *file);

double get_cpu_time(void);
bool test_duplicates (char *infile,int nblocks);


#endif	/* TESTS_H */

