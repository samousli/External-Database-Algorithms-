/* 
 * File:   HelpingMethods.cpp
 * Author: geohar
 * 
 * Created on 22 Μάϊος 2014, 4:12 μμ
 */

#include "HelpingMethods.h"
#include <iostream>
#include "dbtproj.h"
#include <fstream>


using namespace std;

//Method sorts records from buffer and saves them to file

void SortRecords(block_t *buffer, unsigned int nmem_blocks, char *outputPath, unsigned char field) {
    ofstream outfile; // file that saves sorted records 
    outfile.open(outputPath, ios::out | ios::binary); // open output file 
    int positionForCheck[nmem_blocks]; // array that points each record of block for check
    int stopProcess = 0; // flag for Termination
    for (int i = 0; i < nmem_blocks; i++) {
        positionForCheck[i] = 0; // initialize array to zero (first record of each block)
    }
    while (true) {
        int positionOfMinimum = FindMinimumValue(buffer, nmem_blocks, positionForCheck, field); // count block position of minimum record value
        outfile.write((char*) &buffer[positionOfMinimum].entries[positionForCheck[positionOfMinimum]], sizeof (record_t)); // write record to file 
        positionForCheck[positionOfMinimum]++;
        if (positionForCheck[positionOfMinimum] == MAX_RECORDS_PER_BLOCK) {
            buffer[positionOfMinimum].valid = false; // end of block . Change valid to false
        }
        for (int i = 0; i < nmem_blocks; i++) {
            if (buffer[i].valid) {
                stopProcess++; // found valid block 
            }
        }
        if (stopProcess == 0) {
            break; // if all blocks are not valid exit . 
        } else {
            stopProcess = 0; // valid block found .Continue Process
        }
    }
    outfile.close(); // close output file stream
}

//Method finds minimum value of records  . Returns block position of minimum record value

int FindMinimumValue(block_t *buffer, unsigned int nmem_blocks, int *positionForCheck, unsigned char field) {
    int position = -1; // position of minimum value
    int minValue = -1; //  minimum value 
    int positionOfRecord = -1;

    for (int i = 0; i < nmem_blocks; i++) {
        if (position == -1) {
            position = i; // initialize position of minimum value 
            if (field == '0' && buffer[i].valid && buffer[i].entries[positionForCheck[i]].valid) { // case of minimum recid value
                minValue = buffer[i].entries[positionForCheck[i]].recid; // initialize minimum value
                positionOfRecord = positionForCheck[i];
            } else if (field == '1' && buffer[i].valid && buffer[i].entries[positionForCheck[i]].valid) { // case of minimum num value
                minValue = buffer[i].entries[positionForCheck[i]].num; // initialize minimum value
                positionOfRecord = positionForCheck[i];
            }

        } else {
            if (field == '0' && buffer[i].valid) { // case of minimum recid value 
                if (buffer[i].entries[positionForCheck[i]].recid < minValue && buffer[i].entries[positionForCheck[i]].valid) { // found smaller value than minimum .Change it
                    minValue = buffer[i].entries[positionForCheck[i]].recid;
                    position = i;
                    positionOfRecord = positionForCheck[i];

                }
            } else if (field == '1' && buffer[i].valid) { // case of minimum num value
                if (buffer[i].entries[positionForCheck[i]].num < minValue && buffer[i].entries[positionForCheck[i]].valid) { // found smaller value than minimum .Change it
                    minValue = buffer[i].entries[positionForCheck[i]].num;
                    position = i;
                    positionOfRecord = positionForCheck[i];

                }
            }

        }
    }
    buffer[position].entries[positionOfRecord].valid = false; // make record valid = false . 
    return position;
}
//comparator for num compare  (sorting block before Final sorting)

int compare(const void * a, const void * b) {
    record_t *r1 = (record_t*) a;
    record_t *r2 = (record_t*) b;
    int r1value = r1->num;
    int r2value = r2->num;

    return (r1value - r2value);
}


//int compare(const void * a, const void * b) {
//    return  (*(record_t*)a).num - (*(record_t*)b).num;
//}


//Merge
