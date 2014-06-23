#include "FileOperations.h"
#include "Tests.h"
#include "dbtproj.h"
#include <fstream>
#include <iostream>
#include <cstring>

using namespace std;

/**
 *
 */
uint file_block_count( char *file) {
    ifstream s(file, ios::in);
    s.seekg(0, s.end);
    uint c = (uint) s.tellg() / sizeof (block_t);
    s.close();
    return c;
}

/**
 *
 */
bool file_exists( char *file) {
    ifstream f(file, ios::in);
    bool r = f.is_open();
    if (r) f.close();
    return r;
}

/**
 *
 */
void read_block(ifstream &input, uint block_id, block_t *output_block, uint *nios) {

    input.seekg(block_id * sizeof (block_t));

    input.read((char*) output_block, sizeof (block_t));
    output_block->dummy = 0;
    output_block->next_blockid = block_id + 1;
    ++(*nios);
}

/**
 *
 */
bool serialize_record(ofstream &outfile, block_t &block, const record_t & record, uint *nios) {

    memcpy(&block.entries[block.nreserved++], &record, sizeof(record_t));

    // If block is full, write to file.
    if (block.nreserved == MAX_RECORDS_PER_BLOCK) {
        block.valid = true;
        block.dummy = 0;
        outfile.write((char*) &block, sizeof (block_t));
        ++block.blockid;
        ++block.next_blockid;
        block.nreserved = 0;
        block.valid = false;
        ++(*nios);
        return true;
    }
    return false;
}

// Generates a partition name for the hash join function
char *gen_partition_name()  {

    char * name = new char[80];
    tmpnam(name);
    return name;
}

// Serializes the block containing the hash bucket once it's full
void serialize_partition(char *filename, block_t &block,
                          const record_t &record, uint *nios) {

    memcpy(&block.entries[block.nreserved++], &record, sizeof(record_t));

    // If block is full, write to file.
    if (block.nreserved == MAX_RECORDS_PER_BLOCK) {

        block.valid = true;

        ofstream f(filename, ios::out | ios::binary);
        f.write((char*) &block, sizeof (block_t));
        f.close();

        ++block.blockid;
        ++block.next_blockid;
        block.nreserved = 0;
        block.valid = false;
        ++(*nios);
    }
}


