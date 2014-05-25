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

struct block_comparator {
    unsigned char _field;
    bool _greater;

    /**
     * Default constructor
     * @param field which field will be used for sorting: 0 is for recid, 1 is for num, 2 is for str and 3 is for both num and str
     * @param greater if true, ascending sort, if false descending.
     */
    block_comparator(unsigned char field = 1, bool greater = false)
    : _field(field), _greater(greater) {
    }

    // Copy constructor

    block_comparator(const block_comparator& other)
    : _field(other._field), _greater(other._greater) {
    }

    // Assignment operator  

    block_comparator& operator =(const block_comparator &e) {
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

struct record_comparator {
    unsigned char _field;
    bool _greater;

    /**
     * Default constructor
     * @param field which field will be used for sorting: 0 is for recid, 1 is for num, 2 is for str and 3 is for both num and str
     * @param greater if true, ascending sort, if false descending.
     */
    record_comparator(unsigned char field = 1, bool greater = false)
    : _field(field), _greater(greater) {
    }

    // Copy constructor

    record_comparator(const record_comparator& other)
    : _field(other._field), _greater(other._greater) {
    }

    // Assignment operator  

    record_comparator& operator =(const record_comparator &e) {
        if (&e != this) {
            _field = e._field;
            _greater = e._greater;
        }
        return *this;
    }

    bool operator()(const record_t& l, const record_t& r) {
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
        }
        return _greater ? result : !result;
    }

};

#endif	/* COMPARISONPREDICATES_H */

