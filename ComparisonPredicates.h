/* 
 * File:   ComparisonPredicates.h
 * Author: Sahin
 *
 * Created on May 23, 2014, 5:27 PM
 */

#ifndef COMPARISONPREDICATES_H
#define	COMPARISONPREDICATES_H

#include <cstdio>
#include <cstring>
#include "dbtproj.h"

struct compare_block {
    unsigned char _field;
    bool _greater;

    /**
     * Default constructor
     * @param field which field will be used for sorting: 0 is for recid, 1 is for num, 2 is for str and 3 is for both num and str
     * @param greater if true, ascending sort, if false descending.
     */
    compare_block(unsigned char field = 1, bool greater = false)
    : _field(field), _greater(greater) {
    }

    // Copy constructor

    compare_block(const compare_block& other)
    : _field(other._field), _greater(other._greater) {
    }

    // Assignment operator  

    compare_block& operator =(const compare_block &e) {
        if (&e != this) {
            _field = e._field;
            _greater = e._greater;
        }
        return *this;
    }

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

