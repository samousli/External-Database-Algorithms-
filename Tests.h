/*
 * File:   Tests.h
 * Author: avail
 *
 * Created on May 25, 2014, 11:20 PM
 */

#ifndef TESTS_H
#define	TESTS_H

#include "dbtproj.h"

typedef unsigned int uint;

void create_test_file(char *filename, uint nblocks);

void print_file_contents(char *filename, uint nblocks);

void heap_test(char *filename, uint nblocks);

void merge_sort_driver();

void print_block_data(block_t &block);

char *is_sorted(char *filename);

#endif	/* TESTS_H */

