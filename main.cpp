#include "dbtproj.h"
#include "ComparisonPredicates.h"
#include "MergeSortImpl.h"
#include "MergeJoinImpl.h"
#include "Tests.h"
#include <fstream>
#include "Tests.h"


using namespace std;

void MergeSortDriver();
void benchmark();

int main(int argc, char** argv) {

    double sah = get_cpu_time();

    eliminate_duplicates_driver(15000,200);
    //merge_sort_driver(9412, 64);

    sah = get_cpu_time() - sah;
    cout << "Time: " << sah << " seconds" << endl;

    return 0;
}

void benchmark() {
    double sah = get_cpu_time();
    //  merge_sort_driver(1 << 14, 1 << 14);
    sah = get_cpu_time() - sah;
    cout << "Sah: " << sah << " seconds" << endl;

}
