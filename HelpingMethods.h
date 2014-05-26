/* 
 * File:   HelpingMethods.h
 * Author: geohar
 *
 * Created on 22 Μάϊος 2014, 4:12 μμ
 */

#ifndef HELPINGMETHODS_H
#define	HELPINGMETHODS_H
#include "dbtproj.h"
#include <iostream>
#include <fstream>
typedef unsigned int uint;

void SortRecords(block_t *buffer,unsigned int nmem_blocks,char *outputPath,unsigned char field);
std::pair<int,int> FindMinimumValue(block_t *buffer,unsigned int nmem_blocks,int *positionForCheck,unsigned char field);
int compare (const void * a, const void * b);
int compare1(const void * a, const void * b);
int compare2(const void * a, const void * b);


// Keeping streams open may boost performance but doing this for now to simplify things.

void Write_Block(std::ofstream& outfile, block_t block);
void Serialize_Record(std::ofstream& outfile, block_t &block, record_t record);


    

#endif	/* HELPINGMETHODS_H */

