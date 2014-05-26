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
#include <cstdlib>
#include <cstring>


using namespace std;

//Method sorts records from buffer and saves them to file

void SortRecords(block_t *buffer, unsigned int nmem_blocks, char *outputPath, unsigned char field) {
    block_t block;
    block.nreserved = 0;
    block.blockid = 0;

    record_t record;
    int size_Of_Blocks = nmem_blocks*MAX_RECORDS_PER_BLOCK;
    int positionForCheck[nmem_blocks]; // array that points each record of block for check
    int stopProcess = 0; // flag for Termination
    for (int i = 0; i < nmem_blocks; i++) {
        positionForCheck[i] = 0; // initialize array to zero (first record of each block)
    }
    while (true) {
        std::pair<int, int> minimumBlock = FindMinimumValue(buffer, nmem_blocks,
                positionForCheck, field);
        cout << minimumBlock.first << " " << minimumBlock.second << endl;
        stopProcess++;
        if (minimumBlock.first == -1 || stopProcess >= size_Of_Blocks) {
            cout << "Min block id: " << minimumBlock.first << endl;
            break;
        }
        if (minimumBlock.second < MAX_RECORDS_PER_BLOCK) {
            record = buffer[minimumBlock.first].entries[minimumBlock.second];
            cout << record.recid << " " << record.num << endl;
            Serialize_Record(outputPath, block, record);
        }



    }
}

//Method finds minimum value of records  . Returns block position of minimum record value

std::pair<int, int> FindMinimumValue(block_t *buffer, unsigned int nmem_blocks, int *positionForCheck, unsigned char field) {
    int position = -1; // position of minimum value
    int minValue = -1; //  minimum value 
    int positionOfRecord = -1;

    for (int i = 0; i < nmem_blocks; i++) {
        if (position == -1) {

            if (field == '1' && buffer[i].valid) { // case of minimum num value
                position = i;
                minValue = buffer[i].entries[positionForCheck[i]].num; // initialize minimum value
                positionOfRecord = positionForCheck[i];
            }

        } else {

            if (field == '1' && buffer[i].valid) { // case of minimum num value
                if (buffer[i].entries[positionForCheck[i]].num < minValue) { // found smaller value than minimum .Change it
                    minValue = buffer[i].entries[positionForCheck[i]].num;
                    position = i;
                    positionOfRecord = positionForCheck[i];

                }
            }

        }
    }
    if (position != -1) {
        positionForCheck[position]++;
        if (positionForCheck[position] >= MAX_RECORDS_PER_BLOCK - 1) {
            buffer[position].valid = false;
        }
    }
    return std::make_pair(position, positionOfRecord);
}

// Keeping streams open may boost performance but doing this for now to simplify things.

void Write_Block(char* filename, block_t &block) {
    ofstream outfile;
    outfile.open(filename, ios::out | ios::binary);
    outfile.seekp(outfile.end);
    outfile.write((char*) &block, sizeof (block_t));
    outfile.close();
}

void Serialize_Record(char *filename, block_t &block, record_t &record) {
    //block.entries[block.nreserved] = record;
    memcpy(&block.entries[block.nreserved], &record, sizeof (record_t));

    cout << "lets see : " << block.nreserved << " "
            << record.recid << " " << record.num << endl;
    block.nreserved++;

    // If block is full, write to file.
    if (block.nreserved == MAX_RECORDS_PER_BLOCK) {
        block.valid = true;
        Write_Block(filename, block);
        block.blockid++;
        block.nreserved = 0;
        block.valid = false;
    }
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
