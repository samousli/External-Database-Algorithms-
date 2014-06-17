/*
 * File:   ComparisonPredicates.h
 * Author: Sahin
 *
 * Created on May 23, 2014, 5:27 PM
 */

#ifndef COMPARISONPREDICATES_H
#define	COMPARISONPREDICATES_H

#include <iostream>
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

    bool operator()(const block_t& lb, const block_t& rb);
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

    bool operator()(const record_t& l, const record_t& r);

};


int compareRecords(record_t rec1, record_t rec2, unsigned char field);


#endif	/* COMPARISONPREDICATES_H */

