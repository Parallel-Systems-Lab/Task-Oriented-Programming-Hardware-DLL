#include "dec.h"
#include "../../../tools/scheduler/src/e_main_sched.h"
#define SECTION(x)  __attribute__ ((section (x)))
#define N 16
int Aarray[N] SECTION("shared_dram") = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16 };
int Barray[N] SECTION("shared_dram")= {11,12,13,14,15,16,17,18,19,110,111,112,113,114,115,116} ;
int Carray[N] SECTION("shared_dram");
int res SECTION("shared_dram")= 0;
int n SECTION("shared_dram")= N;

void init3()
{
SetQuota(add3,N);
}

void add3()
{
   int i=TaskInst();
Carray[i]=Aarray[i]+Barray[i];
}

void while_cond3()
{int local_n=n/2;
int i=0;
n=local_n;
   if(local_n>0)
   {
   SetQuota(pair_add3,local_n);
   while(i<100) i++;
   RetValue(1);
   }
   else
   {
   RetValue(0);
   }
}
void pair_add3()
{
 int i=TaskInst();
Carray[i]=Carray[i]+Carray[i+n];
}

void finish3()
{

}
