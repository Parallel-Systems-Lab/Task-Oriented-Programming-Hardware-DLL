#include "dec.h"
#include "../../../tools/scheduler/src/e_main_sched.h"
void init()
{
SetQuota(HW_batch_1,5);
SetQuota(HW_batch_2,5);
SetQuota(HW_batch_3,10);
SetQuota(HW_batch_4,4);
SetQuota(HW_batch_5,5);
SetQuota(HW_batch_6,5);
SetQuota(HW_batch_7,5);
SetQuota(HW_batch_8,5);
SetQuota(HW_batch_9,5);
int i=0;
while(i<10000){i++;}
}
void HW_batch_1()
{
int i=0;
while(i<10000){i++;}
}
void HW_batch_2()
{
int i=0;
while(i<5000){i++;}
}
void HW_batch_3()
{
int i=0;
while(i<20000){i++;}
}
void HW_batch_4()
{
int i=0;
while(i<50000){i++;}
}
void HW_batch_5()
{
int i=0;
while(i<50000){i++;}
}
void HW_batch_6()
{
int i=0;
while(i<50000){i++;}
}
void HW_batch_7()
{
int i=0;
while(i<50000){i++;}
}
void HW_batch_8()
{
int i=0;
while(i<50000){i++;}
}
void HW_batch_9()
{
int i=0;
while(i<50000){i++;}
}
void finish()
{
int i=0;
while(i<10000){i++;}
}

/*
int res=0;

void finish(){RetValue(0);}
void init()
{
int i=0;
SetCoreNum(2);
SetQuota(macintosh,15);
SetQuota(dupli,10);
SetQuota(starter,20);
SetQuota(blabla,200);
RetValue(1);
while(i<10) {i++;}
res+=i;
return;
} 
void foo()  
{
int i=0;

while(i<100){i++;}
RetValue(0);//RetFalse;
res+=i;
return;
}
 void dupli()
{
int i=0;
while(i<10){i++;}
res+=i;
return;
}
 void macintosh()
{
int i=0;
while(i<100){i++;}
if(TaskInst()>=3)
    while(i<10000000){i++;}
    res+=i;
return;
}
 void blabla()
{
int i=0;
while(i<1000){i++;}
res+=i;

return;
}
 void foobar()
{
int i=0;
SetCoreNum(12);
RetValue(1);//RetTrue;
while(i<10000){i++;}
res+=i;
return;
}
 void bar()
{
int i=0;
SetCoreNum(16);
while(i<100000){i++;}
res+=i;
return;
}

 void starter()
{
int i=0;
while(i<1000000){i++;}
res+=i;
return;
}
*/