#include "dec.h"
#include "../../../tools/scheduler/src/e_main_sched.h"
/* source code for white balance */
#include "lennaR.h"  
#include "lennaG.h" 
#include "lennaB.h" 


#define DIM 128
#define COLORS 256
#define RFACTOR 1.017f
#define GFACTOR 1
#define BFACTOR 1.347f
/*************/

/** Globals **/


unsigned char rlut[COLORS] __attribute__ ((section ("shared_dram"))) = { 0 };
unsigned char glut[256] __attribute__ ((section ("shared_dram"))) = { 0 };
unsigned char blut[256] __attribute__ ((section ("shared_dram"))) = { 0 };
unsigned char rresult[DIM][DIM] __attribute__ ((section ("shared_dram"))) = { 0 };
unsigned char gresult[DIM][DIM] __attribute__ ((section ("shared_dram"))) = { 0 };
unsigned char bresult[DIM][DIM] __attribute__ ((section ("shared_dram"))) = { 0 };
int dup=16;
/*************/

void dummy1 (void) {}

void init(void) {


	
  
	SetQuota(buildlut, dup);
	SetQuota(balance, dup);

}
void buildlut(void)
{
int i;
for (i=TaskInst();i<DIM;i+=dup)
 {
int tmp=i*RFACTOR;
rlut[i]=(tmp>255?255:tmp);
tmp=i*GFACTOR;
glut[i]=(tmp>255?255:tmp);
tmp=i*BFACTOR;
blut[i]=(tmp>255?255:tmp);
}
}
void balance(void) 
{


int i,j;
for (j=TaskInst();j<DIM;j+=dup)
for (i=0;i<DIM;i++)
{
rresult[i][j]=rlut[lennaR[i][j]];
gresult[i][j]=glut[lennaG[i][j]];
bresult[i][j]=blut[lennaB[i][j]];
}
}

void finish(void) {

}
