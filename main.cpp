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

    hash_join_driver(1000, 12000, 100);

    //eliminate_duplicates_driver(10,2);
//    for (uint total = 2; total <= 10; ++total) {
//        for (uint mem = 2; mem <= total; ++mem) {
//            double time = get_cpu_time();
//            eliminate_duplicates_driver(total, mem);
//            time = get_cpu_time() - time;
//            cout << "time: " << time << " seconds" << endl;
//        }
//    }

    return 0;
}


