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



unsigned char rresult[DIM][DIM] __attribute__ ((section ("shared_dram"))) = { 0 };
unsigned char gresult[DIM][DIM] __attribute__ ((section ("shared_dram"))) = { 0 };
unsigned char bresult[DIM][DIM] __attribute__ ((section ("shared_dram"))) = { 0 };
unsigned char rlut[COLORS] __attribute__ ((section ("shared_dram"))) = { 0 };
unsigned char glut[256] __attribute__ ((section ("shared_dram"))) = { 0 };
unsigned char blut[256] __attribute__ ((section ("shared_dram"))) = { 0 };
int dup=16;
/*************/

void dummy1 (void) {}

void init(void) {


	
  SetQuota(rbalance, DIM);
	SetQuota(gbalance, DIM);
	SetQuota(bbalance, DIM);
 	SetQuota(rbuildlut, COLORS);
  SetQuota(gbuildlut, COLORS);
  SetQuota(bbuildlut, COLORS);

}
void rbuildlut(void)
{
int i=TaskInst();
int tmp=i*RFACTOR;
rlut[i]=(tmp>255?255:tmp);
}
void gbuildlut(void)
{
int i=TaskInst();
int tmp=i*RFACTOR;
rlut[i]=(tmp>255?255:tmp);
}
void bbuildlut(void)
{
int i=TaskInst();
int tmp=i*RFACTOR;
rlut[i]=(tmp>255?255:tmp);
}
void rbalance(void) 
{


int i,j;
j=TaskInst();
for (i=0;i<DIM;i++)
{
rresult[i][j]=rlut[lennaR[i][j]];
}
}
void bbalance(void) 
{


int i,j;
j=TaskInst();
for (i=0;i<DIM;i++)
{
bresult[i][j]=blut[lennaB[i][j]];
}
}
void gbalance(void) 
{


int i,j;
j=TaskInst();
for (i=0;i<DIM;i++)
{
gresult[i][j]=glut[lennaG[i][j]];
}
}


void finish(void) {

}
