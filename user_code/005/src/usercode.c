#include "dec.h"
#include "../../../tools/scheduler/src/e_main_sched.h"


void init()
{
SetCoreNum(16);
SetQuota(dup_A,25);
SetQuota(dup_B,25);
//RetValue(0);
int i=0;

   while(i<1000) i++;
}

void dup_A()
{
int i=0;
   while(i<10) i++;
}
void dup_B()
{
int i=0;
   while(i<100) i++;
}


void finish()
{
int i=0;

   while(i<100) i++;
}

