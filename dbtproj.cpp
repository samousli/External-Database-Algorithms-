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
    ifstream input("sorted.bin",ios::in | ios::binary); // temporary sorted file 
    ofstream output(outfile,ios::out | ios::binary); // output file 
    if(!input.good()){ // file not exists . Sort file and then elinimate duplicates 
      merge_sort_driver(1 << 14, 1 << 5,infile,"sorted.bin");
    }
    input.close();
    input.open("sorted.bin",ios::in | ios::binary);
     // Find file size
    input.seekg(0, input.end);
    uint block_count = input.tellg() / sizeof (block_t); // get number of blocks in file
    input.seekg(0, input.beg);
    block_t currentBlock; // current retrievedBlock
    currentBlock.blockid = 0; // initialize block id to zero 
    currentBlock.valid = false; // initialize valid to false 
    currentBlock.nreserved = 0; // initialize nreserved to zero 
    
    record_t currentRecord; // stores current Record for examination 
    currentRecord = buffer[0].entries[0]; // initialize current Record 
    int startIteration = 1 ; //start iteration from second record 
    serialize_record(output,currentBlock,currentRecord,nios); 
    ++(*nunique); // found unique record 
    for (uint i = 0; i < block_count; i += nmem_blocks) { // read blocks while reached buffer size
        // Read into buffer
        uint read_count = nmem_blocks < block_count - i ? nmem_blocks : block_count - i;
        input.read((char*) buffer, read_count * sizeof (block_t));
        ++(*nios);
        
        for(int y=0;y<nmem_blocks;y++){ // iterate buffer 
            for(int k=startIteration;k<MAX_RECORDS_PER_BLOCK;k++){ // for each block iterate records 
                    record_t nextRecord = buffer[y].entries[k]; // temporary storage of next record 
                    int compare = compareField(currentRecord,nextRecord,field);
                    if(compare == 1 || compare == -1){ // if record 
                        currentRecord = nextRecord;
                      //  cout<<currentRecord.num<<endl;
                        serialize_record(output,currentBlock,currentRecord,nios);
                        ++(*nunique); // found unique record 
                    }
                
            }
            startIteration = 0; // start from first record 
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
    input.open(outfile,ios::in | ios::binary);
     // Find file size
    input.seekg(0, input.end);
    uint block_count1 = input.tellg() / sizeof (block_t); // get number of blocks in file
    input.seekg(0, input.beg);
    input.close();
    bool duplicates = test_duplicates (outfile,block_count1,field);
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
    int firstIterator  = 0; // iterator for first sorted file 
    int secondIterator = 0; // iterator for second sorted file 
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
    
    //Count numbers of blocks from each file 
    ifstream countBlocks_output1("output1.bin",ios::in | ios::binary);
    ifstream countBlocks_output2("output2.bin",ios::in | ios::binary);
     // Find  first file size
    countBlocks_output1.seekg(0, countBlocks_output1.end);
    uint first_block_count = countBlocks_output1.tellg() / sizeof (block_t); // get number of blocks in file
    countBlocks_output1.seekg(0, countBlocks_output1.beg);
    countBlocks_output1.close();
     // Find  second file size
    countBlocks_output2.seekg(0, countBlocks_output2.end);
    uint second_block_count = countBlocks_output2.tellg() / sizeof (block_t); // get number of blocks in file
    countBlocks_output2.seekg(0, countBlocks_output2.beg);
    countBlocks_output2.close();
    cout<<"Sorted 1 : "<<first_block_count<<endl;
    cout<<"Sorted 2 : "<<second_block_count<<endl;
    // ------------ 
    
   // print_file_contents("output1.bin");
   // print_file_contents("output2.bin");
    
    ifstream input1("output1.bin",ios::in | ios::binary);
    ifstream input2("output2.bin",ios::in | ios::binary);
    ofstream output(outfile,ios::out | ios::binary);
    
    input1.read((char*)&firstBlock,sizeof(block_t)); // read first block from file 
    input2.read((char*)&secondBlock,sizeof(block_t)); // read first block from file 
    ++(*nios);
    firstRecord = firstBlock.entries[firstBlock.dummy]; // get first record from file 
    secondRecord = secondBlock.entries[secondBlock.dummy]; // get first record from file 
    
    while(firstIterator < first_block_count && secondIterator < second_block_count){
        int compare = compareField(firstRecord,secondRecord,field); // compare records 
        if(compare == -1){// first record value is smaller than second record value  
            if(firstBlock.dummy < firstBlock.nreserved){ 
                firstRecord = firstBlock.entries[++firstBlock.dummy]; // go to next record 
            }
            else { // change block from file 
                if(input1.eof()){
                    break; // if end of file , break;
                }
                input1.read((char*)&firstBlock,sizeof(block_t));
                firstRecord = firstBlock.entries[firstBlock.dummy];
                ++(*nios);
            }
        } 
        else if(compare == 1){ // first record value is bigger than second record value  
            if(secondBlock.dummy < secondBlock.nreserved){
                secondRecord = secondBlock.entries[++secondBlock.dummy]; // go to next record 
            }
            else { // change block from file 
                if(input2.eof()){
                    break;// if end of file , break;
                }
                input2.read((char*)&secondBlock,sizeof(block_t));
                secondRecord = secondBlock.entries[secondBlock.dummy];
                ++(*nios);
            }
        }
        else { // first record value is equal with  second record value 
            serialize_record(output,outputBlock,firstRecord,nios);
            serialize_record(output,outputBlock,secondRecord,nios);
            
            if(firstBlock.dummy < firstBlock.nreserved){
                firstRecord = firstBlock.entries[++firstBlock.dummy]; // go to next record 
            }
            else { // change block from file 
                 if(input1.eof()){
                    break;// if end of file , break;
                }
                input1.read((char*)&firstBlock,sizeof(block_t));
                firstIterator++;
                firstRecord = firstBlock.entries[firstBlock.dummy];
                ++(*nios);
            }
            if(secondBlock.dummy < secondBlock.nreserved){
                secondRecord = secondBlock.entries[++secondBlock.dummy]; // go to next record 
            }
            else { // change block from file 
                 if(input2.eof()){
                    break;// if end of file , break;
                }
                input2.read((char*)&secondBlock,sizeof(block_t));
                secondIterator++;
                secondRecord = secondBlock.entries[secondBlock.dummy];
                ++(*nios);
            }
        }
        
   }
        input1.close(); // close input stream 
        input2.close(); // close input stream 
        output.close(); // close output stream 


    
}

