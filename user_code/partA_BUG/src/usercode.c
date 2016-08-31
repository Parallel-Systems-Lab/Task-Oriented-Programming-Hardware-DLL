#include "dec.h"
#include "../../../tools/scheduler/src/e_main_sched.h"
#define SECTION(x)  __attribute__ ((section (x)))
#define N 16
int Aarray[N] SECTION("shared_dram") = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16 };
int Barray[N] SECTION("shared_dram")= {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16 } ;
int Carray[N] SECTION("shared_dram");
int res SECTION("shared_dram")= 0;
int n SECTION("shared_dram")= N;
void init1()
{
  SetCoreNum(16);
  SetQuota(evaluate1,N);
}

void evaluate1()
{
  int i=TaskInst();
  Carray[i]=Aarray[i]+Barray[i];
}

/*void evaluate2()
{
  int i;
  for(i=0; i<N; i++){
    res+=Carray[i];
  }
}*/

void config_par(void) {
int i;
int local_n=n/2;
n=local_n;
	if (n==1) {
		res = Carray[0];
		RetValue(1);
	} else {
		
    SetQuota(par_res,n);
    while(i<1000) i++;
		RetValue(0);
	}

}

void par_res (void) {
	int i=TaskInst();
	Carray[i] = Carray[i] + Carray[i+n];
}

void finish1()
{

}
