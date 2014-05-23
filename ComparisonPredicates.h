/* 
 * File:   ComparisonPredicates.h
 * Author: avail
 *
 * Created on May 23, 2014, 5:27 PM
 */

#ifndef COMPARISONPREDICATES_H
#define	COMPARISONPREDICATES_H

unsigned char _field = 0; // 0 is for recid, 1 is for num, 2 is for str and 3 is for both num and str
bool _greater = false; // If false, do descending sort


void setComparator(unsigned char field, bool greater) {
    _field = field;
    _greater = greater;
}

struct compare {
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
                else result = 0;
                break;
        }
        return _greater ? result : !result;
    }
}; 

#endif	/* COMPARISONPREDICATES_H */

