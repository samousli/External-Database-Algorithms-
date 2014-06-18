#include "HashJoinImpl.h"
#include "Tests.h"
#include "ComparisonPredicates.h"

// Should move serialize_record
#include "MergeSortImpl.h"

#include <iostream>
#include <unordered_set>
#include <cstring>
#include <fstream>
#include <functional>

using namespace std;


template<typename T>
class Alloc {
public:
    typedef T value_type;
    typedef value_type* pointer;
    typedef const value_type* const_pointer;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;

private:
    uint load = 0;
    uint buffer_size;
    block_t *_buffer;

public :

    // Dummy
    template<typename U>
    struct rebind {
        typedef Alloc<U> other;
    };

    inline explicit Alloc(T t) { }

    inline explicit Alloc(block_t *buffer, uint nmem_blocks) {
        _buffer = buffer;
        buffer_size = nmem_blocks;
    }

    inline ~Alloc() {}
    inline explicit Alloc(Alloc const&) {}

    // memory allocation
    inline record_t* allocate(size_t cnt,
                               std::allocator<void>::const_pointer hint = 0) {

        if (_buffer[load].nreserved == MAX_RECORDS_PER_BLOCK
                && load < buffer_size)
            ++load;

        return &_buffer[load].entries[_buffer[load].dummy++];

    }

    inline void deallocate(record_t* p, size_t cnt) {

    }

    // size
    inline size_t max_size() const {
        return (buffer_size * MAX_RECORDS_PER_BLOCK);
    }

    // construction/destruction
    inline void construct(pointer p, const T& t) {
        new(p) T(t);
    }
    inline void destroy(pointer p) {
        /* p->~T(); */
    }
};



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
    unordered_multiset<record_t, recordHash, recordEquals/*, Alloc<record_t> */>
    hashSet(size1 * smaller_size, recordHash(field), recordEquals(field) /*, Alloc<record_t>(buffer, nmem_blocks)*/);

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

        // Build the hash
        for(uint b = 0; b < read_len; ++b)
            for(uint r = 0; r < buffer[b].nreserved; ++r)
                hashSet.insert(buffer[b].entries[r]);
    }
    small_file.close();

    // Initialize output block
    block_t output_block;
    memset(&output_block, 0, sizeof(block_t));
    output_block.nreserved = 0;
    output_block.valid = false;
    output_block.blockid = 0;

    record_t rec;
    memset(&rec, 0, sizeof(record_t));


    // Open streams
    if (first_in)
        small_file.open(infile2, ios::in | ios::binary);
    else
        small_file.open(infile1, ios::in | ios::binary);

    ofstream output(outfile, ios::out | ios::binary);

    record_comparator r_comp(field);

    // Start probing
    for (uint i = 0; i < block_count; i += nmem_blocks) {
        // Find the number of blocks that can be read
        uint read_len = nmem_blocks < block_count - i ? nmem_blocks : block_count - i;
        // Read into buffer
        small_file.read((char*) buffer, read_len * sizeof (block_t));
        ++(*nios);

        // For each record
        for(uint b = 0; b < read_len; ++b)
            for(uint r = 0; r < buffer[b].nreserved; ++r) {

                rec = buffer[b].entries[r];
                size_t h = hashSet.bucket(rec);
                // Iterate through the bucket and output the matching pairs
                for (auto it = hashSet.begin(h); it != hashSet.end(h); ++it) {

                    if (compareRecords(*it, rec, field) == 0) {
                        serialize_record(output, output_block, *it, nios);
                        serialize_record(output, output_block, rec, nios);
                        cout << it->num << " matches " << rec.num
                             <<  "\tID's: " << it->recid << ", " << rec.recid << endl;
                        ++(*nres);
                    }
                }
            }
    }

    // Write the last block to file.
    if (output_block.nreserved) {
        output_block.valid = true;
        output_block.dummy = 0;
        output.write((char*)&output_block, sizeof(block_t));
    }
}


