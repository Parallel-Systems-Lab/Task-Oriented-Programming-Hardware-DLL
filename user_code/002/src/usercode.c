#include "dec.h"
#include "../../../tools/scheduler/src/e_main_sched.h"


void init()
{
SetCoreNum(16);
SetQuota(dup_50,50);
//RetValue(0);
int i=0;

   while(i<1000) i++;
}

void dup_50()
{
int i=0;

   while(i<100) i++;
}

void SAS_5()
{
int i=0;

   while(i<100) i++;
}

void SAF_10()
{
int i=0;

   while(i<100) i++;
 
}
void SAF_25()
{
int i=0;

   while(i<100) i++;
 
}
void finish()
{
int i=0;

   while(i<100) i++;
}

