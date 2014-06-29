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

    double time = get_cpu_time();
    merge_sort_driver(2, 10);
    //merge_join_driver(5, 3);
    //hash_join_driver(1, 2, 10);
    time = get_cpu_time() - time;
    cout << "time: " << time << " seconds" << endl;

    //eliminate_duplicates_driver(10,2);
//    for (uint total = 2; total <= 10; ++total) {
//        for (uint mem = 2; mem <= total; ++mem) {
//
//            eliminate_duplicates_driver(total, mem);
//
//
//        }
//    }

    return 0;
}


