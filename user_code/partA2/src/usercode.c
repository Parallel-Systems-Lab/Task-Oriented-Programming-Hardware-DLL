#include "dec.h"
#include "../../../tools/scheduler/src/e_main_sched.h"
#define SECTION(x)  __attribute__ ((section (x)))
#define N 16
int Aarray[N] SECTION("shared_dram") = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16 };
int Barray[N] SECTION("shared_dram")= {16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1} ;
int Carray[N] SECTION("shared_dram");
int res SECTION("shared_dram")= 0;
int n SECTION("shared_dram")= N;
void init1()
{
SetCoreNum(16);
}

void evaluate1()
{
   int i;
   for (i=0;i<N;i++)
Carray[i]=Aarray[i]+Barray[i];
   for (i=0;i<N;i++)
res+=Carray[i];
}
void finish1()
{

}
