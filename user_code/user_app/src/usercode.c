#include "dec.h"
#include "../../../tools/scheduler/src/e_main_sched.h"
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
    while(i<10000){i++;}
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
while(i<1000){i++;}
res+=i;
return;
}