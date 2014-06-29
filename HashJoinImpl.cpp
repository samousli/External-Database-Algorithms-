#include "HashJoinImpl.h"
#include "Tests.h"
#include "ComparisonPredicates.h"
#include "FileOperations.h"

#include <iostream>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <set>
#include <algorithm>

using namespace std;

// converts std::string to a c_str, probably not safe, 1 time uses only.
char *C_STR(string s) {
    return (char*)s.c_str();
}

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

    if (nmem_blocks < 3) {
        cout << "Not enough blocks in memory, cancelling hash join." << endl;
        return;
    }

    // mem_hash_join appends to the output, so the output file must not exist.
    if (file_exists(outfile)) {
        cout << "Output file already exists, removing the existing file." << endl;
        remove(outfile);
    }

    // Find file sizes
    size_t size1 = file_block_count(infile1),
           size2 = file_block_count(infile2);

    ifstream small_file;
    bool first_in = (size1 < size2);
    size_t build_size = first_in ? size1 : size2,
           probe_size = first_in ? size2 : size1;

    if (!first_in)
        swap(infile1, infile2);

    // Generate a random salt, changes for each pair.
    random_device rd;
    mt19937 gen(rd());
    uint salt = gen();

    // If the build file fits into memory, do an in memory hash join and return.
    if (build_size < nmem_blocks - 1) {       // 1 for probe 1 for output block
        mem_hash_join(infile1, infile2, field, buffer, nmem_blocks, outfile, nres, nios, salt);
        return;
    }

    cout << "Mercy" << endl;

    // Find required partition count
    // One block for input, rest for buckets

    // Run partition for both blocks
    vector<hash_pair> pairs = partition(infile1, infile2, field, buffer,
                                          nmem_blocks, build_size, probe_size, nios);

    cout << "Running mem hash join for pairs:\n";
    for (auto e : pairs) {
        salt = gen();
        cout << e.a << ", " << e.b << " salt: " << salt << endl;

        mem_hash_join(C_STR(e.a), C_STR(e.b), field,
                      buffer, nmem_blocks, outfile, nres, nios, salt);
        // Remove the partition files.
        remove(C_STR(e.a));
        remove(C_STR(e.b));
    }
}

/**
 * Partitions the input file to hash buckets, serializes them into different files.
 */
vector<hash_pair> partition(char *infile1, char *infile2, unsigned char field, block_t *buffer,
                            uint nmem_blocks, size_t file1_size, size_t file2_size, uint *nios) {

    vector<hash_pair> hash_pairs;

    // Generate a random salt, changes for each pair.
    random_device rd;
    mt19937 gen(rd());
    uint salt = gen();

    vector<string> parts1 = hash_partition(infile1, file1_size, field, nmem_blocks, buffer, nios, salt),
                     parts2 = hash_partition(infile2, file2_size, field, nmem_blocks, buffer, nios, salt);

    // file name, salt pair
    vector<pair<string, uint> > files1, files2;
    for (uint i = 0; i < parts1.size(); ++i) {
        files1.push_back( make_pair(parts1[i], salt));
        files2.push_back( make_pair(parts2[i], salt));
    }

    // If at least one of them doesn't fit in memory,
    // repartition both and replace the original files.
    for (int i = parts1.size() - 1; i > -1; --i) {

        char *str1 = C_STR(files1[i].first), *str2 = C_STR(files2[i].first);
        size_t size1 = file_block_count(str1), size2 = file_block_count(str2);

        if (size1 > nmem_blocks - 1 || size2 > nmem_blocks - 1) {


            // Generate new salt
            salt = gen();

            // Create partitions
            parts1 = hash_partition(str1, size1, field, nmem_blocks, buffer, nios, salt);
            parts2 = hash_partition(str2, size2, field, nmem_blocks, buffer, nios, salt);

            cout << "Removing: " << (*(files1.begin() + i)).first
                 << " and " << (*(files2.begin() + i)).first << endl;
            cout << "Adding: " << endl;
            for (auto e : parts1)
                cout << "\t" << e << endl;
            cout << "and:" << endl;
            for (auto e : parts2)
                cout << "\t" << e << endl;

            // Remove input files
            files1.erase(files1.begin() + i);
            files2.erase(files2.begin() + i);
            remove(str1);
            remove(str2);

            // Insert partitioned files
            for (uint i = 0; i < parts1.size(); ++i) {
                files1.insert(files1.begin(), make_pair(parts1[i], salt));
                files2.insert(files2.begin(), make_pair(parts2[i], salt));
            }

            // Move iterator back by the number of inserted elements
            // So the check continues from where it was left off.
            // Effectively replacing recursion with a queue model (!?)
            i += parts1.size();
        } else {
            // Every pair that fits into memory will be passed once
            // Adding them as pairs for the mem_hashing
            cout << "Pair: " << files1[i].first << ", " << files2[i].first
                 << " Salt: " << files1[i].second << endl;
            hash_pairs.push_back( hash_pair(files1[i].first, files2[i].first, files1[i].second));
        }
    }

    return hash_pairs;
}

vector<string> hash_partition(char *file, size_t file_size, unsigned char field,
                              uint mem_size, block_t *buffer, uint *nios, uint hash_salt) {

    vector<string> parts;

    // Convenience declarations
    block_t *input_block = buffer;
    block_t *bucket = buffer + 1;
    uint bucket_size = mem_size - 1;

    // Find names for the partitions
    char **names = (char**) malloc(bucket_size * L_tmpnam * sizeof(char) );
    for(uint i = 0; i < bucket_size; ++i) {
        bucket[i].dummy = i; // Hash value
        bucket[i].nreserved = 0;
        names[i] = gen_partition_name();
    }
    // Use maximum possible size for the hash_table
    recordHash r_hash(field, hash_salt, bucket_size);

    ifstream input_stream(file, ios::in | ios::binary);

    //  Fill the buffer
    for (uint i = 0; i < file_size; ++i) {
        // Read a single block
        input_stream.read((char*) input_block, sizeof(block_t));
        ++(*nios);

        // Build the hash
        for(uint r = 0; r < (*input_block).nreserved; ++r) {

            size_t h = r_hash((*input_block).entries[r]);
            serialize_partition(names[h], bucket[h],
                                (*input_block).entries[r], nios);
        }
    }

    // Serialize the unfilled blocks
    for(uint b = 0; b < bucket_size; ++b) {
        if (bucket[b].nreserved) {

            bucket[b].valid = true;
            ofstream f(names[b], ios::app | ios::binary);
            f.write((char*) &bucket[b], sizeof (block_t));
            ++(*nios);
            f.close();
        }
        // Push filenames into the vector in an ascending sort by hash bucket
        parts.push_back(string(names[b]));
    }

    return parts;
}

/**
 * In memory hashing and probing.
 */
void mem_hash_join (char *infile1, char *infile2, unsigned char field, block_t *buffer,
                    uint nmem_blocks, char *outfile, uint *nres, uint *nios, uint hash_salt) {

    // Find file sizes
    size_t build_size = file_block_count(infile1),
           probe_size = file_block_count(infile2);
    if (!build_size || !probe_size) {
        cerr << "Size can't be equal to 0, empty file?" << endl;
    }
    // For convenience
    char * build_file = infile1,
           * probe_file = infile2;

    // Set the smaller file as the build file.
    if (probe_size < build_size) {
        swap(probe_size, build_size);
        swap(probe_file, build_file);
    }


    // Reserve blocks for probing
    // The key will be the hash and the value a pointer to the buffer location
    block_t output_block = *buffer;
    block_t *bucket = buffer + 1;
    block_t *probes = buffer + build_size + 1;

    // Open the build file
    ifstream input_file(build_file, ios::in | ios::binary);

    // Using a hash map to map hashes to the record addresses
    unordered_multimap<size_t, record_t*> hashSet(nmem_blocks);

    // The hash function
    recordHash r_hash(field, hash_salt, build_size * 100);

    // Fill the buffer
    input_file.read((char*) bucket, build_size * sizeof (block_t));
    ++(*nios);

    // Build the hash
    for(uint b = 0; b < build_size; ++b)
        for(uint r = 0; r < bucket[b].nreserved; ++r) {
            size_t h = r_hash(bucket[b].entries[r]);
            hashSet.insert( make_pair(h, &bucket[b].entries[r]) );
        }

    input_file.close();
    ////////////////////////////////////////////////////////////////
    //                          PROBE STAGE                       //
    ////////////////////////////////////////////////////////////////

    // Initialize output block
    memset(&output_block, 0, sizeof(block_t));
    output_block.nreserved = 0;
    output_block.valid = false;
    output_block.blockid = 0;

    // Init new record
    record_t rec;
    memset(&rec, 0, sizeof(record_t));

    // Open streams
    input_file.open(probe_file, ios::in | ios::binary);
    // Appending instead of overwriting
    ofstream output(outfile, ios::app | ios::binary);

    uint probe_at_once = nmem_blocks - build_size - 1;

    // Start probing
    for (uint i = 0; i < probe_size; i += probe_at_once) {
        // Find the number of blocks to be read
        uint read_len = probe_at_once > probe_size - i ? probe_size - i : probe_at_once;
        // Read into buffer
        input_file.read((char*) probes, read_len * sizeof (block_t));
        ++(*nios);
        // For each record
        for (uint b = 0; b < read_len; ++b) {
            for(uint r = 0; r < probes[b].nreserved; ++r) {

                rec = probes[b].entries[r];
                size_t h = r_hash(rec);
                // Bucket index
                size_t bi = hashSet.bucket(h);
                // Iterate through the bucket and output the matching pairs
                for (auto it = hashSet.begin(bi); it != hashSet.end(bi); ++it) {

                   // cout << rec.num << ", " << h << ", " << (*(*it).second).num << endl;
                    record_t r2 = *(*it).second;
                    if (!compareRecords(r2, rec, field) ) {

                        serialize_record(output, output_block, r2, nios);
                        serialize_record(output, output_block, rec, nios);
                        ++(*nres);
                    }
                }
            }
        }
    }

    //input_file.close();

    // Write the last block to file.
    if (output_block.nreserved) {
        output_block.valid = true;
        output_block.dummy = 0;
        output.write((char*)&output_block, sizeof(block_t));
    }
}


