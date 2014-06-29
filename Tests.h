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

void merge_sort_driver(uint total, uint mem);

void eliminate_duplicates_driver(uint total, uint mem);

void merge_join_driver(uint total, uint mem);

void hash_join_driver(uint total1, uint total2, uint mem);

void create_test_file(char *filename, uint nblocks);

void print_file_contents(char *filename, uint nblocks = 0);

void heap_test(char *filename, uint nblocks);

void print_block_data(block_t &block);

void print_record(record_t &record);

bool is_sorted(char *filename, unsigned char field);

uint file_block_count(char *file);

double get_cpu_time(void);

bool test_duplicates(char *infile, unsigned char field);

void print_contents_to_file(char *filename,char *output);

void convert_to_txt_file(char *input, char *output, uint block_count);


#endif	/* TESTS_H */

