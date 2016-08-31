#include "table.h"
#define MAXCORES 16
#define diag1(x) if (x>=1) 
#define diag2(x) if (x>=2)
#define diag3(x) if (x>=3)
typedef enum {
    TASK_ID=0,                                           //p+0
    FUNC_ADDR=4,                                         //p+4
    FUNC_NAME=8,                                         //p+8
    GROUP_FLAG=20,                                       //p+20
    GROUP_SIZE=24,                                       //p+24
    QUETA_FLAG=28,                                       //p28
    QUETA_SIZE=32,                                       //p+32
    SET_CORE_NUM=36,                                     //p+36
    DUP_ITERATIONS=40,                                   //p+40
    TASK_TYPE=42,                                        //p+44
    TIME=48,                                             //p+48
    STATUS=52,                                           //p+52
    RET_VAL=56                                           //p+56
} shift;
//#define p_int(p,core,shift) (int*)((p)+(15*4*(core))+(shift))  
//#define p_char(p,core,shift) ((p)+(15*4*(core))+(shift))   
#define p_int(p,core,shift) (int*)(((char*)p)+(15*4*(core))+(shift))
#define p_char(p,core,shift)(char*)(((char*)p)+(15*4*(core))+(shift))
typedef enum {
        E_IDLE,// core has no tasks scheduled
        E_SCHEDULED,//scheduled task ,but yet to start
        E_RUNNING,//task started
        E_ENDED //task ended , yet was not processed by scheduler
} e_status_t;
typedef enum{
    E_RET_TRUE,E_RET_FALSE,E_DONTCARE
}ret_val_t;

typedef struct {
    int task_id;                                      //p+0
    unsigned int funcAddr;                            //p+4
    char funcName[12];                                //p+8
    int group_mail_box;                               //p+20
    int group_size;                                   //p+24
    int func_name_length;                             //p+28  
    int queta_size;                                   //p+32
    int queta_mail_box;                               //p+36
    int dupIteration;                                 //p+40
    int task_type;                                    //p+44
    int time;                                         //p+48
    int status;                                       //p+52
    int ret_val;                                      //p+56
} e_core_stat_t;
//typedef int  e_core_stat_t;

void init_cores_stat(e_core_stat_t* p)
{
	int i;
  for (i=0;i<16;i++)
  {
    p[i].task_id=0;
    p[i].funcAddr=0;
    p[i].funcName;
    p[i].func_name_length=0;
    p[i].queta_size=1;
    p[i].queta_mail_box=0;
    p[i].dupIteration=0;
    p[i].task_type=0;
    p[i].time=0;
    p[i].status=E_IDLE;
    p[i].ret_val=E_DONTCARE;
   //p[i]=0;
  }
}




/*
typedef struct {
    int task_id;
    unsigned int funcAddr;
    int start_i_y;
    int start_i_z;
    int current_i_x,current_i_y,current_i_z;
    int func_name_length;
    int queta_size,queta_mail_box;
    int dupIteration, task_type, qouta_z;
    int status;
    int ret_val;
} e_core_stat_t;
//typedef int  e_core_stat_t;

void init_cores_stat(e_core_stat_t* p)
{
	int i;
  for (i=0;i<16;i++)
  {
    p[i].task_id=0;
    p[i].funcAddr=0;
    p[i].start_i_y=0;
    p[i].start_i_z=0;
    p[i].current_i_x=0;
    p[i].current_i_y=0;
    p[i].current_i_z=0;
    p[i].func_name_length=0;
    p[i].queta_size=1;
    p[i].queta_mail_box=0;
    p[i].dupIteration=0;
    p[i].task_type=0;
    p[i].qouta_z=0;
    p[i].status=E_IDLE;
    p[i].ret_val=E_DONTCARE;
   //p[i]=0;
  }
}
*/