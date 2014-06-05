#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <cstddef>
#include <queue>

#include "dbtproj.h"
#include "ComparisonPredicates.h"
#include "MergeSortImpl.h"
#include "Tests.h"

using namespace std;

/* ----------------------------------------------------------------------------------------------------------------------
   infile: the name of the input file
   field: which field will be used for sorting: 0 is for recid, 1 is for num, 2 is for str and 3 is for both num and str
   buffer: pointer to memory buffer
   nmem_blocks: number of blocks in memory
   outfile: the name of the output file
   nsorted_segs: number of sorted segments produced (this should be set by you)
   npasses: number of passes required for sorting (this should be set by you)
   nios: number of IOs performed (this should be set by you)
   ----------------------------------------------------------------------------------------------------------------------
 */
void MergeSort(char *infile, unsigned char field, block_t *buffer, unsigned int nmem_blocks,
               char *outfile, unsigned int *nsorted_segs, unsigned int *npasses, unsigned int *nios) {
   

}

/* ----------------------------------------------------------------------------------------------------------------------
   infile: the name of the input file
   field: which field will be used for sorting: 0 is for recid, 1 is for num, 2 is for str and 3 is for both num and str
   buffer: pointer to memory buffer
   nmem_blocks: number of blocks in memory
   outfile: the name of the output file
   nunique: number of unique records in file (this should be set by you)
   nios: number of IOs performed (this should be set by you)
   ----------------------------------------------------------------------------------------------------------------------
 */
void EliminateDuplicates(char *infile, unsigned char field, block_t *buffer,
                         unsigned int nmem_blocks, char *outfile, unsigned int *nunique, unsigned int *nios) {
    uint *sorted_segs = new uint(0),
    *passes = new uint(0);
    nunique = new uint(0);
    char *sortedfile = "sorted.bin";
    ifstream input(sortedfile,ios::in | ios::binary); // temporary sorted file 
    ofstream output(outfile,ios::out | ios::binary); // output file 
    if(!input.good()){ // file not exists . Sort file and then elinimate duplicates 
      merge_sort_driver(1 << 14, 1 << 5,infile,sortedfile);
    }
    input.close();
    uint block_count = file_block_count(sortedfile);
    input.open(sortedfile,ios::in | ios::binary);
    block_t currentBlock; // current retrievedBlock
    currentBlock.blockid = 0; // initialize block id to zero 
    currentBlock.valid = false; // initialize valid to false 
    currentBlock.nreserved = 0; // initialize nreserved to zero 
    record_t currentRecord; // stores current Record for examination 
    input.read((char*) buffer, nmem_blocks * sizeof (block_t));
     ++(*nios);
     currentRecord = buffer[0].entries[buffer[0].dummy++];
    int startIteration = 1 ; //start iteration from second record 
    serialize_record(output,currentBlock,currentRecord,nios); 
    ++(*nunique); // found unique record 
    uint read_count = nmem_blocks;
    for (uint i = nmem_blocks; i <= block_count; i += read_count) { // read blocks while reached buffer size
       int result =  testRecordsForDuplicates(startIteration,buffer,read_count,currentRecord,currentBlock,output,nunique,nios,field);
       if(result == 0){
           return; // problem 
       }
        if(i < block_count){ // read next blocks to buffer 
        // Read into buffer
        read_count = nmem_blocks < block_count - i ? nmem_blocks : block_count - i;
        input.read((char*) buffer, read_count * sizeof (block_t));
        ++(*nios);
        }
    }
    if(currentBlock.nreserved != 0 ){ // last block that is not filled with Maximum Records . Write it
        currentBlock.valid = true;
        output.write((char*) &currentBlock, sizeof (block_t));
        ++(*nios);
    }
    input.close();
    output.close();
    cout << "Is sorted? " << is_sorted(outfile,1) << endl;
    cout << "Sorted segments: " << *sorted_segs << endl
         << "IO's: " << *nios << endl
         << "Unique Records: " << *nunique << endl;
    uint block_count_output = file_block_count(outfile);
    bool duplicates = test_duplicates (outfile,block_count_output,field);
    cout<<"Elinimated Duplicates ?? "<<duplicates<<endl;

    
    
}

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
void MergeJoin(char *infile1, char *infile2, unsigned char field, block_t *buffer,
               unsigned int nmem_blocks, char *outfile, unsigned int *nres, unsigned int *nios) {
    
    // Merge Sort Both!
    // Iterate linearly and write matching pairs to the output
    uint *firstIterator = new uint(0); // iterator for first sorted file 
    uint *secondIterator = new uint(0); // iterator for second sorted file 
    block_t firstBlock; // retrieved block from first sorted file 
    block_t secondBlock; // retrieved block from second sorted file 
    record_t firstRecord ; // retrieved record from first sorted file 
    record_t secondRecord; // retrieved record from second sorted file 
    block_t  outputBlock ;  // block to write results to output file 
    outputBlock.blockid = 0; // initialize block id to zero 
    outputBlock.valid = false; // initialize valid to false 
    outputBlock.nreserved = 0; // initialize nreserved to zero 
    
    //Sort files 
    merge_sort_driver(1 << 14, 1 << 5,infile1,"output1.bin");
    merge_sort_driver(1 << 14 , 1 << 5,infile2,"output2.bin");
    // -----------
    
    uint first_block_count = file_block_count("output1.bin");
    uint second_block_count = file_block_count("output2.bin");
    
    cout<<"Sorted 1 : "<<first_block_count<<endl;
    cout<<"Sorted 2 : "<<second_block_count<<endl;
    // ------------ 
    
   // print_file_contents("output1.bin");
   // print_file_contents("output2.bin");
    
    ifstream input1("output1.bin",ios::in | ios::binary);
    ifstream input2("output2.bin",ios::in | ios::binary);
    ofstream output(outfile,ios::out | ios::binary);
    
    input1.read((char*)&firstBlock,sizeof(block_t)); // read first block from file 
    ++(*nios);
    input2.read((char*)&secondBlock,sizeof(block_t)); // read first block from file 
    ++(*nios);
    firstRecord = firstBlock.entries[firstBlock.dummy++]; // get first record from file 
    secondRecord = secondBlock.entries[secondBlock.dummy++]; // get first record from file 
    
    while((*firstIterator) < first_block_count && (*secondIterator) < second_block_count){
        int compare = compareField(firstRecord,secondRecord,field); // compare records 
        if(compare == -1){// first record value is smaller than second record value  
           int result =  retrieveRecord(&firstBlock,&firstRecord,input1,nios,firstIterator);
           if(result == 0 ){ // file eof 
               break;
           }
        } 
        else if(compare == 1){ // first record value is bigger than second record value  
            int result =  retrieveRecord(&secondBlock,&secondRecord,input2,nios,secondIterator);
            if(result == 0 ){// file eof
               break;
            }
        }
        else { // first record value is equal with  second record value 
            serialize_record(output,outputBlock,firstRecord,nios); //write first record to file 
            ++(*nios);
            serialize_record(output,outputBlock,secondRecord,nios); // write second record to file 
            ++(*nios);
            ++(*nres);
            record_t compareRecord = firstRecord; // temporary record to examine if there are other same values to write 
            do{ //while loop to examine for same values 
              int result =  retrieveRecord(&firstBlock,&firstRecord,input1,nios,firstIterator);
              if(result == 0 ){// file eof
                break;
              }
              if(compareField(compareRecord,firstRecord,field) != 0){ // different value from that was written to file 
                  break;
              }
              else{ // founded same value with that was written to file
                  serialize_record(output,outputBlock,firstRecord,nios);
                  ++(*nios);
                  ++(*nres);
              }
              
              int result1 =  retrieveRecord(&secondBlock,&secondRecord,input2,nios,secondIterator);
              if(result1 == 0 ){// file eof
                break;
              }
              if(compareField(compareRecord,secondRecord,field) != 0){ // different value from that was writen to file
                  break;
              }
              else{ // founded same value with that was written to file
                  serialize_record(output,outputBlock,secondRecord,nios);
                  ++(*nios);
                  ++(*nres);
              }
            }while(true);
            
        }
        
   }
   if(outputBlock.nreserved != 0 ){ // last block that is not filled with Maximum Records . Write it
        outputBlock.valid = true;
        output.write((char*) &outputBlock, sizeof (block_t));
        ++(*nios);
   }
    cout<<"Merge-Join Output File : "<<endl;
   cout << "Is sorted? " << is_sorted(outfile,1) << endl;
   cout <<"IO's: " << *nios << endl
        << "Pairs : " << *nres << endl;

   input1.close(); // close input stream 
   input2.close(); // close input stream 
   output.close(); // close output stream 
   

    
}

