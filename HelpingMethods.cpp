/*
 * File:   HelpingMethods.cpp
 * Author: geohar
 *
 * Created on 22 Μάϊος 2014, 4:12 μμ
 */

#include "HelpingMethods.h"
#include "Tests.h"
#include <iostream>
#include "dbtproj.h"
#include <fstream>
#include <cstdlib>
#include <cstring>


using namespace std;

//Method sorts records from buffer and saves them to file

void SortRecords(block_t *buffer, unsigned int nmem_blocks, ofstream& outputfile, unsigned char field) {
    block_t block;
    block.blockid = buffer[0].blockid;
    block.dummy = 0;
    block.nreserved = 0;
    block.next_blockid = block.blockid + 1;

    record_t record;
    int positionForCheck[nmem_blocks]; // array that points each record of block for check
    int stopProcess = 0; // flag for Termination
    for (int i = 0; i < nmem_blocks; i++) {
        positionForCheck[i] = 0; // initialize array to zero (first record of each block)
    }
    while(true) {
        std::pair<int,int> minimumBlock = FindMinimumValue(buffer,nmem_blocks,positionForCheck,field);
        stopProcess++;
        if(minimumBlock.first == -1 ) { // All blocks are visited . Stop !!
            break;
        }
        if(minimumBlock.second <= MAX_RECORDS_PER_BLOCK) {
            record = buffer[minimumBlock.first].entries[minimumBlock.second]; // store record
            Serialize_Record(outputfile,block,record); // write record to block
        }

    }
}

//Method finds minimum value of records  . Returns block position of minimum record value
//and position minimum record value from records array
std::pair<int,int> FindMinimumValue(block_t *buffer, unsigned int nmem_blocks, int *positionForCheck, unsigned char field) {
    int position = -1; // position of minimum value
    int minValue = -1; //  minimum value
    int positionOfRecord = -1;
    char minvalue[STR_LENGTH];

    for (int i = 0; i < nmem_blocks; i++) {
        if (position == -1) {
            if(field == '0' && buffer[i].valid) { // case of minimum recid value
                position = i;
                minValue = buffer[i].entries[positionForCheck[i]].recid; // initialize minimum value
                positionOfRecord = positionForCheck[i];
            } else if (field == '1' && buffer[i].valid ) { // case of minimum num value
                position = i;
                minValue = buffer[i].entries[positionForCheck[i]].num; // initialize minimum value
                positionOfRecord = positionForCheck[i];
            } else if(field == '2' && buffer[i].valid) { // case of minimum str value
                position = i;
                strcpy(minvalue,buffer[i].entries[positionForCheck[i]].str); // initialize minimum value
                positionOfRecord = positionForCheck[i];
            } else if(field == '3' && buffer[i].valid) { // case of minimum num - str value
                position = i;
                minValue = buffer[i].entries[positionForCheck[i]].num; // initialize minimum value
                strcpy(minvalue , buffer[i].entries[positionForCheck[i]].str);
                positionOfRecord = positionForCheck[i];
            }


        } else {
            if(field == '0' && buffer[i].valid) {
                if (buffer[i].entries[positionForCheck[i]].recid < minValue ) { // found smaller value than minimum .Change it
                    minValue = buffer[i].entries[positionForCheck[i]].recid;
                    position = i;
                    positionOfRecord = positionForCheck[i];

                }
            } else if (field == '1'&& buffer[i].valid) { // case of minimum num value
                if (buffer[i].entries[positionForCheck[i]].num < minValue ) { // found smaller value than minimum .Change it
                    minValue = buffer[i].entries[positionForCheck[i]].num;
                    position = i;
                    positionOfRecord = positionForCheck[i];

                }

            } else if(field == '2' && buffer[i].valid) { // case of minimum str value
                if (strcmp(buffer[i].entries[positionForCheck[i]].str , minvalue ) < 0)  { // found smaller value than minimum .Change it
                    strcpy(minvalue , buffer[i].entries[positionForCheck[i]].str);
                    position = i;
                    positionOfRecord = positionForCheck[i];

                }
            } else if(field == '3' && buffer[i].valid) { // case of minimum num - str
                if (buffer[i].entries[positionForCheck[i]].num < minValue ) { // found smaller value than minimum .Change it
                    minValue = buffer[i].entries[positionForCheck[i]].num;
                    position = i;
                    positionOfRecord = positionForCheck[i];

                } else if(buffer[i].entries[positionForCheck[i]].num == minValue) { // found same num value with minimum value . Sort by str value
                    if (strcmp(buffer[i].entries[positionForCheck[i]].str , minvalue ) < 0)  { // found smaller value than minimum .Change it
                        strcpy(minvalue , buffer[i].entries[positionForCheck[i]].str);
                        position = i;
                        positionOfRecord = positionForCheck[i];
                        minValue = buffer[i].entries[positionForCheck[i]].num;
                    }
                }
            }


        }
    }
    if(position != -1) { // found position of minimum value
        positionForCheck[position]++; // go to next record of minimum found for next compare
        if(positionForCheck[position] >= MAX_RECORDS_PER_BLOCK) { // all records visited . valid = false
            buffer[position].valid = false;
        }
    }
    return std::make_pair(position,positionOfRecord);
}

void Serialize_Record(std::ofstream& outfile, block_t &block, record_t record) {
    // block.entries[block.nreserved] = record;
    memcpy(&block.entries[block.nreserved], &record, sizeof (record_t));
    block.nreserved++;
    // If block is full, write to file.
    if (block.nreserved == MAX_RECORDS_PER_BLOCK) {
        block.valid = true;
        outfile.write((char*) &block, sizeof (block_t));
        block.blockid ++;
        block.nreserved = 0;
        block.valid = false;
    }
}

//comparator for num compare  (sorting block before Final sorting)

int compare1(const void * a, const void * b) {
    record_t *r1 = (record_t*) a;
    record_t *r2 = (record_t*) b;
    int r1value = r1->num;
    int r2value = r2->num;

    return (r1value - r2value);
}
//comparator for recid compare  (sorting block before Final sorting)

int compare(const void * a, const void * b) {
    record_t *r1 = (record_t*) a;
    record_t *r2 = (record_t*) b;
    int r1value = r1->recid;
    int r2value = r2->recid;

    return (r1value - r2value);
}
//comparator for str compare  (sorting block before Final sorting)

int compare2(const void * a, const void * b) {
    record_t *r1 = (record_t*) a;
    record_t *r2 = (record_t*) b;
    char *r1value = r1->str;
    char *r2value = r2->str;

    return strcmp(r1value , r2value);
}

//int compare(const void * a, const void * b) {
//    return  (*(record_t*)a).num - (*(record_t*)b).num;
//}


//Merge

