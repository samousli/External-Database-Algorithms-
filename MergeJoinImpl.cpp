#include "Tests.h"
#include "MergeJoinImpl.h"
#include "MergeSortImpl.h"
#include "dbtproj.h"
#include "ComparisonPredicates.h"
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <cstring>

/* ----------------------------------------------------------------------------------------------------------------------
   infile1: the name of the first input file
   infile2: the name of the second input file
   field: which field will be used for the join: 0 is for recid, 1 is for num, 2 is for str and 3 is for both num and str
   buffer: pointer to memory buffer
   nmem_blocks: number of blocks in memory
   outfile: the name of the output file
   nres: number of pairs in output (this should be set by you)
   nios: number of IOs performed (this should be set by you)
   ----------------------------------------------------------------------------------------------------------------------
 */
void mergeJoinImpl(char *infile1, char *infile2, unsigned char field, block_t *buffer,
        unsigned int nmem_blocks, char *outfile, unsigned int *nres, unsigned int *nios) {

    // Merge Sort Both!
    // Iterate linearly and write matching pairs to the output
    uint *firstIterator = new uint(0); // iterator for first sorted file
    uint *secondIterator = new uint(0); // iterator for second sorted file
    uint *remainingBlocks = new uint(0);
    block_t firstBlock; // retrieved block from first sorted file
    block_t secondBlock; // retrieved block from second sorted file
    record_t firstRecord; // retrieved record from first sorted file
    record_t pairRecord ;
    record_t secondRecord; // retrieved record from second sorted file
    block_t outputBlock; // block to write results to output file
    outputBlock.blockid = 0; // initialize block id to zero
    outputBlock.valid = false; // initialize valid to false
    outputBlock.nreserved = 0; // initialize nreserved to zero

    char output1[] = "sorted1.bin", output2[] = "sorted2.bin";

    uint *dummy = new uint(0);
    //Sort files
    merge_sort(infile1, field, buffer, nmem_blocks, output1, dummy, dummy, nios);
    merge_sort(infile2, field, buffer, nmem_blocks, output2, dummy, dummy, nios);
    // -----------

    uint first_block_count = file_block_count(output1);
    uint second_block_count = file_block_count(output2);

    *remainingBlocks = first_block_count;

    cout << "Sorted 1 : " << first_block_count << endl;
    cout << "Sorted 2 : " << second_block_count << endl;

    ifstream input1(output1, ios::in | ios::binary); // open first sorted file
    ifstream input2(output2, ios::in | ios::binary); // open second sorted file
    ofstream output(outfile, ios::out | ios::binary); // open output file (join from two files)


    input1.read((char*) &firstBlock, sizeof (block_t)); // read first block from file
    ++(*nios);
    input2.read((char*) &secondBlock, sizeof (block_t)); // read first block from file
    ++(*nios);
    firstRecord = firstBlock.entries[firstBlock.dummy++]; // get first record from file
    secondRecord = secondBlock.entries[secondBlock.dummy++]; // get first record from file

    while ((*firstIterator) < first_block_count && (*secondIterator) < second_block_count) { // while any of two files reached eof
        int compare = compareField(firstRecord, secondRecord, field); // compare records
        if (compare == -1) {// first record value is smaller than second record value
            int result = retrieveRecord(&firstBlock, &firstRecord, input1, nios, firstIterator,first_block_count);
            if (result == 0) { // file eof
                break;
            }
        }
        else if (compare == 1) { // first record value is bigger than second record value
            int result = retrieveRecord(&secondBlock, &secondRecord, input2, nios, secondIterator,second_block_count);

            if (result == 0) {// file eof
                break;
            }
        } else { // first record value is equal with  second record value


            pairRecord = firstRecord; // initialize pairRecord for comparison

            serialize_record(output, outputBlock, firstRecord, nios); //write first record to file
            ++(*nios);
            serialize_record(output, outputBlock, secondRecord, nios); // write second record to file
            ++(*nios);
            ++(*nres);

            do { //while loop to examine for same values
                int result = retrieveRecord(&secondBlock, &secondRecord, input2, nios, secondIterator,second_block_count); // keep record from first file and retrieve from second file

                if (result == 0) {// file eof
                    break;
                }
                if (compareField(firstRecord, secondRecord, field) != 0) { // different value from that was written to file
                    do{ // search to first file for same values
                      int result = retrieveRecord(&firstBlock,&firstRecord,input1,nios,firstIterator,first_block_count);//read next record from first file
                      if(result == 0){ // file eof
                          break;
                      }
                      else {
                          if(compareField(pairRecord,firstRecord,field) == 0){  // same value . add it to output
                               serialize_record(output, outputBlock, secondRecord, nios);
                               ++(*nios);
                               ++(*nres);
                          }
                          else{ // not same value . go to next record  . break while loop
                              break;
                          }
                      }
                    }while(true);

                    break; // break while loop . go to next iteration

                } else { // founded same value with that was written to file
                    serialize_record(output, outputBlock, secondRecord, nios);
                    ++(*nios);
                    ++(*nres);
                }
            } while (true);

        }

    }
    if (outputBlock.nreserved != 0) { // last block that is not filled with Maximum Records . Write it
        outputBlock.valid = true;
        output.write((char*) &outputBlock, sizeof (block_t));
        ++(*nios);
    }
    cout << "Merge-Join Output File : " << endl;

    cout << "Is sorted? " << is_sorted(outfile, 0) << endl;
    cout << "IO's: " << *nios << endl
            << "Pairs : " << *nres << endl;

    input1.close(); // close input stream
    input2.close(); // close input stream
    output.close(); // close output stream
}

// Tzampa zorizese :P
int shortComp(record_t r1, record_t r2, unsigned char field) {
    record_comparator cmp(field);
    if (cmp(r1, r2)) return  1;
    if (cmp(r2, r1)) return -1;
    return 0;
}

int compareField(record_t rec1, record_t rec2, unsigned char field) {
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
            if (strcmp(rec1.str, rec2.str) == 1) return 1;
            else if (strcmp(rec1.str, rec2.str) == 0) return 0;
            else return -1;
        case 3:
            if (rec1.num > rec2.num) return 1;
            else if (rec1.num == rec2.num)
                if (strcmp(rec1.str, rec2.str) == 1) return 1;
                else if (strcmp(rec1.str, rec2.str) == 0) return 0;
                else return -1;
            else return -1;
        default:
            cerr << "Bad field input" << endl;
            return 0;
    }
}

int retrieveRecord(block_t *block, record_t *record, ifstream &input,

        unsigned int *nios, unsigned int *iterator,uint block_count) {
    if (block->dummy < block->nreserved) {
        *record = block->entries[block->dummy++]; // go to next record

    } else { // change block from file
        if (*iterator >= block_count) {
            return 0; // if end of file , return zero;
        }
        input.read((char*) block, sizeof (block_t));
        *record = block->entries[block->dummy++];
        ++(*nios);

        ++(*iterator);
    }
    return 1;
}
