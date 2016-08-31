#include "dec.h"
#include "../../../tools/scheduler/src/e_main_sched.h"
#define SECTION(x)  __attribute__ ((section (x)))
int c_n SECTION("shared_dram") = 1;
int dt=1;
void init()
{
   SetCoreNum(c_n);
   SetQuota(batch1,3);
   SetQuota(batch2,3);
   int i=0;
   while(i<dt){i++;}
}
void batch1()
{  
int i=0;
   while(i<dt){i++;}

}
void step1()
{
SetQuota(batch3,5);
SetQuota(batch4,10);
SetQuota(batch5,10);
SetQuota(batch6,10);
  int i=0;
   while(i<dt){i++;}
}
void batch2(){  int i=0;
   while(i<dt){i++;}}
void step2(){  int i=0;
   while(i<dt){i++;}}
void batch3(){  int i=0;
   while(i<dt){i++;}}
void batch4(){  int i=0;
   while(i<dt){i++;}}
void batch5(){  int i=0;
   while(i<dt){i++;}}
void batch6(){  int i=0;
   while(i<dt){i++;}}

void step3()
{
c_n=c_n+1;
if (c_n>16) {RetValue(0);}
else {
SetCoreNum(c_n);
RetValue(1);
}
}

void finish(){  int i=0;
   while(i<dt){i++;}}
