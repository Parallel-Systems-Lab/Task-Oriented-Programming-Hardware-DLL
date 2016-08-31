#include "table.h"
#include <stdlib.h>
int _DEBUG=3;
int DelTable(TABLE_LINE* pTable)
{
		free(pTable);
}



int CreateTable(TABLE_LINE** pTable, char* output_name,int* size, int DEBUG)
{
	_DEBUG=DEBUG;
FILE*  fp=fopen(output_name, "rb");
int i;
 if(NULL==fp) { printf("error: cant open file ||%s|| \n",output_name);return -1;}
 diag1(DEBUG) 	printf(" reading from %s\n",output_name);
 fread(size,sizeof(int),1,fp);
 
 diag1(DEBUG)  	printf(" read  %d\n",*size);
 TABLE_LINE* p = (TABLE_LINE*)malloc(sizeof(TABLE_LINE) * (*size));//allocate static table memory
 for(i=0;i<*size;i++)
 {
 fread(&(p[i]),sizeof(TABLE_LINE),1,fp);
 diag1(DEBUG)  	printf(" line %d is : %d %d %d %d %d %d \n",i, p[i].A_ID, p[i].A, p[i].B_ID, p[i].B, p[i].C_ID, p[i].C);
 }
 fclose(fp);

 *pTable=p;
}	
 
//init dynamic table
int InitTable(TABLE_LINE* pTable)
{
  int i;
	for(i=0;i<table_size;i++)
	{
    pTable[i].lastEnded=-1;
		pTable[i].exec_state = ZERO;//is this what I want to set it to? unnecessary?
		pTable[i].dupIterations = 1;
		pTable[i].lastIterationStarted =0;
		pTable[i].counter = 0;
   	pTable[i].A_FLAG=0;
    pTable[i].B_FLAG=0;
    pTable[i].C_FLAG=0;
    //if the condition is a window or an init task - it can start immediately
		if((pTable[i].A == INIT) ||  (((pTable[i].A == FINISH_WINDOW)) && (pTable[i].A_IT>0)))
			pTable[i].A = ONE;
		if(pTable[i].B == INIT || (((pTable[i].B == FINISH_WINDOW))&& (pTable[i].B_IT>0)))
			pTable[i].B = ONE;
		if(pTable[i].C == INIT || (((pTable[i].C == FINISH_WINDOW))&& (pTable[i].C_IT>0)))
			pTable[i].C = ONE;
	}
	return 0;
}
/*
EvalLogic - checks the status of a condition (A,B or C)
~~~~~~~~~~~~~~~
int req            - what is the condition that we check 
int itReq          - the condition for duplicable conditions.
int myWindow       - what is the size of the window in the condition of the line the we are checking.
int status         - what is my status (running, finished etc.)
int iteration      - in a normal condition - which of my iteration started / finished 
                     in a window condition - what is the last iteration that started / finished in the **other** task (the task that triggered the update table)
int lastEnded      - what is most advanced iteration that ended in **id** task
int funcId         - 
int nextIteration  - what is the next iteration that i shold start
int myid           -
int funcLastEnded  - what is the most advanced iteration that finished in the **my** task
int *mem_var       - a flag 
int myGroup
int idGroup
~~~~~~~~~~~~~~~
return the new status of the condition
*/
int EvalLogic(int req,int itReq,int myWindow, int status, int iteration, int lastEnded, int funcId,int nextIteration, int myid, int funcLastEnded, int *mem_var)
{
diag3(_DEBUG) printf("EvalLogic:,req=%d\n,itReq=%d\n,myWindow=%d\n,status=%d\n,iteration=%d\n,lastEnded=%d\n,funcId=%d\n,nextIteration=%d\n,myid=%d\n,funcLastEnded=%d\n",req,itReq,myWindow,status,iteration,lastEnded,funcId,nextIteration,myid,funcLastEnded); 
	if(req==ZERO) 
	{
		return 0;
	}
	else if(req==ONE)
	{
		return 1;
	}
 else if(req==FINISH_1)
  {
    //printf("req ==FINISH 1 and status =%d\n",status);
    if(status==FINISH_1)
    {
       return 1;
    }
    else
       return 0; 
  } 
  else if(req==FINISH_0)
  {
    if(status==FINISH_0)
       return 1;
    else
       return 0; 
  }
 	else if(req==FINISH_U)
	{
		if(status==FINISH_0 || status==FINISH_1 || status==FINISH_U)
			return 1;
    else
      return 0;
	}
	else if(req==START)
	{
		if(status==START)
			return 1;
    else
      return 0;
	}
 else if(req==FINISH_IT)
 {
   if((itReq==iteration) && (status==FINISH_IT) )
      return 1;
   else
      return 0;
 }
  else if(req==START_IT)
 {
   if((itReq==iteration) && (status==START_IT) )
      return 1;
   else
      return 0;
 }
  else if(req==START_GROUP)
  {
    if(status==START_IT)
    {
    //printf("started %d iterations, group size is %d  ",iteration+1,itReq);
        if((iteration+1)%itReq==0)
        {
       // printf("Go! \n");
           return 1;
           }
        else
        {
        //printf("No Go (but yet to schedule all grouped iterations.... condition is wrong)\n");
          return 0;
          }
    }
    else if(status==FINISH_0 || status==FINISH_1 || status==FINISH_U)
    {
   // printf("finished\n");
      return 3;
    }
    else 
      return 0;
  }
  else if(req==FINISH_GROUP)
  {
  if(status==FINISH_IT)
    {
        //printf("finished %d iterations, group size is %d\n ",iteration+1,myWindow);
        //if((iteration+1)%myWindow==0) itReq
        if((iteration+1)%myWindow==0) 
        {
      //  printf("Go! (add to flag) \n");
           return 1;
           }
        else
        {
      //  printf("No Go\n");// (but yet to schedule all grouped iterations.... condition is wrong)\n");
    //     printf("No Go dosnt always print bug in eval sending?\n i found one where you send with id instead of i \n");
          return 0;
          }
    }
    else if(status==FINISH_0 || status==FINISH_1 || status==FINISH_U)
      return 3;
    else 
      return 0;
  }
  else if(req==FINISH_ALL_IT_BEFORE)
  {
          //int i,last_ended=-1; 
    
     //printf("the last iteration is %d\n",last_ended);
    // printf("%d\n",itReq);
    if((lastEnded<itReq))
    {
    // printf("i cannot run the task\n");
    *mem_var=1;
    }
     if((lastEnded>=itReq)&&((*mem_var)==1))
     {
     *mem_var=0;
     //    printf("i can run the task\n");
        return 1;
     }
     else
         return 0;
  }
   else if(req==START_ALL_IT_BEFORE)
  {
          //int i,last_ended=-1; 
    
    // printf("STARTED iteration is %d    itreq = %d ",iteration,itReq);
    if((iteration<itReq) && (status==START_IT))
    {
    // printf("i cannot run the task\n");
    *mem_var=1;
    }
     if((iteration>=itReq)&&((*mem_var)==1))
     {
     *mem_var=0;
   //      printf("i can run the task\n");
        return 1;
     }
     else
         return 0;
  }
   else if(req==FINISH_WINDOW)
   {
   
        //    printf("the last iteration is %d\n",last_ended); 
       if((status==START_IT))
        {
      //  printf("funcLastEnded = %d\n myGroup = %d\n myWindow = %d\n",funcLastEnded,myGroup,myWindow);
      //  printf("iteration = %d\n idGroup=%d\n",iteration,idGroup);
      //  printf("(funcLastEnded/myGroup)+myWindow =%d\n",(funcLastEnded/myGroup)+myWindow);
      //  printf("(iteration/idGroup) =%d\n",(iteration/idGroup));
         //printf("start - triger %d iteraion?: %d+%d>%d?",iteration+1,lastEnded,myWindow,iteration);
          //printf("funcLastEnded %d +  myWindow = %d  > iteration-1 %d \n",funcLastEnded,myWindow,iteration-1);
         if(funcLastEnded==-2)
             {         
             //printf("yes the other task had finished\n");
            return 3; // the other task had finished
            } 
            //when we`ll learn to send some copies of the task together we wont need this.
          //  if(nextIteration%myGroup !=0)
            //        return 0;
            // the last "i" task ended + "id" window size > "id" iteration ?
           
          else if((funcLastEnded)+myWindow > (iteration))//-1//or other task ended and should be trigered
          {
          //   printf("YES \n");
             return 0;// i can still run tasks
          }
          else
          {
            //  printf("NO! \n"); 
             return 2;// i can't run more tasks!
            }
         }
         else if(status==FINISH_IT)
         {
     //    printf("lastEnded = %d\n idGroup = %d\n itReq = %d\n nextIteration = %d\n myGroup = %d\n",lastEnded,idGroup,itReq,nextIteration,myGroup);
      //   printf("(lastEnded/idGroup)+itReq =%d\n",(lastEnded/idGroup)+itReq);
      //  printf("(nextIteration/myGroup)-1 =%d\n",(nextIteration/myGroup)-1);
         //printf("termination  triger %d iteraion? : %d+%d>%d?  ",nextIteration,lastEnded,itReq,nextIteration-1);
         //the last "id" task ended + "i" window size > next "i" iteration
         if((lastEnded)+itReq>(nextIteration)-1)
         {
         //printf("YES \n");
            return 3; // the other core can run too!
         }
          else
         {
         //printf("NO! \n");
             return 0; // the other core cannot run yet.
         }
         }
       else if(status==FINISH_U)
           {  
           //printf(" the other task had finished\n");
             return 4; //the other core had finished
           }
        else
        {
             return 0;
        }
     }
     else if(req==START_WINDOW)
     {
        if((status==START_IT))
         {
         
            if(myWindow==0)
            {         
             //  printf("yes the other task had finished\n");
                  return 3; // the other task had finished
            }
            //next "i" iteration + "i" window size > "id" iteration
          
            else  
            {
            //when we`ll learn to send some copies of the task together we wont need this.
          //  if(nextIteration%myGroup !=0)
            //        return 1;
                          //    printf("%d +%d <= %d? ",nextIteration,myWindow,iteration);
                if((nextIteration)+myWindow <= (iteration))
                {
                //  printf("RUN\n");
                  return 1;
                }
               else
                 {
                 //  printf("DONT RUN\n");
                   return 2;
                 }
            }
          }
          /*
          else if(status==FINISH_U)//START_IT && iteration>=MAX_ITERATION 
             return 4;
          else
             return 0;
             */
     }
	else
	{
		return (req==status);
	}
}
/*
UpdateTable - the main program calls this method every time something happens.
the goal of the function is to get what changed in the program and chage the table accordingly  
~~~~~~~~~~~~~~~
TABLE_LINE* pTable          - the dynamic table
TABLE_LINE* pTable_static   - the static table
int id                      - the id of the task that changed
int state                   - what happend to this task (started, ended etc.)
int iteration               - which iteration of the task changed
~~~~~~~~~~~~~~~
*/
int UpdateTable(TABLE_LINE* pTable, TABLE_LINE* pTable_static, int id, int state, int iteration)
{
  int i;
  int res;
  //check all the table
  diag3(_DEBUG) printf("task %d enters UpdateTable.\n",id);
	for(i=0;i<table_size;i++)
	{
 diag3(_DEBUG) printf("table line %d\n",i);
 diag3(_DEBUG) printf("   A   \n");
      if(pTable_static[i].A==START_WINDOW && ((id == pTable[i].A_ID) || (id == i))) //
          {
          if(id == pTable[i].A_ID)
              {
            
              res=EvalLogic(START_WINDOW,0,pTable[i].A_IT,state,pTable[pTable[i].A_ID].lastIterationStarted,0,0,pTable[i].lastIterationStarted,0,0,NULL);
              if(pTable[pTable[i].A_ID].lastIterationStarted> pTable[pTable[i].A_ID].dupIterations)
                  res=4;
              }
          else 
              res=EvalLogic(START_WINDOW,0,pTable[i].A_IT,state,pTable[pTable[i].A_ID].lastIterationStarted,0,0,pTable[i].lastIterationStarted,0,0,NULL);

           if((res==1) && (id == pTable[i].A_ID))
           {
              pTable[i].A = ONE;
            //  pTable[pTable[id].A_ID].A = pTable_static[pTable[id].A_ID].A;
           }
           else if((res==2) && (id == i)) 
             {
            //    printf("limit starter\n");
                pTable[i].A = pTable_static[i].A;
            //    pTable[pTable[id].A_ID].A = ONE;
             }
             else if((res==4) && (id == pTable[i].A_ID))
             {
            //  printf("ret 4\n");
               pTable[i].A_IT=0;//potantial BUG
                pTable[i].A = ONE;
              //  pTable[id].A = pTable_static[id].A;
          //      pTable[pTable[id].A_ID].A = ONE;
             }
             else if((res==3) && (id == i)) 
             {
              //  pTable[id].A = pTable_static[id].A;
           //    printf("i started enough tasks so the other core can run\n");
                pTable[i].A = ONE;
             } 
          }
          
		else if(pTable[i].A_ID == id)  // there are some things that depend on me
      {
          //int EvalLogic(int req,int itReq,int myWindow, int status, int iteration, int lastEnded, int funcId,int nextIteration, int myid, int funcLastEnded, int *mem_var)
          if(EvalLogic(pTable[i].A,pTable[i].A_IT,pTable[id].A_IT, state,iteration,pTable[id].lastEnded,pTable[i].A_ID,pTable[i].lastIterationStarted,id,pTable[i].lastEnded,&(pTable[i].A_FLAG)))
	      	{
          if((pTable_static[i].A==FINISH_WINDOW))
          {
          res=EvalLogic(FINISH_WINDOW,pTable[i].A_IT,pTable[id].A_IT, state,iteration,pTable[id].lastEnded,pTable[i].A_ID,pTable[i].lastIterationStarted,id,pTable[i].lastEnded,&(pTable[i].A_FLAG));
            if(res==2) //i started all the tasks that i could
            
             {
             //   printf("i started all the tasks that i could\n");
                pTable[id].A = pTable_static[id].A;
                //pTable[i].A = ONE;//?
             }
             else if(res==3) // i finished enough tasks so the other core can run
             {
              //  pTable[id].A = pTable_static[id].A;
              // printf("i finished enough tasks so the other core can run\n");
                pTable[i].A = ONE;
             }
              else if(res==4) //one task had finished
             {
            //  printf("one task had finished\n");
               pTable[id].lastEnded=-2;//potantial BUG
              //  pTable[id].A = pTable_static[id].A;
                pTable[i].A = ONE;
             }
          }
          else if((pTable_static[i].A==FINISH_GROUP)||(pTable_static[i].A==START_GROUP))
          {
          res=EvalLogic(pTable_static[i].A,pTable[i].A_IT,pTable[id].A_IT, state,iteration,pTable[id].lastEnded,pTable[i].A_ID,pTable[i].lastIterationStarted,id,pTable[i].lastEnded,&(pTable[i].A_FLAG));
                if(res==3)
                {

                   pTable[i].A_FLAG=-1;
                   pTable[i].A = ONE;
                }
                else if(res==1)
                {
                //printf("flag = %d, adding %d\n",pTable[i].A_FLAG,pTable_static[i].A_FLAG);
                pTable[i].A_FLAG+=pTable_static[i].A_FLAG;
                pTable[i].A = ONE;
                }
                
                
         }
          else
		       	pTable[i].A = ONE;
	      	}
          
      }
    diag3(_DEBUG)   printf("   B  \n");
		if(pTable_static[i].B==START_WINDOW && ((id == pTable[i].B_ID) || (id == i)))
          {
          if(id == pTable[i].B_ID)
              {
              
              res=EvalLogic(START_WINDOW,0,pTable[i].B_IT,state,pTable[pTable[i].B_ID].lastIterationStarted,0,0,pTable[i].lastIterationStarted,0,0,NULL);
              if(pTable[pTable[i].B_ID].lastIterationStarted> pTable[pTable[i].B_ID].dupIterations)
                  res=4;
              }
          else 
               res=EvalLogic(START_WINDOW,0,pTable[i].B_IT,state,pTable[pTable[i].B_ID].lastIterationStarted,0,0,pTable[i].lastIterationStarted,0,0,NULL); 
              

           if((res==1) && (id == pTable[i].B_ID))
           {
              pTable[i].B = ONE;
            //  pTable[pTable[id].A_ID].A = pTable_static[pTable[id].A_ID].A;
           }
           else if((res==2) && (id == i)) 
             {
            //    printf("limit starter\n");
                pTable[i].B = pTable_static[i].B;
            //    pTable[pTable[id].A_ID].A = ONE;
             }
             else if((res==4) && (id == pTable[i].B_ID))
             {
            //  printf("ret 4\n");
               pTable[i].B_IT=0;//potantial BUG
                pTable[i].B = ONE;
              //  pTable[id].A = pTable_static[id].A;
          //      pTable[pTable[id].A_ID].A = ONE;
             }
             else if((res==3) && (id == i)) 
             {
              //  pTable[id].A = pTable_static[id].A;
           //    printf("i started enough tasks so the other core can run\n");
                pTable[i].B = ONE;
             } 
          }
          
		else if(pTable[i].B_ID == id) 
      {
        //   printf("  OZ B_ID  \n");
      //int EvalLogic(int req,int itReq,int myWindow, int status, int iteration, int lastEnded, int funcId,int nextIteration, int myid, int funcLastEnded, int *mem_var)          
          if(EvalLogic(pTable[i].B,pTable[i].B_IT,pTable[id].B_IT, state,iteration,pTable[id].lastEnded,pTable[i].B_ID,pTable[i].lastIterationStarted,id,pTable[i].lastEnded,&(pTable[i].B_FLAG)))
          //should id be i??? i replaced it in this line only //oz
         // if(EvalLogic(pTable[i].B,pTable[i].B_IT,pTable[i].B_IT, state,iteration,pTable[i].lastEnded,pTable[i].B_ID,pTable[i].lastIterationStarted,id,pTable[i].lastEnded,&(pTable[i].B_FLAG)))
	      	{
          if((pTable_static[i].B==FINISH_WINDOW))
          { diag3(_DEBUG)  printf("   B  FINISH_WINDOW\n");
          res=EvalLogic(FINISH_WINDOW,pTable[i].B_IT,pTable[id].B_IT, state,iteration,pTable[id].lastEnded,pTable[i].B_ID,pTable[i].lastIterationStarted,id,pTable[i].lastEnded,&(pTable[i].B_FLAG));
            if(res==2) //i started all the tasks that i could
            
             {
             //   printf("i started all the tasks that i could\n");
                pTable[id].B = pTable_static[id].B;
                //pTable[i].A = ONE;//?
             }
             else if(res==3) // i finished enough tasks so the other core can run
             {
              //  pTable[id].A = pTable_static[id].A;
              // printf("i finished enough tasks so the other core can run\n");
                pTable[i].B = ONE;
             }
              else if(res==4) //one task had finished
             {
            //  printf("one task had finished\n");
               pTable[id].lastEnded=-2;//potantial BUG
              //  pTable[id].A = pTable_static[id].A;
                pTable[i].B = ONE;
             }
          }
        else if((pTable_static[i].B==FINISH_GROUP)||(pTable_static[i].B==START_GROUP))
                {
               res=EvalLogic(pTable[i].B,pTable[i].B_IT,pTable[id].B_IT, state,iteration,pTable[id].lastEnded,pTable[i].B_ID,pTable[i].lastIterationStarted,id,pTable[i].lastEnded,&(pTable[i].B_FLAG));
                if(res==3)
                {
                   pTable[i].B_FLAG=-1;
                   pTable[i].B = ONE;
                }
                else if(res==1)
                {
                pTable[i].B_FLAG+=pTable_static[i].B_FLAG;
                pTable[i].B = ONE;
                }
                }
          else
		       	pTable[i].B = ONE;
	      	}
          
      }
      
      
     diag3(_DEBUG)  printf("   C   \n");
	if(pTable_static[i].C==START_WINDOW && ((id == pTable[i].C_ID) || (id == i)))
          {
          if(id == pTable[i].C_ID)
              {
              res=EvalLogic(START_WINDOW,0,pTable[i].C_IT,state,pTable[pTable[i].C_ID].lastIterationStarted,0,0,pTable[i].lastIterationStarted,0,0,NULL);
              if(pTable[pTable[i].C_ID].lastIterationStarted> pTable[pTable[i].C_ID].dupIterations)
                  res=4;
              }
          else 
              res=EvalLogic(START_WINDOW,0,pTable[i].C_IT,state,pTable[pTable[i].C_ID].lastIterationStarted,0,0,pTable[i].lastIterationStarted,0,0,NULL);

           if((res==1) && (id == pTable[i].C_ID))
           {
              pTable[i].C = ONE;
            //  pTable[pTable[id].A_ID].A = pTable_static[pTable[id].A_ID].A;
           }
           else if((res==2) && (id == i)) 
             {
            //    printf("limit starter\n");
                pTable[i].C = pTable_static[i].C;
            //    pTable[pTable[id].A_ID].A = ONE;
             }
             else if((res==4) && (id == pTable[i].C_ID))
             {
            //  printf("ret 4\n");
               pTable[i].C_IT=0;//potantial BUG
                pTable[i].C = ONE;
              //  pTable[id].A = pTable_static[id].A;
          //      pTable[pTable[id].A_ID].A = ONE;
             }
             else if((res==3) && (id == i)) 
             {
              //  pTable[id].A = pTable_static[id].A;
           //    printf("i started enough tasks so the other core can run\n");
                pTable[i].C = ONE;
             } 
          }
          
		else if(pTable[i].C_ID == id) 
      {
          
          if(EvalLogic(pTable[i].C,pTable[i].C_IT,pTable[id].C_IT, state,iteration,pTable[id].lastEnded,pTable[i].C_ID,pTable[i].lastIterationStarted,id,pTable[i].lastEnded,&(pTable[i].C_FLAG)))
	      	{
          if((pTable_static[i].A==FINISH_WINDOW))
          {
          res=EvalLogic(FINISH_WINDOW,pTable[i].C_IT,pTable[id].C_IT, state,iteration,pTable[id].lastEnded,pTable[i].C_ID,pTable[i].lastIterationStarted,id,pTable[i].lastEnded,&(pTable[i].C_FLAG));
            if(res==2) //i started all the tasks that i could
            
             {
             //   printf("i started all the tasks that i could\n");
                pTable[id].C = pTable_static[id].C;
                //pTable[i].A = ONE;//?
             }
             else if(res==3) // i finished enough tasks so the other core can run
             {
              //  pTable[id].A = pTable_static[id].A;
              // printf("i finished enough tasks so the other core can run\n");
                pTable[i].C = ONE;
             }
              else if(res==4) //one task had finished
             {
            //  printf("one task had finished\n");
               pTable[id].lastEnded=-2;//potantial BUG
              //  pTable[id].A = pTable_static[id].A;
                pTable[i].C = ONE;
             }
          }
         else if((pTable_static[i].C==FINISH_GROUP)||(pTable_static[i].C==START_GROUP))
                {
               res=EvalLogic(pTable[i].C,pTable[i].C_IT,pTable[id].C_IT, state,iteration,pTable[id].lastEnded,pTable[i].C_ID,pTable[i].lastIterationStarted,id,pTable[i].lastEnded,&(pTable[i].C_FLAG));
                if(res==3)
                {
                   pTable[i].C_FLAG=-1;
                   pTable[i].C = ONE;
                }
                else if(res==1)
                {
                pTable[i].C_FLAG+=pTable_static[i].C_FLAG;
                pTable[i].C = ONE;
                }
                }
          else
		       	pTable[i].C = ONE;
	      	}
          
      }
      if((pTable[pTable_static[i].A_ID].lastIterationStarted > pTable[pTable_static[i].A_ID].dupIterations)&&(pTable_static[i].A==START_GROUP))
                {
                   pTable[i].A_FLAG=-1;
                   pTable[i].A = ONE;
                }
      if((pTable[pTable_static[i].B_ID].lastIterationStarted > pTable[pTable_static[i].B_ID].dupIterations)&&(pTable_static[i].B==START_GROUP))
                {
                   pTable[i].B_FLAG=-1;
                   pTable[i].B = ONE;
                }
        if((pTable[pTable_static[i].C_ID].lastIterationStarted > pTable[pTable_static[i].C_ID].dupIterations)&&(pTable_static[i].C==START_GROUP))
                {
                   pTable[i].C_FLAG=-1;
                   pTable[i].C = ONE;
                }
      
	}
  
  
  
 
               
 if((state==START_IT) && ((pTable_static[id].A==FINISH_GROUP)||(pTable_static[id].A==START_GROUP)))
 {
 //printf("flag staus is: %d\n",pTable[id].A_FLAG-1);
    if((--pTable[id].A_FLAG)==0)
          pTable[id].A = pTable_static[id].A;
 
   
 }
 if((state==START_IT) && ((pTable_static[id].B==FINISH_GROUP)||(pTable_static[id].B==START_GROUP)))
 {
 //printf("i can run %d tasks\n",pTable[id].B_FLAG-1);
//printf("flag staus is: %d\n",pTable[id].B_FLAG-1);
    if((--pTable[id].B_FLAG)==0)
    {
          pTable[id].B = pTable_static[id].B;
    }

 }
 if((state==START_IT) && ((pTable_static[id].C==FINISH_GROUP)||(pTable_static[id].C==START_GROUP)))
 {
 //printf("flag staus is: %d\n",pTable[id].C_FLAG-1);
    if((--pTable[id].C_FLAG)==0)
          pTable[id].C = pTable_static[id].C;   
 }
	
 
 
 
	if((state==START) && ((pTable[id].type==REGULAR) || (pTable[id].type==DUMMY)))
	{
 //printf("task %d resets.\n",id);
		pTable[id].A = pTable_static[id].A;
		pTable[id].B = pTable_static[id].B;
		pTable[id].C = pTable_static[id].C;
	}
 if(((state==START_IT) && (pTable[id].lastIterationStarted >= pTable[id].dupIterations)))
 {
  //  printf("task %d resets.\n",id);
 	  pTable[id].A = pTable_static[id].A;
		pTable[id].B = pTable_static[id].B;
		pTable[id].C = pTable_static[id].C;
 }
 if(((state==FINISH_U) && (pTable[id].type==DUPLICABLE)))
 {
   pTable[i].lastEnded=-1;
   pTable[id].lastIterationStarted=0;
	 pTable[i].counter = 0;
   	pTable[i].A_FLAG=0;
    pTable[i].B_FLAG=0;
    pTable[i].C_FLAG=0;
 }
 
	return 0;
}