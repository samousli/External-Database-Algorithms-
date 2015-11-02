#include "ComparisonPredicates.h"
#include <cstring>
#include <iostream>

using namespace std;


/**
 * Block comp
 */
bool block_comparator::operator()(const block_t& lb, const block_t& rb) {
    bool result;
    if (lb.dummy > 99 || rb.dummy > 99) {
        std::cerr << "ERROR ! dummy out of bounds" << std::endl
                  << lb.nreserved << ", " << rb.nreserved << std::endl
                  << lb.dummy << ", " << rb.dummy << std::endl;
        std::string s;
        std::cin >> s;
    }
    record_t l = lb.entries[lb.dummy];
    record_t r = rb.entries[rb.dummy];
    switch (this->_field) {
    case 0:
        result = l.recid > r.recid;
        break;
    case 1:
        result = l.num > r.num;
        break;
    case 2:
        result = strcmp(l.str, r.str) > 0;
        break;
    case 3:
        if (l.num >= r.num) result = true;
        else if (l.num == r.num) result = strcmp(l.str, r.str) >= 0;
        else result = false;
        break;
    default:
        result = false;
        std::cerr << "Undefined behavior" << std::endl;
    }
    return this->_greater ? result : !result;
}

/**
 * Record comp
 */
bool record_comparator::operator()(const record_t& l, const record_t& r) {
    bool result;
    switch (_field) {
    case 0:
        result = l.recid > r.recid;
        break;
    case 1:

        result = l.num > r.num;
        break;
    case 2:
        result = strcmp(l.str, r.str) > 0;
        break;
    case 3:
        if (l.num > r.num) result = true;
        else if (l.num == r.num) result = strcmp(l.str, r.str) > 0;
        else result = false;
        break;
    default:
        result = false;
        std::cerr << "Undefined behavior" << std::endl;
    }
    return _greater ? result : !result;
}


/**
 * C-style record comp
 */
int compareRecords(record_t rec1, record_t rec2, unsigned char field) {
    switch (field) {
    case 0:
        if (rec1.recid > rec2.recid) return 1;
        else if (rec1.recid == rec2.recid) return 0;
        else return -1;
    case 1:
        if (rec1.num > rec2.num) return 1;
        else if (rec1.num == rec2.num) return 0;
        else return -1;
    case 2:
        return strcmp(rec1.str, rec2.str);
    case 3:
        if (rec1.num > rec2.num) return 1;
        else if (rec1.num == rec2.num)
            return strcmp(rec1.str, rec2.str);
        else return -1;
    default:
        std::cerr << "Bad field input" << std::endl;
        return 0;
    }
}

unsigned char _FIELD  = 1;

void setComparisonField(unsigned char _field) {
    _FIELD = _field;
}

int shortComp(const void *a, const void *b) {
    record_t *r1 = (record_t*)a,
             *r2 = (record_t*)b;

    return compareRecords(*r1, *r2, _FIELD);
}

