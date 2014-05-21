#include "dbtproj.h"
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <string>
#include <iostream>
#include <math.h>

using namespace std;

int main(int argc, char** argv) {

	int nblocks =10000;	// number of blocks in the file
	record_t record;	
	block_t block;
	unsigned int recid = 0;
	FILE *infile, *outfile;
        int nblocks_buffer = 3;
        
	//Test code with temporary Creation of binary file with nblocks at file
        // -----------------------------------------------------------------
        
	// generate a file 
	outfile = fopen("file.bin", "w");
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
		
		fwrite(&block, 1, sizeof(block_t), outfile);	// write the block to the file
	}
	
	fclose(outfile);
	// -----------------------  END OF TEST  --------------------------
        
	// open file and print contents
	infile = fopen("file.bin", "r");
        // set the file pointer to end of file
        fseek(infile, 0, SEEK_END );
        // get the file size
        int Size = ftell(infile);
        // return the file pointer to begin of file if you want to read it
        rewind(infile);
        
        int numberOfBlocks = Size/sizeof(block_t);
        int size = numberOfBlocks/nblocks_buffer;
        cout<<size<<endl;
        int getBlocks = 0;
        block_t blocksForSorting[2];
	int nreserved;   
        int idOfOutputFile = 1;
	while (!feof(infile)) { // while end-of-file has not been reached ... 
		fread(&block, 1, sizeof(block_t), infile); // read the next block
		nreserved = block.nreserved;
                
                blocksForSorting[getBlocks] = block; // add block to block array for sorting
                getBlocks++; // block added 
                
		// print block contents
		for (int i=0; i<nreserved; ++i) {
			//printf("this is block id: %d, record id: %d, num: %d, str: %s\n", 
			//		block.blockid, block.entries[i].recid, block.entries[i].num, block.entries[i].str);
		}
                if(getBlocks == 2){ // Stored two blocks . Go and sort them
                    getBlocks = 0; // reset blocks counter variable
                    unsigned int temporaryForTest = 0;
                    char path[] = "file.bin"; // input file 
                    std::string outputPathString("output"); // string with output file path
                    outputPathString.append(std::to_string(idOfOutputFile)); // append number of output file
                    idOfOutputFile++; // next output file id 
                    outputPathString.append(".bin"); // append extension of output file 
                    char *outputPath = (char*)outputPathString.c_str(); // convert string to char array
                    cout<<outputPath<<endl;
                    MergeSort(path,'1',blocksForSorting,nblocks_buffer,outputPath,&temporaryForTest,&temporaryForTest,&temporaryForTest);
                }
	}
	
	fclose(infile);
	return 0;
}

