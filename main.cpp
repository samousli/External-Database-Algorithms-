#include "dbtproj.h"
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <stdlib.h>     /* atoi */

#include <string>
#include <iostream>
#include <fstream>
#include <math.h>

using namespace std;

int main(int argc, char** argv) {

	int nblocks =11;	// number of blocks in the file
	record_t record;	
	block_t block;
	unsigned int recid = 0;
        ifstream infile;
        ofstream outfile;
        int nblocks_buffer = 5;
        unsigned char field = '1';
        
	//Test code with temporary Creation of binary file with nblocks at file
        // -----------------------------------------------------------------
        
	// generate a file 
       outfile.open("file.bin",ios::out | ios::binary); // open input file 
	for (int b=0; b<nblocks; ++b) { // for each block
		
		block.blockid = b;
		for (int r=0; r<MAX_RECORDS_PER_BLOCK; ++r) { // for each record
			
			// prepare a record
			record.recid = recid++;
			record.num = rand() % 1000;
			strcpy(record.str,"hello");   // put the same string to all records
			record.valid = true;
			
			memcpy(&block.entries[r], &record, sizeof(record_t)); // copy record to block
		}
		
		block.nreserved = MAX_RECORDS_PER_BLOCK;
		block.valid = true;
		
		outfile.write((char*)&block,sizeof(block_t));	// write the block to the file
	}
	
	
	// -----------------------  END OF TEST  --------------------------
        
	// open file and print contents
	infile.open("file.bin", ios::in | ios::binary);
        
        
        block_t blocksForSorting[nblocks_buffer]; // empty buffer with nblocks_buffer blocks 
        unsigned int temporaryForTest = 0;   
       char path[] = "file.bin"; // input file 
       char  resultsFile[] = "results.bin";
        MergeSort(path,field,blocksForSorting,nblocks_buffer,resultsFile,&temporaryForTest,&temporaryForTest,&temporaryForTest);
        
	
	
	infile.close();
        infile.open("output3.bin",ios::in | ios::binary);
        while(!infile.eof()){
          infile.read((char*)&record,sizeof(record_t)); // read block from file 

         
			printf("this record id: %d, num: %d, str: %s\n", 
					 record.recid, record.num, record.str);
		
        }
        infile.close();
	return 0;
}

