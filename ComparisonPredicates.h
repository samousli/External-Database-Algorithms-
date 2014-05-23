/* 
 * File:   ComparisonPredicates.h
 * Author: avail
 *
 * Created on May 23, 2014, 5:27 PM
 */

#ifndef COMPARISONPREDICATES_H
#define	COMPARISONPREDICATES_H

#include "dbtproj.h"


unsigned char _field = 0; // 0 is for recid, 1 is for num, 2 is for str and 3 is for both num and str
bool _greater = false; // If false, do descending sort


void setComparator(unsigned char field, bool greater) {
    _field = field;
    _greater = greater;
}

struct compare_block_min {
    bool operator()(const block_t& lb, const block_t& rb) {
        bool result;
        record_t l = lb.entries[lb.dummy];
        record_t r = rb.entries[rb.dummy];
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
        }
        return _greater ? result : !result;
    }
}; 

#endif	/* COMPARISONPREDICATES_H */

