#include "MergeJoinImpl.h"
#include "MergeSortImpl.h"
#include "dbtproj.h"
#include "ComparisonPredicates.h"
#include "FileOperations.h"
#include "Tests.h"

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
    block_t *firstBlock; // retrieved block from first sorted file
    block_t *secondBlock; // retrieved block from second sorted file
    record_t firstRecord; // retrieved record from first sorted file
    record_t pairRecord ; // variable saves current record from first input to examine other records from first input
    record_t secondRecord; // retrieved record from second sorted file
    uint buffer1_fill_size = 0; // buffer size for first input
    uint buffer2_fill_size = 0; // buffer size for second input
    uint buffer1_iterator = 0; // first buffer iterator
    uint buffer2_iterator = 0; // second buffer iterator




    char output1[] = "sorted1.bin", output2[] = "sorted2.bin";

    uint *dummy = new uint(0);
    //Sort files
    merge_sort(infile1, field, buffer, nmem_blocks, output1, dummy, dummy, nios);
    merge_sort(infile2, field, buffer, nmem_blocks, output2, dummy, dummy, nios);
    // -----------
    uint first_block_count = file_block_count(output1);
    uint second_block_count = file_block_count(output2);

    block_t *outputBlock = &buffer[nmem_blocks -1]; // block to write results to output file
    // Initializing all the bytes of the output block
    // in order to silence Valgrind. (Caused from implicit struct padding)
    memset(outputBlock, 0, sizeof(block_t));

    buffer1_fill_size = nmem_blocks/2; // calculate first buffer size
    buffer2_fill_size = nmem_blocks - (buffer1_fill_size) -1; // calculate second buffer size . reduce by one because last block of buffer is for output
    buffer1 = buffer; // initialize first buffer
    buffer2 = &buffer[buffer1_fill_size]; // initialize second buffer reduced size by one because last block of initial buffer is for output

    outputBlock->blockid = 0; // initialize block id to zero
    outputBlock->valid = false; // initialize valid to false
    outputBlock->nreserved = 0; // initialize nreserved to zero

    //print_contents_to_file(output1,"output1.txt");
    // print_contents_to_file(output2,"output2.txt");

    if (!is_sorted(output1, field) || !is_sorted(output2, field) )
        cerr << "Goomba!\n";

    ifstream input1(output1, ios::in | ios::binary); // open first sorted file
    ifstream input2(output2, ios::in | ios::binary); // open second sorted file
    ofstream output(outfile, ios::out | ios::binary); // open output file (join from two files)


    input1.read((char*)buffer1,buffer1_fill_size*sizeof (block_t)); // read first input file to buffer
    ++(*nios);
    input2.read((char*)buffer2,buffer2_fill_size*sizeof (block_t)); // read first block from file
    ++(*nios);
    firstBlock = &buffer1[buffer1_iterator];
    ++buffer1_iterator;
    firstRecord = firstBlock->entries[firstBlock->dummy++]; // get first record from file
    secondBlock = &buffer2[buffer2_iterator];
    ++buffer2_iterator;
    secondRecord =secondBlock->entries[secondBlock->dummy++]; // get first record from file

    while ((*firstIterator) < first_block_count && (*secondIterator) < second_block_count) { // while any of two files reached eof
        int compare = compareRecords(firstRecord,secondRecord, field); // compare records
        if (compare == -1) {// first record value is smaller than second record value
            int result = retrieveRecord(buffer1,firstBlock,&firstRecord, input1, nios, firstIterator,&buffer1_iterator,buffer1_fill_size,first_block_count);
            if (result == 0) { // file eof
                break;
            }
        } else if (compare == 1) { // first record value is bigger than second record value
            int result = retrieveRecord(buffer2,secondBlock,&secondRecord, input2, nios, secondIterator,&buffer2_iterator,buffer2_fill_size,second_block_count);

            if (result == 0) {// file eof
                break;
            }
        } else { // first record value is equal with  second record value


            pairRecord = firstRecord; // initialize pairRecord for comparison
            serialize_record(output, outputBlock,firstRecord, nios); //write first record to file
            serialize_record(output, outputBlock,secondRecord, nios); // write second record to file
            ++(*nres);
            do { //while loop to examine for same values
                int result = retrieveRecord(buffer2,secondBlock,&secondRecord, input2, nios, secondIterator,&buffer2_iterator,buffer2_fill_size,second_block_count); // keep record from first file and retrieve from second file
                if (result == 0) {// file eof
                    break;
                }
                if (compareRecords(firstRecord,secondRecord, field) != 0) { // different value from that was written to file
                    do { // search to first file for same values
                        int result = retrieveRecord(buffer1,firstBlock,&firstRecord,input1,nios,firstIterator,&buffer1_iterator,buffer1_fill_size,first_block_count);//read next record from first file
                        if(result == 0) { // file eof
                            break;
                        } else {
                            if(compareRecords(pairRecord,firstRecord,field) == 0) { // same value . add it to output
                                serialize_record(output, outputBlock,firstRecord, nios);
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
    if (outputBlock->nreserved != 0) { // last block that is not filled with Maximum Records . Write it
        outputBlock->valid = true;
        output.write((char*)outputBlock, sizeof (block_t));
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
                   unsigned int *nios, unsigned int *iterator,uint *buffer_iterator,uint buffer_size,uint block_count) {

    if (block->dummy < block->nreserved) {
        *record =block->entries[block->dummy++]; // go to next record

    } else { // change block from file
        if (*iterator >= block_count) {
            return 0; // if end of file , return zero;
        }
        if(*buffer_iterator < buffer_size) { // read block from buffer
            block =&file_buffer[(*buffer_iterator)];
            ++(*buffer_iterator);
            *record = block->entries[block->dummy++];
            ++(*iterator);
        } else { // all buffer blocks iterated . read new buffer blocks from file
            input.read((char*)file_buffer,(buffer_size)*sizeof (block_t)); // read new buffer blocks
            *buffer_iterator = 0; // reset buffer iterator to zero
            block =&file_buffer[(*buffer_iterator)];
            ++(*buffer_iterator);
            *record =block->entries[block->dummy++];
            ++(*nios);
            ++(*iterator);
        }
    }
    return 1;
}



void merge_join(char *infile1, char *infile2, unsigned char field,
                block_t *buffer, uint nmem_blocks, char *outfile, uint *nres, uint *nios) {

    size_t  file1_size = file_block_count(infile1),
            file2_size = file_block_count(infile2);


    if ( (file1_size > nmem_blocks || file2_size > nmem_blocks)
            && nmem_blocks < 3) {
        cerr << "Merge join: External merge requires more"
             << " than 3 blocks of memory, canceling.";
        return;
    }

    // Temporary output names for the sorted files.
    char *sorted_file1 = get_temp_name(),
          *sorted_file2 = get_temp_name();

    uint dummy = 0;

    // Sort files unless they are already sorted
    // ToDo: If a file fits into the buffer, sort on the fly.
    if (!is_sorted(infile1, field))
        merge_sort(infile1, field, buffer, nmem_blocks,
                   sorted_file1, &dummy, &dummy, nios);
    if (!is_sorted(infile2, field))
        merge_sort(infile2, field, buffer, nmem_blocks,
                   sorted_file2, &dummy, &dummy, nios);

    /**************************************************
        Partitioning the buffer as follows:
                buffer for file 1:  min([0, mid), [0, file_size) )
                buffer for file 2:  min([mid, end), [mid, mid + file_size) )
                output buffer    :  [end]
    ***************************************************/
    size_t halve = nmem_blocks / 2;
    size_t mem_blocks1 = file1_size < halve ? file1_size : halve,
           mem_blocks2 = file2_size < halve ? file2_size : halve;
    // If even, reserve a block for output
    if ( (nmem_blocks % 2) == 0) {
        if (mem_blocks2 - 1) --mem_blocks2;
        else if (mem_blocks1 - 1) --mem_blocks1;
    }

    block_t *buffer1    = buffer,
             *buffer2    = buffer + mem_blocks1,
              *out_block  = buffer + mem_blocks1 + mem_blocks2;

    out_block->dummy = 0;
    out_block->nreserved = 0;
    out_block->valid = false;

    ifstream sorted1(sorted_file1, ios::in | ios::binary),
             sorted2(sorted_file2, ios::in | ios::binary);
    ofstream output(outfile, ios::out | ios::binary);

    // Iterators
    uint b1 = 0, r1 = 0, b2 = 0, r2 = 0;

    // Read the first segments
    uint read_len1 = mem_blocks1 < file1_size - b1 ? mem_blocks1 : file1_size - b1,
         read_len2 = mem_blocks2 < file2_size - b2 ? mem_blocks2 : file2_size - b2;

    sorted1.read((char*)buffer1, read_len1 * sizeof(block_t));
    sorted2.read((char*)buffer2, read_len2 * sizeof(block_t));

    // Used to keep the records that get flushed from the buffer during matching
    vector<record_t> overflow_buffer1;
    vector<record_t> overflow_buffer2;

    // Swaps performed so far for each file.
    uint swaps1 = 0, swaps2 = 0;

    record_t *record1 = &buffer1[b1].entries[r1],
              *record2 = &buffer2[b2].entries[r2];


    // While not at the end of either file
    while (true) {

        record1 = &buffer1[b1].entries[r1],
        record2 = &buffer2[b2].entries[r2];

        int r = compareRecords(*record1, *record2, field);
        // If there is a match
        if (!r) {
            /**************************************************************
                Assuming there are 3 matching records on each side:
                So we want to match:
                    A1 : B1, B2, B3
                    A2 : B1, B2, B3
                    A3 : B1, B2, B3

                    1 2 3       First match A1 with B1, B2, B3
                A   * * *       Then  match B1 with A2, A3
                B   * * *       Advance both iterators
                    2 3         Match   A2 with B2, B3
                A   * *         Then    B2 with A3
                B   * *         Advance

                Finally only A3 and B3 have to be matched and the next
                advancement takes care of that.
                This rule applies to any length of records.

                The implementation is as follows:
                    For both sides:
                        While there is a match and not EOF
                            fill overflow_buffer   (lack of a better name)
                            advance

                    for each record a in overflow_buffer1:
                        for each record b in overflow_buffer2:
                            serialize a, b

            ***************************************************************/
            //cerr << (*record1).num << ", " << (*record2).num << "\tin\n";

            record_t vanilla = *record1;

            // Find all the consecutive matching records in file 1
            while (!compareRecords(vanilla, *record1, field)
                    && !is_last_record(buffer1, b1, r1, file1_size,
                                       mem_blocks1, swaps1)) {

                //cerr << vanilla.num << ", " << (*record1).num << "\tw1\n";

                overflow_buffer1.push_back(*record1);

                bool s = advance(sorted1, file1_size, mem_blocks1,
                                 buffer1, b1, r1, nios);
                if (s) ++swaps1;

                record1 = &buffer1[b1].entries[r1];
            }

            // Find all the consecutive matching records in file 2
            while (!compareRecords(vanilla, *record2, field)
                    && !is_last_record(buffer2, b2, r2, file2_size,
                                       mem_blocks2, swaps2)) {

                //cerr << vanilla.num << ", " << (*record2).num << "\tw2\n";

                overflow_buffer2.push_back(*record2);

                bool s = advance(sorted2, file2_size, mem_blocks2,
                                 buffer2, b2, r2, nios);
                if(s) ++swaps2;

                record2 = &buffer2[b2].entries[r2];
            }

            for(auto e1 : overflow_buffer1) {
                for (auto e2 : overflow_buffer2) {
                    serialize_record(output, out_block, e1, nios);
                    serialize_record(output, out_block, e2, nios);
                    ++(*nres);
                }
            }

            overflow_buffer1.clear();
            overflow_buffer2.clear();

            if (is_last_record(buffer1, b1, r1, file1_size, mem_blocks1, swaps1)  ||
                    is_last_record(buffer2, b2, r2, file2_size, mem_blocks2, swaps2)) break;

            // Advance the lesser side
        } else if (r < 0) {
            if (!is_last_record(buffer1, b1, r1, file1_size, mem_blocks1, swaps1)) {
                bool s = advance(sorted1, file1_size, mem_blocks1,
                                 buffer1, b1, r1, nios);
                if (s) ++swaps1;
            } else break;
        } else {
            if (!is_last_record(buffer2, b2, r2, file2_size, mem_blocks2, swaps2)) {
                bool s = advance(sorted2, file2_size, mem_blocks2,
                                 buffer2, b2, r2, nios);
                if (s) ++swaps2;
            } else break;
        }
    }

    /*******************************************************************************/

    // If last element of 1 side matches anything from the other..

    // Last record for file2, while file1 not EOF and
    // record1 lesser or equal to record2, search for matches
    if (is_last_record(buffer2, b2, r2, file2_size, mem_blocks2, swaps2)) {

        while (compareRecords(*record1,  *record2, field) < 1) {

            if (!compareRecords(*record1,  *record2, field)) {
                serialize_record(output, out_block, *record1, nios);
                serialize_record(output, out_block, *record2, nios);
                ++(*nres);
            }

            if (!is_last_record(buffer1, b1, r1, file1_size, mem_blocks1, swaps1) ) {
                bool s = advance(sorted1, file1_size, mem_blocks1,
                                 buffer1, b1, r1, nios);
                if(s) ++swaps1;
                record1 = &buffer1[b1].entries[r1];
            } else break;
        }
    }

    // Last record for file1, while file1 not EOF and
    // record2 lesser or equal to record1, search for matches
    if (is_last_record(buffer1, b1, r1, file1_size, mem_blocks1, swaps1)) {

        while (compareRecords(*record2,  *record1, field) < 1 ) {
            if (!compareRecords(*record1,  *record2, field)) {
                serialize_record(output, out_block, *record1, nios);
                serialize_record(output, out_block, *record2, nios);
                ++(*nres);
            }

            if (!is_last_record(buffer2, b2, r2, file2_size, mem_blocks2, swaps2) ) {
                bool s = advance(sorted2, file2_size, mem_blocks2,
                                 buffer2, b2, r2, nios);
                if(s) ++swaps2;
                record2 = &buffer2[b2].entries[r2];
            } else break;
        }
    }

    // If the output block is not empty, write to file.
    if (out_block->nreserved != 0) {
        out_block->valid = true;
        output.write((char*)out_block, sizeof (block_t));
        ++(*nios);
    }

    // Cleanup
    sorted1.close();
    sorted2.close();
    output.close();

    delete [] sorted_file1;
    delete [] sorted_file2;
}


bool is_last_record(const block_t *buffer, uint b, uint r,
                    uint total, uint mem, uint swaps) {
    // If last segment and id's match
    return (swaps * mem >= total || mem == total)
           && buffer[b].entries[r].recid ==
           buffer[mem - 1].entries[
               buffer[mem - 1].nreserved - 1].recid;
}

// The caller has to ensure that there is another block to fetch
// Returns true if the buffer gets swapped
bool advance(ifstream &input, uint total, uint mem,
             const block_t *buffer, uint &b, uint &r, uint *nios) {

    // If past memory limit, reload buffer, reset iterators
    // Otherwise increment iterators
    bool swap = ((b % mem) == mem - 1) && (r == buffer[b].nreserved - 1);
    if (swap) {

        // Read the new segment
        uint read_len = mem > total - b ? total - b : mem;
        input.read((char*)buffer, read_len * sizeof(block_t));
        ++(*nios);
        b = 0;
        r = 0;

    } else if (++r == buffer[b].nreserved) {
        r = 0;
        ++b;
    }


    return swap;
}




