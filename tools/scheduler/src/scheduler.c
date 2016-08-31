#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include<time.h> 
#include <e-hal.h>
#include "scheduler.h"
#include "timing.h"
#include "io_handle.h"
#define IO_PLOT   1
#define _BufSize   (1024)
#define _BufOffset (0x01000000)
e_core_stat_t cores_stat[MAXCORES];
static clock_t t1=0;
static clock_t t2=0;
#define e_clock_div   250
#define arm_clock_div 2000
#define idle_c 1000000
int idlecountdown=idle_c;
unsigned  inline tick()
{

 // t1=clock();
  t1=rt_tick(); 
 return (unsigned)0.0;
}
unsigned  inline tock()
{

  //t2=clock()-t1; 
 t2=rt_tock();

 return (unsigned)t2;

}

int main(int argc, char *argv[])
{
int tempTask,tempSize;
    e_mem_t p,p2;
	unsigned  coreid, i,j, k;
	e_platform_t platform;
	e_epiphany_t dev;
	char emsg[_BufSize];
	char* cs=(char*)cores_stat;
	TABLE_LINE* sTable=NULL;
	TABLE_LINE* dTable=NULL;
	int type;
	int A_ID, B_ID, C_ID;
	int A, B, C;
	int DEBUG=0;
 char table_name[100];
 printf("\n");
  diag3(DEBUG) printf("argc=%d\n",argc);
  diag3(DEBUG) printf("argv1=%s\n",argv[1]);          
  diag3(DEBUG) printf("argv2=%s\n",argv[2]);                         
  if (argc>1) {sprintf(table_name,"./%s",argv[1]);} else sprintf(table_name,"./table.bin");
  printf("task_map binfile set as |%s|\n",table_name);
  if (argc>2) {DEBUG=atoi(argv[2]);} else DEBUG=0;
 printf("DEBUG level set to %d\n",DEBUG);	
	int CoresNum=256;
 
  //load tables from task.map
	CreateTable(&sTable,table_name,&table_size,0);
	CreateTable(&dTable,table_name,&table_size,0);
	//printf("created table \n");
	
	//initialize dynamic table
	InitTable(dTable);

  // initialize system, read platform params from
	// default HDF. Then, reset the platform and
	// get the actual system parameters.
  //	printf("pre-init \n");
	e_init(NULL);
	e_reset_system();
	e_get_platform_info(&platform);
 
 if(CoresNum>platform.rows*platform.cols)
   {
      CoresNum=platform.rows*platform.cols;
      printf("due to hardwere limits ");
   }
    printf("num of cores set to %d \n",CoresNum);
	e_alloc(&p,_BufOffset,sizeof(cores_stat));
	init_cores_stat(cores_stat);
	e_write(&p,0 , 0, 0x0,cores_stat,sizeof(cores_stat));
	e_open(&dev, 0,0,4 ,4);
	e_reset_group(&dev);
		// Load the device program onto the selected eCore
		// and launch after loading.
	//printf("loading \n");
 //	printf("%d\n",platform.rows*platform.cols);
  
	e_load_group("e-usercode.srec", &dev,0 ,0,4,4, E_FALSE);
 if(IO_PLOT)
 {
  printf("\nSETTING INPUTS\n");
  handle_input("init");
  printf("\n dumping INPUTS\n");
  handle_output("begining");
  }
e_start_group(&dev);
	printf("start loop \n");
	//printf("loaded \n");
   
  diag2(DEBUG) for(j=0;j<table_size;j++)
   {
      printf("Line %d: %d & (%d | %d) ", j, dTable[j].A_ID, dTable[j].B_ID, dTable[j].C_ID);
      printf("conditions: %d %d %d ", dTable[j].A, dTable[j].B, dTable[j].C);
      printf("duplicate ITERATIONS: %d %d %d\n ", dTable[j].A_IT,dTable[j].B_IT, dTable[j].C_IT);

   }
   
   int *coreState=(int*)malloc(CoresNum*2*sizeof(int));
  // int coreState[2*CoresNum]={-1};
 diag3(DEBUG) printf("----------tick tock test %8.0u %8.0u----------------------------------------------\n",tick(),tock()/arm_clock_div);
   printf("at: %8.0u Task Map execution starts-------------------------------------------------\n",tick());
   int var,temp;
   int corenum;
    while(1)
	{
		for (i=0;i<CoresNum;i++)
        {
     
      	//print_cores_stat(i,(e_core_stat_t *)(cs+15*4*i));
      	e_read(&p,0 , 0,(int) p_char(0,i,0),p_char(cs,i,0),15*4);
        //check if corenum changed
        e_read(&p,0 , 0,(int) p_char(0,0,SET_CORE_NUM), p_char(cs,0,SET_CORE_NUM),sizeof(char));
      
      
      //find an empty flag  
      if(*(p_int(cs,0,SET_CORE_NUM))!=0)
      {
        CoresNum=*(p_int(cs,0,SET_CORE_NUM));//char to int cast
          printf("at: %8.0u : num of cores set to %d    ---!!!!!!!!!!!!!!-----------\n",tock()/arm_clock_div/arm_clock_div,CoresNum);
        *(p_int(cs,0,SET_CORE_NUM))=0;
         e_write(&p,0 , 0,(int)p_char(0,0,SET_CORE_NUM), p_char(cs,0,SET_CORE_NUM),sizeof(int));
      }
      
      
			//check if setQuota had occured:
      e_read(&p,0 , 0,  (int)p_char(0,0,QUETA_FLAG), p_char(cs,0,QUETA_FLAG),sizeof(int));
      if(*(p_int(cs,0,QUETA_FLAG))!=0)
      {
      corenum=(*(p_int(cs,0,QUETA_FLAG)))-1;
  
          e_read(&p,0 , 0,(int) p_char(0,corenum,0), p_char(cs,corenum,0),15*4);
       
          printf("at: %8.0u ,core %2d : task %-15s  SetQuota      %4d          --------------\n",tock()/arm_clock_div,corenum,p_char(cs,corenum,FUNC_NAME),*p_int(cs,corenum,QUETA_SIZE));
      (*(p_int(cs,0,QUETA_FLAG))=0);
      e_write(&p,0 , 0, (int)p_char(0,0,QUETA_FLAG), p_char(cs,0,QUETA_FLAG),sizeof(int));
      char *funcName;
     	int s;
      //diag3(DEBUG) printf("function name is %s ",p_char(cs,corenum,FUNC_NAME));
       for(s = 0; s < table_size; s++)
       {
          
	     	  if(!(strcmp(p_char(cs,corenum,FUNC_NAME), dTable[s].func)))
             {
                dTable[s].dupIterations= (*(p_int(cs,corenum,QUETA_SIZE)))-1;
                //diag3(DEBUG) printf("found and updated qouta to %d \n",dTable[s].dupIterations);
             }
       }
      }
      
      //check if setGroup had occured:
      e_read(&p,0 , 0,  (int)p_char(0,0,GROUP_FLAG), p_char(cs,0,GROUP_FLAG),sizeof(int));
      if(*(p_int(cs,0,GROUP_FLAG))!=0)
      {
      (*(p_int(cs,0,GROUP_FLAG))=-3);
       e_write(&p,0 , 0, (int)p_char(0,0,GROUP_FLAG), p_char(cs,0,GROUP_FLAG),sizeof(int));
       while(*(p_int(cs,0,GROUP_FLAG))<0) {e_read(&p,0 , 0, (int) p_char(0,0,GROUP_FLAG), p_char(cs,0,GROUP_FLAG),sizeof(int));}
      corenum=(*(p_int(cs,0,GROUP_FLAG)))-1;
  
          e_read(&p,0 , 0, (int)p_char(0,corenum,0), p_char(cs,corenum,0),15*4);
          //phase 1
         // printf("at: %8.0u ,core %2d : task %-15s  SetGroup      %4d          --------------\n",tock()/arm_clock_div,corenum,p_char(cs,corenum,FUNC_NAME),*p_int(cs,corenum,GROUP_SIZE));
      
      char *funcName;
     	int s;
       for(s = 0; s < table_size; s++)
	     	  if(!(strcmp(p_char(cs,corenum,FUNC_NAME), dTable[s].func)))
             {
               tempTask=s;
               tempSize=(*(p_int(cs,corenum,GROUP_SIZE)));
               printf("s=%d\n",s);
               break;
             }
               printf("phase 1 function name is %s tempsize=%d \n",p_char(cs,corenum,FUNC_NAME),*(p_int(cs,corenum,GROUP_SIZE)));
               (*p_char(cs,corenum,FUNC_NAME))=0;
       
       printf("phase 1 after flag\n");
       //phase 2
        (*(p_int(cs,0,GROUP_FLAG))=-3);
       e_write(&p,0 , 0, (int)p_char(0,0,GROUP_FLAG), p_char(cs,0,GROUP_FLAG),sizeof(int));
       while(*(p_int(cs,0,GROUP_FLAG))<0) {e_read(&p,0 , 0, (int) p_char(0,0,GROUP_FLAG), p_char(cs,0,GROUP_FLAG),sizeof(int));}
       
       e_read(&p,0 , 0, (int)p_char(0,corenum,0), p_char(cs,corenum,0),15*4);
          printf("phase 2 function name is %s tempsize=%d \n",p_char(cs,corenum,FUNC_NAME),*(p_int(cs,corenum,GROUP_SIZE)));
     
       for(s = 0; s < table_size; s++)
	     	  if(!(strcmp(p_char(cs,corenum,FUNC_NAME), dTable[s].func)))
             {
             printf("task %d depends on task %d\n",tempTask,s);
           if(dTable[tempTask].A_ID==s)
               {
                 if((dTable[tempTask].A!=START_GROUP)&&(dTable[tempTask].A!=FINISH_GROUP))
                   printf("error - the condition in A is not a group!\n");
                 else
                 {
                   dTable[tempTask].A_FLAG=tempSize; //init value!!!
                   dTable[tempTask].A_IT=tempSize;
                   sTable[tempTask].A_FLAG=tempSize;
                   sTable[tempTask].A_IT=tempSize;
                   dTable[tempTask].A=ONE;
                   printf("A : %d,task2 %d, %d :%d\n",tempTask,s, dTable[tempTask].A_FLAG, dTable[tempTask].A_IT);
                 }
               }
             else if(dTable[tempTask].B_ID==s)
             {
                 if((dTable[tempTask].B!=START_GROUP)&&(dTable[tempTask].B!=FINISH_GROUP))
                   printf("error - the condition in B is not a group!\n");
                 else
                 {
                   dTable[tempTask].B_FLAG=tempSize;
                   dTable[tempTask].B_IT=tempSize;
                   sTable[tempTask].B_FLAG=tempSize;
                   sTable[tempTask].B_IT=tempSize;
                   dTable[tempTask].B=ONE;
                   printf("B : %d,task2 %d, %d :%d\n",tempTask,s, dTable[tempTask].B_FLAG, dTable[tempTask].B_IT);
                 }
             }
             else if(dTable[tempTask].C_ID==s)
             {
                 if((dTable[tempTask].C!=START_GROUP)&&(dTable[tempTask].C!=FINISH_GROUP))
                   printf("error - the condition in C is not a group!\n");
                 else
                 {
                   dTable[tempTask].C_FLAG=tempSize;
                   dTable[tempTask].C_IT=tempSize;
                   sTable[tempTask].C_FLAG=tempSize;
                   sTable[tempTask].C_IT=tempSize;
                   dTable[tempTask].C=ONE;
                   printf("C : %d,task2 %d, %d :%d\n",tempTask,s, dTable[tempTask].C_FLAG, dTable[tempTask].C_IT);
                 }
             }
             /*
               if(dTable[tempTask].A_ID==s)
               {
                 if((dTable[tempTask].A!=START_GROUP)&&(dTable[tempTask].A!=FINISH_GROUP))
                   printf("error - the condition in A is not a group!\n");
                 else
                 {
                   dTable[tempTask].A_FLAG=tempSize; //init value!!!
                   dTable[tempTask].A_IT=(*(p_int(cs,corenum,GROUP_SIZE)));
                   sTable[tempTask].A_FLAG=tempSize;
                   sTable[tempTask].A_IT=(*(p_int(cs,corenum,GROUP_SIZE)));
                   printf("A : %d,task2 %d, %d :%d\n",tempTask,s, dTable[tempTask].A_FLAG, dTable[tempTask].A_IT);
                 }
               }
             else if(dTable[tempTask].B_ID==s)
             {
                 if((dTable[tempTask].B!=START_GROUP)&&(dTable[tempTask].B!=FINISH_GROUP))
                   printf("error - the condition in B is not a group!\n");
                 else
                 {
                   dTable[tempTask].B_FLAG=tempSize;
                   dTable[tempTask].B_IT=(*(p_int(cs,corenum,GROUP_SIZE)));
                   sTable[tempTask].B_FLAG=tempSize;
                   sTable[tempTask].B_IT=(*(p_int(cs,corenum,GROUP_SIZE)));
                   printf("B : %d,task2 %d, %d :%d\n",tempTask,s, dTable[tempTask].B_FLAG, dTable[tempTask].B_IT);
                 }
             }
             else if(dTable[tempTask].C_ID==s)
             {
                 if((dTable[tempTask].C!=START_GROUP)&&(dTable[tempTask].C!=FINISH_GROUP))
                   printf("error - the condition in C is not a group!\n");
                 else
                 {
                   dTable[tempTask].C_FLAG=tempSize;
                   dTable[tempTask].C_IT=(*(p_int(cs,corenum,GROUP_SIZE)));
                   sTable[tempTask].C_FLAG=tempSize;
                   sTable[tempTask].C_IT=(*(p_int(cs,corenum,GROUP_SIZE)));
                   printf("C : %d,task2 %d, %d :%d\n",tempTask,s, dTable[tempTask].C_FLAG, dTable[tempTask].C_IT);
                 }
             }
             */
               if(dTable[s].A_ID==tempTask)
               {
                 if((dTable[s].A!=START_GROUP)&&(dTable[s].A!=FINISH_GROUP))
                   printf("error - the condition in A is not a group!\n");
                 else
                 {
                   dTable[s].A_FLAG=(*(p_int(cs,corenum,GROUP_SIZE)));
                   dTable[s].A_IT=(*(p_int(cs,corenum,GROUP_SIZE)));
                   sTable[s].A_FLAG=(*(p_int(cs,corenum,GROUP_SIZE)));
                   sTable[s].A_IT=(*(p_int(cs,corenum,GROUP_SIZE)));
                   dTable[s].A=ONE;
                   printf("A : %d,task2 %d, %d :%d\n",tempTask,s, dTable[tempTask].A_FLAG, dTable[tempTask].A_IT);
                 }
               }
             else if(dTable[s].B_ID==tempTask)
             {
                 if((dTable[s].B!=START_GROUP)&&(dTable[s].B!=FINISH_GROUP))
                   printf("error - the condition in B is not a group!\n");
                 else
                 {
                   dTable[s].B_FLAG=(*(p_int(cs,corenum,GROUP_SIZE)));
                   dTable[s].B_IT=(*(p_int(cs,corenum,GROUP_SIZE)));
                   sTable[s].B_FLAG=(*(p_int(cs,corenum,GROUP_SIZE)));
                   sTable[s].B_IT=(*(p_int(cs,corenum,GROUP_SIZE)));
                   dTable[s].B=ONE;
                   printf("B : %d,task2 %d, %d :%d\n",tempTask,s, dTable[tempTask].B_FLAG, dTable[tempTask].B_IT);
                 }
             }
             else if(dTable[s].C_ID==tempTask)
             {
                 if((dTable[s].C!=START_GROUP)&&(dTable[s].C!=FINISH_GROUP))
                   printf("error - the condition in C is not a group!\n");
                 else
                 {
                   dTable[s].C_FLAG=(*(p_int(cs,corenum,GROUP_SIZE)));
                   dTable[s].C_IT=(*(p_int(cs,corenum,GROUP_SIZE)));
                   sTable[s].C_FLAG=(*(p_int(cs,corenum,GROUP_SIZE)));
                   sTable[s].C_IT=(*(p_int(cs,corenum,GROUP_SIZE)));
                   dTable[s].C=ONE;
                   printf("C : %d,task2 %d, %d :%d\n",tempTask,s, dTable[tempTask].C_FLAG, dTable[tempTask].C_IT);
                 }
             }
             
             else
             printf("error - the first task does not depend on the second task\n"); 
             }
      (*(p_int(cs,0,GROUP_FLAG))=0);
      e_write(&p,0 , 0, (int)p_char(0,0,GROUP_FLAG), p_char(cs,0,GROUP_FLAG),sizeof(int));  
       diag2(DEBUG) for(j=0;j<table_size;j++)
   {
      printf("Line %d: %d & (%d | %d) ", j, dTable[j].A_ID, dTable[j].B_ID, dTable[j].C_ID);
      printf("conditions: %d %d %d ", dTable[j].A, dTable[j].B, dTable[j].C);
      printf("duplicate ITERATIONS: %d %d %d\n ", dTable[j].A_IT,dTable[j].B_IT, dTable[j].C_IT);

   }
      }
      
 	    if (E_ENDED==*(p_int(cs,i,STATUS)))
			{
      
				//printf("done");
				//UPDATE TABLE - take id# from relevant place in memory and change task to not running
				if(dTable[*(p_int(cs,i,TASK_ID))].type==REGULAR)
				{
          coreState[2*i]=-1; 
          coreState[2*i+1]=-1; 
               if (*(p_int(cs,i,RET_VAL))==0)
                   {dTable[*(p_int(cs,i,TASK_ID))].exec_state = FINISH_0;}
          else if (*(p_int(cs,i,RET_VAL))==1)
                   {dTable[*(p_int(cs,i,TASK_ID))].exec_state = FINISH_1;}
          else {dTable[*(p_int(cs,i,TASK_ID))].exec_state = FINISH_U; 
          printf("retval not 0 or 1 !! %d\n",*(p_int(cs,i,RET_VAL)));}
					dTable[*(p_int(cs,i,TASK_ID))].exec_state = FINISH_0 + *(p_int(cs,i,RET_VAL));                            
    	    printf("at: %8.0u ,core %2d : task %-15s  terminated  retval %d        cycles= %8d\n",tock()/arm_clock_div, i,dTable[*(p_int(cs,i,TASK_ID))].func,dTable[*(p_int(cs,i,TASK_ID))].exec_state-8,
          (unsigned)((*((unsigned*)(p_char(cs,i,TIME))))/e_clock_div));
          
					//UpdateTable(dTable, sTable, *(cs+15*4*i), FINISH_0 + *(cs+15*4*i+56),1);//why here why not at ended tasks?
          UpdateTable(dTable, sTable, *(p_int(cs,i,TASK_ID)), dTable[*(p_int(cs,i,TASK_ID))].exec_state,1);//why here why not at ended tasks?
				    
				}
				if(dTable[*(p_int(cs,i,TASK_ID))].type==DUPLICABLE)
				{
        temp =-1;

        
          
          coreState[2*i]=-1; 
          coreState[2*i+1]=-1; 
					dTable[*(p_int(cs,i,TASK_ID))].counter--;
            
            for(var=0;var<16;var++)
          {
            if(coreState[2*var]==(*(p_int(cs,i,TASK_ID))))
              if((coreState[2*var+1]<temp) || (temp==-1))
                temp=coreState[2*var+1]-1;
          }
          if(temp!=-1)
             dTable[*(p_int(cs,i,TASK_ID))].lastEnded= temp;
             else
             {
             //	printf("no tasks running updating %d to %d\n",dTable[*(cs+15*4*i)].lastEnded,dTable[*(cs+15*4*i)].lastIterationStarted-1);
              dTable[*(p_int(cs,i,TASK_ID))].lastEnded= dTable[*(p_int(cs,i,TASK_ID))].lastIterationStarted-1;
              }
          
          	printf("at: %8.0u ,core %2d : task %-15s  terminated  iteration %4d  cycles= %8d\n",tock()/arm_clock_div, i,dTable[*(p_int(cs,i,TASK_ID))].func,*(p_int(cs,i,DUP_ITERATIONS)),(unsigned)((*((unsigned*)(p_char(cs,i,TIME)))/e_clock_div)));  
                            
          UpdateTable(dTable, sTable, *(p_int(cs,i,TASK_ID)), FINISH_IT,*(p_int(cs,i,DUP_ITERATIONS)));       
				
					if(dTable[*(p_int(cs,i,TASK_ID))].counter==0 &&dTable[*(p_int(cs,i,TASK_ID))].lastIterationStarted > dTable[*(p_int(cs,i,TASK_ID))].dupIterations)
					{
						dTable[*(p_int(cs,i,TASK_ID))].exec_state = FINISH_U;
            
     	      printf("at: %8.0u ,core %2d : task %-15s  terminated\n",tock()/arm_clock_div, i,dTable[*(p_int(cs,i,TASK_ID))].func);
			         	UpdateTable(dTable, sTable, *(p_int(cs,i,TASK_ID)), FINISH_U,*(p_int(cs,i,DUP_ITERATIONS)));//why here why not at ended tasks?
			        
					}
				}
					//print_cores_stat(i,(e_core_stat_t *)(cs+15*4*i));
				*(p_int(cs,i,STATUS))=E_IDLE;
				e_write(&p,0 , 0,(int) p_char(0,i,0), p_char(cs,i,0),15*4);
			}

			if (E_RUNNING==*(p_int(cs,i,STATUS)))
			{	
				//printf("core %d :running task id %d\n", i,*(cs+15*4*i));  
			} 
			if (E_IDLE==*(p_int(cs,i,STATUS)))
			{
       			//printf("core %d : scheduled task id %d \n", i,*(cs+15*4*i));

				//check if any tasks should be run. if so run them and then update them to running
 
				for(j=0;j<table_size;j++)
				{
          if((dTable[j].type==DUMMY) && (dTable[j].A == ONE && (dTable[j].B == ONE  || dTable[j].C == ONE )))
          {
         
          printf("at: %8.0u ,core %2d : task %-15s  EXECUTED    ----(task is type dummy)------\n",tock()/arm_clock_div, i, dTable[j].func);   
          UpdateTable(dTable, sTable, j, START,1);
          UpdateTable(dTable, sTable, j, FINISH_U,1);
          j=0;
          }
      //this if checks if scheduling is needed  
					if ((dTable[j].A == ONE && (dTable[j].B == ONE  || dTable[j].C == ONE )) && dTable[j].lastIterationStarted <= dTable[j].dupIterations)
					{
						dTable[j].exec_state = START;
            coreState[2*i]=j;
            coreState[2*i+1]=0;
						*(p_int(cs,i,TASK_ID))=j;  
           // e_write(&p, 0, 0, 15*4*i+4, &dTable[j].funcAddress, sizeof(dTable[j].funcAddress));
          // *(cs+15*4*i+4)= dTable[j].funcAddress;
            //(*(cs+15*4*i+4))=(unsigned int) 1512;
            memcpy(p_char(cs,i,FUNC_ADDR),&dTable[j].funcAddress,sizeof(int));
          //  printf("i should write %d, but im writing %d\n",dTable[j].funcAddress,(int) *(cs+15*4*i+4));
						*(p_int(cs,i,TASK_TYPE))=dTable[j].type;
						if(dTable[j].type==DUPLICABLE)
						{
						*(p_int(cs,i,DUP_ITERATIONS))=dTable[j].lastIterationStarted;    
            coreState[2*i+1]=dTable[j].lastIterationStarted;         
					  dTable[j].counter++;    
                                
            printf("at: %8.0u ,core %2d : task %-15s  scheduled   iteration %4d\n",tock()/arm_clock_div, i, dTable[j].func,dTable[j].lastIterationStarted); 
            if(dTable[j].lastIterationStarted==0)
              UpdateTable(dTable, sTable, *(p_int(cs,i,TASK_ID)), START,*(p_int(cs,i,DUP_ITERATIONS)));   
                
            UpdateTable(dTable, sTable, *(p_int(cs,i,TASK_ID)), START_IT,*(p_int(cs,i,DUP_ITERATIONS))); 
            //we check the window condition here!! WHY? we are already scheduling task!
            
            dTable[j].lastIterationStarted++;
						}
            else 
            {
           
	            	printf("at: %8.0u ,core %2d : task %-15s  scheduled  \n",tock()/arm_clock_div, i, dTable[j].func);                                    
               UpdateTable(dTable, sTable, j, START,1);   
                
           }

 					  *(p_int(cs,i,STATUS))=E_SCHEDULED; //need to change to e-write.
						//  printf("Line %d: %d & (%d | %d) \n", j, dTable[j].A, dTable[j].B, dTable[j].C);
              
     					e_write(&p,0 , 0, (int)p_char(0,i,0), p_char(cs,i,0),15*4);
						//printf("( %-15s , %d ,%d , %d , %d ) //(task, inst,min,N,core)\n",  dTable[j].func,dTable[j].lastIterationStarted,dTable[j].counter,dTable[j].dupIterations,i);//note the -1 bug! //counter should be min running
				 
        		break;
					}
					if(j == table_size - 1)
					{
						for(k=0;k<CoresNum;k++)
						{
							if(*(p_int(cs,i,STATUS)) != E_IDLE) {idlecountdown=idle_c; break;}
								
						}
              
           if((k == CoresNum)&&(idlecountdown>0)) {idlecountdown--;}
           else if ((k == CoresNum)&&(idlecountdown==0))
							{
								
								e_free(&p);
								
								//DelTable(sTable);
								//DelTable(dTable);
								free(sTable);
								free(dTable);
                free(coreState);
								printf("at: %8.0u -----------------------------Task Map DONE--------------------------------\n",tock()/arm_clock_div);
                printf("at: %8.0u -----------------------------CoresNum=%d--------------------------------\n",tock()/arm_clock_div,CoresNum);
            if(IO_PLOT)
           {	
  					  	printf("\n dumping OUTPUT\n");
                handle_output("finish");
          }
                //e_finalize();	
                //e_close(&dev);
              	return 0;
							}    
					}
				}
			}
			// Wait for core program execution to finish, then
			// read message from shared buffer.
		}
		//print_cores_stat(cores_stat);
   //break;
	}

}

