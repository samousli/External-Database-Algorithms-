#include "dbtproj.h"
#include "ComparisonPredicates.h"
#include "MergeSortImpl.h"
#include "MergeJoinImpl.h"
#include "Tests.h"
#include <fstream>
#include "Tests.h"
#include <iostream>

using namespace std;

int main(int argc, char** argv) {
	cout << "The size of total blocks and blocks that fit into the memory can be given as arguments." << endl;
    double time = get_cpu_time();

    //convert_to_txt_file("input1.bin", "input1.txt", 0);
    //convert_to_txt_file("input2.bin", "input2.txt", 0);

    long total = 1 << 17, mem = 1 << 15;
    if (argc == 3) {
    	total = atol(argv[1]);
    	mem = atol(argv[2]);
    }
    merge_sort_driver(total, mem);

    time = get_cpu_time() - time;
    cout << "total time: " << time << " seconds" << endl;

    return 0;
}


