#include "dbtproj.h"
#include "ComparisonPredicates.h"
#include "MergeSortImpl.h"
#include "Tests.h"
#include <fstream>
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
   // merge_sort_driver(1 << 14, 1 << 5,"input.bin","output.bin");
//    merge_sort_driver(1 << 12, 1 << 8);
    sah = get_cpu_time() - sah;
    cout << "Time: " << sah << " seconds" << endl;
    block_t *buffer;
    int nmem_blocks =4;
    buffer = new block_t[nmem_blocks];
    uint *sorted_segs = new uint(0),
    *passes = new uint(0),
    *ios = new uint(0);
    
    

   // EliminateDuplicates("input.bin",1,buffer,nmem_blocks,"output.bin",sorted_segs,ios);
     // print_file_contents("output.bin");

   // remove("sorted.bin");
   // remove("input.bin");
    
    MergeJoin("input1.bin","input2.bin",1,buffer,nmem_blocks,"output.bin", passes , ios);
    print_file_contents("output.bin");

    return 0;
}

void benchmark() {
    double sah = get_cpu_time();
  //  merge_sort_driver(1 << 14, 1 << 14);
    sah = get_cpu_time() - sah;
    cout << "Sah: " << sah << " seconds" << endl;
         
}




