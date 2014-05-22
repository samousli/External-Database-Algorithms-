/* 
 * File:   HelpingMethods.h
 * Author: geohar
 *
 * Created on 22 Μάϊος 2014, 4:12 μμ
 */

#ifndef HELPINGMETHODS_H
#define	HELPINGMETHODS_H
#include "dbtproj.h"
void SortRecords(block_t *buffer,unsigned int nmem_blocks,char *outputPath,unsigned char field);
int FindMinimumValue(block_t *buffer,unsigned int nmem_blocks,int *positionForCheck,unsigned char field);
int compare (const void * a, const void * b);

    

#endif	/* HELPINGMETHODS_H */

