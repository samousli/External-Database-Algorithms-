#include "dbtproj.h"
#include "ComparisonPredicates.h"
#include "HelpingMethods.h"
#include "MergeSortImpl.h"
#include "Tests.h"
#include "Tests.h"


using namespace std;

void MergeSortDriver();
void benchmark();

int main(int argc, char** argv) {
    //char input_path[] = "input.bin";
    //create_test_file(input_path, 4);
    //heap_test(input_path, 4);
    //print_file_contents("output.bin", 0);
    double sah = get_cpu_time();
    merge_sort_driver(1 << 14, 1 << 5);
    sah = get_cpu_time() - sah;
    cout << "Time: " << sah << " seconds" << endl;
    //MergeSortDriver();
    return 0;
}

void benchmark() {
    double sah = get_cpu_time();
    merge_sort_driver(1 << 14, 1 << 14);
    sah = get_cpu_time() - sah;
    double har = get_cpu_time();
    MergeSortDriver();
    har = get_cpu_time() - har;
    cout << "Sah: " << sah << " seconds" << endl
         << "Har: " << har << " seconds" << endl;
}
void MergeSortDriver() {
    int nblocks = 4;
    int nblocks_buffer = 2;
    ifstream infile;
    ofstream outfile;
    unsigned char field = '1';
    char file_name[] = "file.bin";

    // Create test input file.
    create_test_file(file_name, nblocks);

    // Create a buffer with the given block count and
    // pass them as arguments for the sorting to take place

    block_t *buffer = new block_t[nblocks_buffer];


    char resultsFile[] = "results.bin";
    uint *sorted_segs = new uint(0),
    *passes = new uint(0),
    *ios = new uint(0);

    cout<<"---------"<<endl;
    MergeSort(file_name, field, buffer,
              nblocks_buffer, resultsFile,
              sorted_segs, passes, ios);
    cout<<"FUCK --------- zatsssss"<<endl;


    cout << "Is sorted? " << is_sorted(resultsFile) << endl;
    delete[] buffer;
}



