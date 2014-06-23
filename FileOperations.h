#ifndef FILEOPS_H
#define FILEOPS_H

#include "dbtproj.h"
#include <fstream>


uint file_block_count( char *file);

bool file_exists( char *file);

void read_block(std::ifstream &input, uint block_id, block_t *output_block, uint *nios);

bool serialize_record(std::ofstream &outfile, block_t &block, const record_t &record, uint *nios);

// Generates a partition name for the hash join function
char *gen_partition_name();

// Serializes the block containing the hash bucket once it's full
void serialize_partition(char *filename, block_t &block,
                          const record_t &record, uint *nios);


#endif // FILEOPS_H
