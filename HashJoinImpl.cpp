#include "HashJoinImpl.h"
#include "Tests.h"
#include "ComparisonPredicates.h"

#include <iostream>
#include <unordered_set>
#include <cstring>
#include <fstream>
#include <functional>

using namespace std;


struct recordHash {
    unsigned char _field = 0;

    recordHash(unsigned char field) : _field(field) {}

    size_t operator()( record_t const& r) const {

        switch (_field) {
        case 0:
            return hash<uint>()(r.recid);
        case 1:
            return hash<uint>()(r.num);
        case 2:
            return hash<string>()(string(begin(r.str), end(r.str)-1));
        case 3:
            size_t h1 = hash<uint>()(r.num);
            size_t h2 = hash<string>()(string(begin(r.str), end(r.str)-1));
            return h1 ^ h2; // XOR them
        }
    }
};
struct recordEquals {
    unsigned char _field = 0;

    recordEquals(unsigned char field) : _field(field) {}

    bool operator()( record_t const& l, record_t const& r) const {
        switch (_field) {
        case 0:
            return l.recid == r.recid;
        case 1:
            return l.num == r.num;
        case 2:
            return !strcmp(l.str, r.str);
        case 3:
            return (l.num == r.num) && (!strcmp(l.str, r.str));
        }
    }
};
/* --------------------------------------------------------------------------------------------------------------------------------
   infile1: the name of the first input file
   infile2: the name of the second input file
   field: which field will be used for the join: 0 is for recid, 1 is for num, 2 is for str and 3 is for both num and str
   buffer: pointer to memory buffer
   nmem_blocks: number of blocks in memory
   outfile: the name of the output file
   nres: number of pairs in output (this should be set by you)
   nios: number of IOs performed (this should be set by you)
--------------------------------------------------------------------------------------------------------------------------------
*/
void hash_join (char *infile1, char *infile2, unsigned char field, block_t *buffer,
                uint nmem_blocks, char *outfile, uint *nres, uint *nios) {

    size_t size1 = file_block_count(infile1),
           size2 = file_block_count(infile2);

    ifstream small_file;
    bool first_in = (size1 < size2);

    if (first_in)
        small_file.open(infile1, ios::in | ios::binary);
    else
        small_file.open(infile2, ios::in | ios::binary);


    // Initialize the hash set
    size_t smaller_size =  first_in ? size1 : size2;
    unordered_set<record_t, recordHash, recordEquals>
    hashSet(size1 * smaller_size, recordHash(field), recordEquals(field));

    // Find file size
    small_file.seekg(0, small_file.end);
    uint block_count = small_file.tellg() / sizeof (block_t); // get number of blocks in file
    small_file.seekg(0, small_file.beg);


    //  Fill the buffer
    for (uint i = 0; i < block_count; i += nmem_blocks) {
        // Find the number of blocks that can be read
        uint read_len = nmem_blocks < block_count - i ? nmem_blocks : block_count - i;
        // Read into buffer
        small_file.read((char*) buffer, read_len * sizeof (block_t));
        ++(*nios);

        // For each record
        for(uint b = 0; b < read_len; ++b)
            for(uint r = 0; r < buffer[b].nreserved; ++r)
                hashSet.insert(buffer[b].entries[r]);
    }

    small_file.close();

}


