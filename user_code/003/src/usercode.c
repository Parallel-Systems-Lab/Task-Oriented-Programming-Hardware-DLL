#include "dec.h"
#include "../../../tools/scheduler/src/e_main_sched.h"


void init()
{
SetCoreNum(16);
SetQuota(dup_20,25);
//RetValue(0);
int i=0;

   while(i<1000) i++;
}

void dup_20()
{
int i=0;
int j=TaskInst();
if (j>6) {j=1000;}
if (j<6) {j=1;}
if (j==0) {j=100000;}
   while(i<j) i++;
}



void SAS_all_2()
{
int i=0;

   while(i<100) i++;
 
}
void SAF_all_7()
{
int i=0;

   while(i<100) i++;
 
}
void finish()
{
int i=0;

   while(i<100) i++;
}

