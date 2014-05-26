#include "dbtproj.h"
#include "ComparisonPredicates.h"
#include "HelpingMethods.h"
#include "MergeSortImpl.h"
#include "Tests.h"
#include "Tests.h"

using namespace std;


int main(int argc, char** argv) {
    char input_path[] = "input.bin";
    create_test_file(input_path, 4);
    heap_test(input_path, 4);
    
    //merge_sort_driver();
    
    return 0;
}

void create_test_file(std::string filename, uint nblocks) {
    // generate a file 
    ofstream outfile;
    block_t block;
    record_t record;
    uint recid = 0;
    // Seed the pseudo-random generator
    srand(time(NULL));
    outfile.open("file.bin", ios::out | ios::binary); // open input file 
    for (int b = 0; b < nblocks; ++b) { // for each block

        block.blockid = b;
        for (int r = 0; r < MAX_RECORDS_PER_BLOCK; ++r) { // for each record
            // prepare a record
            record.recid = recid++;
            record.num = rand() % 1000;
            if(r < MAX_RECORDS_PER_BLOCK/2){
              strcpy(record.str, "hello"); // put the same string to all records
            }
            else{
                 strcpy(record.str, "hella"); // put the same string to all records

            }
            record.valid = true;

            memcpy(&block.entries[r], &record, sizeof (record_t)); // copy record to block
        }

        block.nreserved = MAX_RECORDS_PER_BLOCK;
        block.valid = true;

        // Set next block id
        b != nblocks - 1 ? block.next_blockid = b + 1 : block.next_blockid = -1;


        outfile.write((char*) &block, sizeof (block_t)); // write the block to the file
    }
    outfile.close();
}

void print_file_contents(string filename, uint nblocks) {
    ifstream infile;
    block_t block;
    record_t record;
    infile.open(filename, ios::in | ios::binary);
    // Assuming that the file is properly formatted.
    for (int b = 0; b < nblocks; ++b) {
        infile.read((char*) &block, sizeof (block_t)); // read block from file 
        cout<<"size : "<<block.nreserved<<endl;
        for (int r = 0; r < block.nreserved; ++r) {
            record = block.entries[r];
            printf("this record id: %d, num: %d, str: '%s' belongs to block %d\n",
                record.recid, record.num, record.str, block.blockid);
        }
    }
    infile.close();
}

void merge_sort_driver() {
    int nblocks = 10; // number of blocks in the file
    record_t record;
    block_t block;
    uint recid = 0;
    ifstream infile;
    ofstream outfile;
    unsigned char field = '2';
    string file_name = "file.bin";

    // Create test input file.
    create_test_file(file_name, nblocks);
   // print_file_contents(file_name, nblocks);

    // Create a buffer with the given block count and 
    // pass them as arguments for the sorting to take place
    int nblocks_buffer = 5;
    block_t *buffer = (block_t*) malloc(nblocks_buffer * sizeof (block_t));


    // Copy std:string to c_str
    char *path = const_cast<char *> (file_name.c_str());

    char resultsFile[] = "results.bin";
    uint *sorted_segs, *passes, *ios;

    cout<<"---------"<<endl;
       MergeSort(path, field, buffer,
            nblocks_buffer, resultsFile,
            sorted_segs, passes, ios);
         cout<<"FUCK --------- zatsssss"<<endl;

    print_file_contents("output1.bin",nblocks_buffer);
    free(buffer);
}



