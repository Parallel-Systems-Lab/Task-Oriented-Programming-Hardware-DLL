#include "dec.h"
#include "../../../tools/scheduler/src/e_main_sched.h"
/* source code for white balance */
#include "lennaR.h"  
#include "lennaG.h" 
#include "lennaB.h" 


#define DIM 128

#define RFACTOR 1.017f
#define GFACTOR 1
#define BFACTOR 1.347f
/*************/

/** Globals **/



unsigned char rresult[DIM][DIM] __attribute__ ((section ("shared_dram"))) = { 0 };
unsigned char gresult[DIM][DIM] __attribute__ ((section ("shared_dram"))) = { 0 };
unsigned char bresult[DIM][DIM] __attribute__ ((section ("shared_dram"))) = { 0 };
int dup=16;
/*************/

void dummy1 (void) {}

void init(void) 
{

}
void rbalance(void) 
{

int i,j;
for (i=0;i<DIM;i++)
for (j=0;j<DIM;j++)
{
rresult[i][j]=(lennaR[i][j]*(float)RFACTOR>255?255:lennaR[i][j]*(float)RFACTOR);
}

}
void gbalance(void) {

int i,j;
for (i=0;i<DIM;i++)
for (j=0;j<DIM;j++)
{
gresult[i][j]=(lennaG[i][j]*(float)GFACTOR>255?255:lennaG[i][j]*(float)GFACTOR);
}

}
void bbalance(void) {

int i,j;
for (i=0;i<DIM;i++)
for (j=0;j<DIM;j++)
{
bresult[i][j]=(lennaB[i][j]*(float)BFACTOR>255?255:lennaB[i][j]*(float)BFACTOR);
}

}

void finish(void) {


}
