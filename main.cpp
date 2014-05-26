#include "dbtproj.h"
#include "ComparisonPredicates.h"
#include "HelpingMethods.h"
#include "MergeSortImpl.h"
#include "Tests.h"
#include "Tests.h"

using namespace std;

void MergeSortDriver();

int main(int argc, char** argv) {
    char input_path[] = "input.bin";
    create_test_file(input_path, 4);
    heap_test(input_path, 4);
    
    //merge_sort_driver();
    MergeSortDriver();
    
    return 0;
}

void MergeSortDriver() {
    int nblocks = 10; // number of blocks in the file
    record_t record;
    block_t block;
    uint recid = 0;
    ifstream infile;
    ofstream outfile;
    unsigned char field = '2';
    char file_name[] = "file.bin";

    // Create test input file.
    create_test_file(file_name, nblocks);
   // print_file_contents(file_name, nblocks);

    // Create a buffer with the given block count and 
    // pass them as arguments for the sorting to take place
    int nblocks_buffer = 5;
    block_t *buffer = (block_t*) malloc(nblocks_buffer * sizeof (block_t));


    char resultsFile[] = "results.bin";
    uint *sorted_segs, *passes, *ios;

    cout<<"---------"<<endl;
       MergeSort(file_name, field, buffer,
            nblocks_buffer, resultsFile,
            sorted_segs, passes, ios);
         cout<<"FUCK --------- zatsssss"<<endl;

    print_file_contents("output1.bin",nblocks_buffer);
    free(buffer);
}



