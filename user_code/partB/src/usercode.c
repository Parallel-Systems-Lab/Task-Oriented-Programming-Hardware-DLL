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
#define COLORS 256
/** Globals **/
unsigned char rlut[COLORS] __attribute__ ((section ("shared_dram"))) = { 0 };
unsigned char glut[COLORS] __attribute__ ((section ("shared_dram"))) = { 0 };
unsigned char blut[COLORS] __attribute__ ((section ("shared_dram"))) = { 0 };



int dup=16;
/*************/

void dummy1 (void) {}

void init(void) 
{
  SetCoreNum(16);
  SetQuota(balance,dup);
  SetQuota(buildlut,dup);
}
void buildlut (void) 
{
  int i=TaskInst();
  int temp;
  for(; i<COLORS; i+=dup){
    
    temp = i*RFACTOR;
    if(temp>255){rlut[i]=255;}
    else{rlut[i]=temp;}
    
    temp = i*GFACTOR;
    if(temp>255){glut[i]=255;}
    else{glut[i]=temp;}
    
    temp = i*BFACTOR;
    if(temp>255){blut[i]=255;}
    else{blut[i]=temp;}
  }
}

void balance(void) 
{
  int i,j;
  i=TaskInst();
  	for(;i<DIM;i+=dup){
		  for(j=0;j<DIM;j++){
        rresult[i][j]=rlut[lennaR[i][j]];
        gresult[i][j]=glut[lennaG[i][j]];
        bresult[i][j]=blut[lennaB[i][j]];
		  }
     }
}

void finish(void) {


}
