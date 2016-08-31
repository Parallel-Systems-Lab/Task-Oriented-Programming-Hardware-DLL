#include "dec.h"
#include "../../../tools/scheduler/src/e_main_sched.h"
#define SECTION(x)  __attribute__ ((section (x)))
#define N 16
int Aarray[N] SECTION("shared_dram") = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16 };
int Barray[N] SECTION("shared_dram")= {11,12,13,14,15,16,17,18,19,110,111,112,113,114,115,116} ;
int Carray[N] SECTION("shared_dram");
int res SECTION("shared_dram")= 0;
int n SECTION("shared_dram")= N;

void init2()
{
SetQuota(add2,N);
}



void add2()
{
   int i=TaskInst();
Carray[i]=Aarray[i]+Barray[i];
}

void sumation2()
{
int i;
 for (i=0;i<N;i++)
res+=Carray[i];
}


void finish2()
{

}
