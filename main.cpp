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