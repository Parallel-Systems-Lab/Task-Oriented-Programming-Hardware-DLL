#include "dec.h"
#include "../../../tools/scheduler/src/e_main_sched.h"
int res=0;
void dupf1(){}
void dupf2(){}
void f3(){}
void dupf4(){}
void dupf5(){}
void f0()
{
int i=0;
SetQuota(dupf1,15);
SetQuota(dupf2,10);
SetQuota(dupf3,20);
SetQuota(dupf4,30);
while(i<10) {i++;}
res+=i;
} 
void f1()  
{
int i=0;

while(i<100){i++;}
RetValue(1);
res+=i;
}
 void f2()
{
int i=0;
while(i<10){i++;}
res+=i;

}
 void dupf3()
{
int i=0;
while(i<100){i++;}
if(TaskInst()>=3)
    while(i<10000){i++;}
    res+=i;

}
 void f4()
{
int i=0;
while(i<1){i++;}
res+=i;

}
 void f5()
{
int i=0;
while(i<10){i++;}
res+=i;

}
 void bar()
{
int i=0;
while(i<100){i++;}
res+=i;

}

 void starter()
{
int i=0;
while(i<1000){i++;}
res+=i;
}