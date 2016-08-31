#include "e_main_sched.h"
#include "scheduler.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "e_lib.h"


int i=0; 
char  nullchar[19]={'\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'};
e_core_stat_t volatile cores_stat[16] SECTION("shared_dram");



int main(void) {
e_coreid_t coreid;
char * p=(char *)cores_stat;
	unsigned x=0,y=0,id=0;
    coreid = e_get_coreid();
    x=coreid%64-8;
    y=coreid/64-32;
    id=y*4+x;
    int time;
    //e_ctimer_start(E_CTIMER_0, 0);
    int (*fn)(); 
   // finish();
	while(1)
	{
		i=0;
       while(*(p_int(p,id,STATUS))!=E_SCHEDULED) {}//wait(1);//stay idle til scheduled task
   
   	memcpy(&fn,p_char(p,id,FUNC_ADDR),sizeof(int));
	  (*(p_int(p,id,STATUS)))=E_RUNNING;  
    //tick();  //fix tick tock functions!!!!!!!    
      (*fn)();
    //  *((unsigned*)(p_char(p,id,TIME)))=tock(); //fix tick tock functions!!!!!!!
      
 //     (*(p+15*4*id+48))=e_ctimer_get(E_CTIMER_0);
	  	*(p_int(p,id,STATUS))=E_ENDED;
	}
	return EXIT_SUCCESS;
}
/*
if a function has raised the flag in core 0, than the scheduler will check which of the cores had a value not equal to zero in (p+15*4*id+32)*/
inline int SetQuota_(char *funcName,int num)
{
e_coreid_t coreid;
unsigned x=0,y=0,id=0;
coreid = e_get_coreid();
char * p=(char *)cores_stat;
x=coreid%64-8;
y=coreid/64-32;
id=y*4+x;   
int flagid=id+1; //so i know when core 0 will raise the flag
while(*(p_int(p,0,QUETA_FLAG))!=0){}//make sure flag is 0 allows multiple set_Q
(*(p_int(p,id,QUETA_SIZE)))=num;
//(*(p+15*4*id+32))=strlen(funcName)+1;
//memcpy(p+15*4*id+8,funcName);
int strLength=strlen(funcName);
if (strLength>11) strLength=11;
memcpy(p_char(p,id,FUNC_NAME),funcName,strLength*sizeof(char));
memcpy(p_char(p,id,FUNC_NAME)+strLength,nullchar,12-strLength*sizeof(char));
(*(p_int(p,0,QUETA_FLAG)))=flagid;

return 0;
}

inline int SetCoreNum(unsigned char  num)
{
char * p=(char *)cores_stat;
*(p_int(p,0,SET_CORE_NUM))=num;
while(*(p_int(p,0,SET_CORE_NUM))!=0){}
}




inline int RetValue(unsigned char ret)
{
e_coreid_t coreid;
unsigned x=0,y=0,id=0;
coreid = e_get_coreid();
char * p=(char *)cores_stat;
x=coreid%64-8;
y=coreid/64-32;
id=y*4+x;   
*(p_int(p,id,RET_VAL))=ret;
return 0;

}
//#define RetTrue  { e_coreid_t coreid; coreid = e_get_coreid(); *((((char *)cores_stat)+15*4(coreid/64-32)*4+(coreid%64-8))+56)=1;}
//#define RetFalse { e_coreid_t coreid; coreid = e_get_coreid(); *((((char *)cores_stat)+15*4(coreid/64-32)*4+(coreid%64-8))+56)=0;}


inline int TaskInst()
{
e_coreid_t coreid;
unsigned x=0,y=0,id=0;
coreid = e_get_coreid();
char * p=(char *)cores_stat;
x=coreid%64-8;
y=coreid/64-32;
id=y*4+x;   
return *(p_int(p,id,DUP_ITERATIONS));
}

inline int SetGroup_(char *funcName1, char *funcName2, int num1, int num2)
{
e_coreid_t coreid;
unsigned x=0,y=0,id=0;
coreid = e_get_coreid();
char * p=(char *)cores_stat;
x=coreid%64-8;
y=coreid/64-32;
id=y*4+x;   
int flagid=id+1; //so i know when core 0 will raise the flag
while(*(p_int(p,0,GROUP_FLAG))!=0){}//make sure flag is 0 allows multiple set_Q
(*(p_int(p,0,GROUP_FLAG)))=-2;//mark as busy
//phase 1
(*(p_int(p,id,GROUP_SIZE)))= num1;
//(*(p+15*4*id+32))=strlen(funcName)+1;
//memcpy(p+15*4*id+8,funcName);
int strLength=strlen(funcName1);
if (strLength>11) strLength=11;
memcpy(p_char(p,id,FUNC_NAME),funcName1,strLength*sizeof(char));
memcpy(p_char(p,id,FUNC_NAME)+strLength,nullchar,12-strLength*sizeof(char));
while(*(p_int(p,0,GROUP_FLAG))!=-3){}//make sure flag is 0 allows multiple set_Q
(*(p_int(p,0,GROUP_FLAG)))=flagid;


while(*(p_int(p,0,GROUP_FLAG))!=-3) {}
(*(p_int(p,0,GROUP_FLAG)))=-2;//mark as busy
 //phase 2
(*(p_int(p,id,GROUP_SIZE)))= num2;
//(*(p+15*4*id+32))=strlen(funcName)+1;
//memcpy(p+15*4*id+8,funcName);
strLength=strlen(funcName2);
if (strLength>11) strLength=11;
memcpy(p_char(p,id,FUNC_NAME),funcName2,strLength*sizeof(char));
memcpy(p_char(p,id,FUNC_NAME)+strLength,nullchar,12-strLength*sizeof(char));
(*(p_int(p,0,GROUP_FLAG)))=flagid;
return 0;
}
/*
inline int SetGroup_(char *funcName1, char *funcName2, int num1, int num2)
{
e_coreid_t coreid;
unsigned x=0,y=0,id=0;
coreid = e_get_coreid();
char * p=(char *)cores_stat;
x=coreid%64-8;
y=coreid/64-32;
id=y*4+x;   
int flagid=id+1; //so i know when core 0 will raise the flag
while(*(p_int(p,0,GROUP_FLAG))!=0){}//make sure flag is 0 allows multiple set_Q
//phase 1
(*(p_int(p,id,GROUP_SIZE)))=num1;
//(*(p+15*4*id+32))=strlen(funcName)+1;
//memcpy(p+15*4*id+8,funcName);
int strLength=strlen(funcName1);
if (strLength>11) strLength=11;
memcpy(p_char(p,id,FUNC_NAME),funcName1,strLength*sizeof(char));
memcpy(p_char(p,id,FUNC_NAME)+strLength,nullchar,12-strLength*sizeof(char));
(*(p_int(p,0,GROUP_FLAG)))=flagid;

while(*(p_int(p,0,GROUP_FLAG))!=-1) {}
 //phase 2
(*(p_int(p,id,GROUP_SIZE)))=num2;
//(*(p+15*4*id+32))=strlen(funcName)+1;
//memcpy(p+15*4*id+8,funcName);
strLength=strlen(funcName2);
if (strLength>11) strLength=11;
memcpy(p_char(p,id,FUNC_NAME),funcName2,strLength*sizeof(char));
memcpy(p_char(p,id,FUNC_NAME)+strLength,nullchar,12-strLength*sizeof(char));
(*(p_int(p,0,GROUP_FLAG)))=flagid;

return 0;
}
*/