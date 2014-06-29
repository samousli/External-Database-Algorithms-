#include "MergeJoinImpl.h"
#include "MergeSortImpl.h"
#include "dbtproj.h"
#include "ComparisonPredicates.h"
#include "FileOperations.h"
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <cstring>

using namespace std;

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
    block_t *buffer1; // buffer for first input (initial buffer that splitted)
    block_t *buffer2; // buffer for second input (initial buffer that splitted)
    block_t firstBlock; // retrieved block from first sorted file
    block_t secondBlock; // retrieved block from second sorted file
    record_t firstRecord; // retrieved record from first sorted file
    record_t pairRecord ; // variable saves current record from first input to examine other records from first input
    record_t secondRecord; // retrieved record from second sorted file
    block_t outputBlock; // block to write results to output file
    outputBlock.blockid = 0; // initialize block id to zero
    uint *buffer1_fill_size = new uint(0); // buffer size for first input 
    uint *buffer2_fill_size = new uint(0); // buffer size for second input 
    uint *buffer1_iterator = new uint(0); // first buffer iterator
    uint *buffer2_iterator = new uint(0); // second buffer iterator
    outputBlock.valid = false; // initialize valid to false
    outputBlock.nreserved = 0; // initialize nreserved to zero


    // Initializing all the bytes of the output block
    // in order to silence Valgrind. (Caused from implicit struct padding)
    memset(&outputBlock, 0, sizeof(block_t));

    char output1[] = "sorted1.bin", output2[] = "sorted2.bin";

    uint *dummy = new uint(0);
    //Sort files
    merge_sort(infile1, field, buffer, nmem_blocks, output1, dummy, dummy, nios);
    merge_sort(infile2, field, buffer, nmem_blocks, output2, dummy, dummy, nios);
    // -----------
    
    uint first_block_count = file_block_count(output1);
    uint second_block_count = file_block_count(output2);
    
    *buffer1_fill_size = nmem_blocks / 2; // calculate first buffer size 
    *buffer2_fill_size = nmem_blocks - (*buffer1_fill_size); // calculate second buffer size
    buffer1 = new block_t[(*buffer1_fill_size)]; // initialize first buffer 
    buffer2 = new block_t[(*buffer2_fill_size)]; // initialize second buffer 

    //    print_contents_to_file(output1,"output1.txt");
    //  print_contents_to_file(output2,"output2.txt");

//   cout << "Sorted 1 : " << first_block_count << endl;
    //  cout << "Sorted 2 : " << second_block_count << endl;

    ifstream input1(output1, ios::in | ios::binary); // open first sorted file
    ifstream input2(output2, ios::in | ios::binary); // open second sorted file
    ofstream output(outfile, ios::out | ios::binary); // open output file (join from two files)


    input1.read((char*)buffer1,(*buffer1_fill_size)*sizeof (block_t)); // read first input file to buffer
    ++(*nios);
    input2.read((char*)buffer2,(*buffer2_fill_size)*sizeof (block_t)); // read first block from file
    ++(*nios);
    firstBlock = buffer1[(*buffer1_iterator)];
    ++(*buffer1_iterator);
    firstRecord = firstBlock.entries[firstBlock.dummy++]; // get first record from file
    secondBlock = buffer2[(*buffer2_iterator)];
    ++(*buffer2_iterator);
    secondRecord = secondBlock.entries[secondBlock.dummy++]; // get first record from file
    
    while ((*firstIterator) < first_block_count && (*secondIterator) < second_block_count) { // while any of two files reached eof
        int compare = compareRecords(firstRecord, secondRecord, field); // compare records
        if (compare == -1) {// first record value is smaller than second record value
            int result = retrieveRecord(buffer1,&firstBlock, &firstRecord, input1, nios, firstIterator,buffer1_iterator,buffer1_fill_size,first_block_count);
            if (result == 0) { // file eof
                break;
            }
        } else if (compare == 1) { // first record value is bigger than second record value
            int result = retrieveRecord(buffer2,&secondBlock, &secondRecord, input2, nios, secondIterator,buffer2_iterator,buffer2_fill_size,second_block_count);

            if (result == 0) {// file eof
                break;
            }
        } else { // first record value is equal with  second record value


            pairRecord = firstRecord; // initialize pairRecord for comparison

            serialize_record(output, outputBlock, firstRecord, nios); //write first record to file
            serialize_record(output, outputBlock, secondRecord, nios); // write second record to file
            ++(*nres);
            do { //while loop to examine for same values
                int result = retrieveRecord(buffer2,&secondBlock, &secondRecord, input2, nios, secondIterator,buffer2_iterator,buffer2_fill_size,second_block_count); // keep record from first file and retrieve from second file
                if (result == 0) {// file eof
                    break;
                }
                if (compareRecords(firstRecord, secondRecord, field) != 0) { // different value from that was written to file
                    do { // search to first file for same values
                        int result = retrieveRecord(buffer1,&firstBlock,&firstRecord,input1,nios,firstIterator,buffer1_iterator,buffer1_fill_size,first_block_count);//read next record from first file
                        if(result == 0) { // file eof
                            break;
                        } else {
                            if(compareRecords(pairRecord,firstRecord,field) == 0) { // same value . add it to output
                                serialize_record(output, outputBlock, firstRecord, nios);
                                ++(*nres);
                            } else { // not same value . go to next record  . break while loop
                                break;
                            }
                        }
                    } while(true);

                    break; // break while loop . go to next iteration

                } else { // founded same value with that was written to file
                    serialize_record(output, outputBlock, secondRecord, nios);
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

    delete firstIterator;
    delete secondIterator;
    delete dummy;
    input1.close(); // close input stream
    input2.close(); // close input stream
    output.close(); // close output stream
}


int retrieveRecord(block_t *file_buffer,block_t *block, record_t *record, ifstream &input,
                   unsigned int *nios, unsigned int *iterator,uint *buffer_iterator,uint *buffer_size,uint block_count) {

    if (block->dummy < block->nreserved) {
        *record = block->entries[block->dummy++]; // go to next record

    } else { // change block from file
        if (*iterator >= block_count) {
            return 0; // if end of file , return zero;
        }
        if(*buffer_iterator < *buffer_size){ // read block from buffer
            *block = (file_buffer[(*buffer_iterator)]);
            ++(*buffer_iterator);
            *record = block->entries[block->dummy++];
            ++(*iterator);
        }
        else{ // all buffer blocks iterated . read new buffer blocks from file 
            input.read((char*)file_buffer,(*buffer_size)*sizeof (block_t)); // read new buffer blocks
            *buffer_iterator = 0; // reset buffer iterator to zero 
            *block = file_buffer[(*buffer_iterator)];
            ++(*buffer_iterator);
            *record = block->entries[block->dummy++];
            ++(*nios);
            ++(*iterator);
        }
    }
    return 1;
}

