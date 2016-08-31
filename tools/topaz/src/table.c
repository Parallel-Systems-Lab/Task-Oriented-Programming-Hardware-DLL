#include "table.h"
#include <stdlib.h>

#define diag1(x) if (x>=1) 
#define diag2(x) if (x>=2)
#define diag3(x) if (x>=3)
#define max_func_name_langth 20
char task_map_name[100];
char lut_name[100];
char output_name[100];

int main(int argc ,char **argv )
{
if(argc<4){  printf("usage :topaz task.map symboltable.txt output%d\n",table_size); return -1;}
int DEBUG=4;
if(argc=5)
{
DEBUG=atoi(argv[4]);
diag1(DEBUG) printf("debug=%d\n",DEBUG);

}
diag3(DEBUG) printf("argc=%d\n",argc);
diag2(DEBUG) printf("arg1 ||%s|| \n",argv[1]);
diag2(DEBUG) printf("arg2 ||%s|| \n",argv[2]);
diag2(DEBUG) printf("arg3 ||%s|| \n",argv[3]);
strcpy(task_map_name,argv[1]);
strcpy(lut_name,argv[2]);
strcpy(output_name,argv[3]);
table_size=getTableSize(DEBUG);
if (1>table_size) {  printf("getTableSize returned %d\n",table_size); return table_size;}
	TABLE_LINE* sTable = (TABLE_LINE*)malloc(sizeof(TABLE_LINE)*table_size);
 if(0>CreateTable_stg1(sTable,DEBUG)){printf("(CreateTable_stg1) failed , task map has errors\n");   return -1;}
if(0>CreateTable_stg2(sTable,DEBUG)){printf(" (CreateTable_stg2)failed , task map has errors\n");   return -1;}
if(0>reflection(sTable,DEBUG)){printf("(reflection) failed , c code compiled successfully?\n"); return -1;}
if(0>plot_table(table_size,sTable,DEBUG)){printf("(plot_table) failed \n"); return -1;}
if(0>read_table(&table_size,sTable,DEBUG)){printf("(plot_table) failed \n"); return -1;}
diag1(DEBUG) printf("task map loaded successfuly \n");
}
int plot_table(int size,TABLE_LINE* pTable, int DEBUG)
{
FILE* fp=fopen(output_name, "wb");
int i;
 if(NULL==fp) { printf("error: cant open file ||%s|| \n",output_name);return -1;}
 diag2(DEBUG)  	printf(" writing to %s\n",output_name);
 fwrite(&size,sizeof(int),1,fp);
 diag2(DEBUG)  	printf(" wrote  %d\n",size);
 for(i=0;i<size;i++)
 {
 fwrite(&(pTable[i]),sizeof(pTable[i]),1,fp);
 diag2(DEBUG)  	printf(" wrote line %d  : %d %d %d %d %d %d \n",i, pTable[i].A_ID, pTable[i].A, pTable[i].B_ID, pTable[i].B, pTable[i].C_ID, pTable[i].C);
 }
 fclose(fp);
}
int read_table(int * size,TABLE_LINE* pTable, int DEBUG)
{
FILE*  fp=fopen(output_name, "rb");
int i;
 if(NULL==fp) { printf("error: cant open file ||%s|| \n",output_name);return -1;}
 diag2(DEBUG)  	printf(" reading from %s\n",output_name);
 fread(size,sizeof(int),1,fp);
diag2(DEBUG)  	printf(" read  %d\n",*size);
 for(i=0;i<*size;i++)
 {
 fread(&(pTable[i]),sizeof(pTable[i]),1,fp);
 diag2(DEBUG)  	printf(" line %d is : %d %d %d %d %d %d \n",i, pTable[i].A_ID, pTable[i].A, pTable[i].B_ID, pTable[i].B, pTable[i].C_ID, pTable[i].C);
 }
 fclose(fp);
}

int getTableSize(int DEBUG)
{

int size=0;
    FILE *fp;
	char line[100];
	
	//load the taskmap
	fp=fopen(task_map_name, "r");
 if(NULL==fp) { printf("error: cant open file ||%s|| \n",task_map_name);return -1;}
	table_size = 0;
int emptylines=0;
	while(fgets(line, 100, fp) != NULL)
 {
 		if(NULL!=strtok(line, " ()/ \r \n"))
     {	
       size++;
     }else  
     {
     emptylines++;
     printf("error: task map has empty line at line %d\n",size+emptylines);
     }
 }
	fclose(fp);
 if (emptylines)
            {
              printf("error: task map has %d empty lines \n",emptylines);
              return -emptylines;
            }
	diag3(DEBUG) printf("%s has %d tasks\n",task_map_name,table_size);
 return size;
}

int funcToId(char* func, TABLE_LINE* pTable)
{
	int i;
	for(i = 0; i < table_size; i++)
	{	
		if(!(strcmp(func, pTable[i].func)))
			return i;
	}
	return -1;
}

int CreateTable_stg1(TABLE_LINE* pTable,int DEBUG)
{
int error_detected=0;

char*tmp_strtok=NULL;
char line[100], buffer[100], input[100], lutline[100];
 char *temp;
    FILE *fp,*lut;
	int and, or, i, j, empty;
diag3(DEBUG)	printf("opening file %s  \n",task_map_name);
	fp=fopen(task_map_name, "r");
if(NULL==fp) { printf("error: cant open file ||%s|| \n",task_map_name);return -1;}

	//initialize static table
 TABLE_LINE line_p;
	for(i = 0; i < table_size; )
	{	
		fgets(line, 100, fp);
  //  diag1(DEBUG)	printf("allocating memory for table line \n");
	// TABLE_LINE* pTable[i] = (TABLE_LINE*)malloc(sizeof(TABLE_LINE));
	  
    //stage 1 task type ---------------------------
     diag2(DEBUG) printf("line %d:TYPE task name(Acond & (Bcond | Ccond))\n",i+1);
    tmp_strtok=strtok(line, " ()/");
    	//strcpy(buffer, );//extract task type
     // handle empty line!
		  if(!(strcmp(tmp_strtok, "regular")))
		  {
			  line_p.type = REGULAR;
       diag2(DEBUG) printf("line %d:  type is: %s \n",i+1, "regular");
		  }else
	    if(!(strcmp(tmp_strtok, "duplicable")))
		  {
			  line_p.type = DUPLICABLE;
       diag2(DEBUG) printf("line %d: type is: %s \n",i+1, "duplicable");
		  }else 
		  if(!(strcmp(tmp_strtok, "dummy")))
		  {
			  line_p.type = DUMMY;
       diag2(DEBUG) printf("line %d: type is: %s \n",i+1, "dummy");
		  }else
      if(!(strcmp(tmp_strtok, "hardware")))
		  {
			  line_p.type = HARDWARE;
       diag2(DEBUG) printf("line %d: type is: %s \n",i+1, "hardware");
		  }else
        {
        printf("error line %d: unknown task type ||%s|| SKIPING LINE\n",i+1, tmp_strtok);
        error_detected++;
        continue;
        }
		
   
   //stage 2 the word task------------------------------------ 
   tmp_strtok=strtok(NULL, " ");
   diag3(DEBUG) printf("line %d:_____ TASK name(Acond & (Bcond | Ccond))\n",i+1);
	//	strcpy(buffer, strtok(NULL, " ()/"));
   if(strcmp(tmp_strtok, "task"))
   {
    printf("error line %d: task expected after task type  SKIPING LINE\n",i+1);
    error_detected++;
    continue;
   }
   //stage 3 task name ------------------------------------
   tmp_strtok=strtok(NULL, " (/");
		diag3(DEBUG) printf("line %d:_____ ______ NAME (Acond & (Bcond | Ccond))\n",i+1);
     if(NULL==tmp_strtok)
		  {
			  printf("error line %d: task name expected after task \n",i+1);
        error_detected++;
		  }else
      if(strlen(tmp_strtok)>max_func_name_langth)
		  {
			  printf("error line %d: task name must be under %d charecters but it is %d long SKIPING LINE\n",i+1,max_func_name_langth,strlen(tmp_strtok));
        error_detected++;
        continue;
		  }else
       {
       strcpy(line_p.func, tmp_strtok);//extract func name
       diag2(DEBUG) printf(" line %d: task name is :%s\n",i+1,line_p.func);
       }
   diag3(DEBUG)	printf("line %d OK allocating memory for table line \n",i+1);
   strcpy(pTable[i].func,line_p.func);
   pTable[i].type=line_p.type;
   i++;
	 
  }
  fclose(fp);
         diag1(DEBUG) printf(" ------------table so far---------------\n");
   for (j=0;j<table_size; j++)
     { 
         diag1(DEBUG) printf(" line %d: %d task %s\n",j+1,pTable[j].type,pTable[j].func);
           
     }
     diag1(DEBUG) printf(" -------------------------------------------\n");
  
 
return -error_detected;
}
int CreateTable_stg2(TABLE_LINE* pTable,int DEBUG)
{
int error_detected=0;
int i;
char*tmp_strtok=NULL;
	char line[100], buffer[100], input[100], lutline[100];
 char *temp;
    FILE *fp;

	diag3(DEBUG)	printf("opening file %s  (now that we know tasks types we can farther evaluate conditions)\n",task_map_name);
	//load the taskmap
	fp=fopen(task_map_name, "r");
if(NULL==fp) { printf("error: cant open file ||%s|| \n",task_map_name);return -1;}
  
  int Atype=-1;
  int Btype=-1;
  int Ctype=-1;
  int swapAB=-1;
	for(i = 0; i < table_size; i++)
	{	
		swapAB=0;
      fgets(line, 100, fp);
    //stage 4 condition task A------------------------------------
     tmp_strtok=strtok(line, " ");//stage 1
     tmp_strtok=strtok(NULL, " ");//stage 2
     tmp_strtok=strtok(NULL, "");//stage 3
     tmp_strtok+=strlen(pTable[i].func);
     diag3(DEBUG) printf("line %d:_____ ______ (Acond & (Bcond | Ccond))\n ||%s||\n",i+1,tmp_strtok);
     //tmp_strtok=strtok(tmp_strtok, "()/");//stage 4
     if('('!=tmp_strtok[0])  
        {
           printf(" line %d: error expecting '(' after task name SKIPING LINE\n",i+1);
           error_detected++;
           continue;
        }
        tmp_strtok++;
     diag3(DEBUG) printf("line %d:_____ ______ _____ OPEN( \n ||%s||\n",i+1,tmp_strtok);
    
     if(tmp_strtok[0]==')') 
     {
     diag1(DEBUG) printf(" line %d: task condition is empty f() type \n",i+1);
 				pTable[i].A_ID = -1;
				pTable[i].A = INIT;
				pTable[i].B_ID = -1;
				pTable[i].B = ONE;
				pTable[i].C_ID = -1;				
				pTable[i].C = ONE;
 
     continue;
     }
      tmp_strtok=strtok(tmp_strtok, "/[( ");//stage 3
       diag3(DEBUG) printf("line %d: /[(  ||%s||\n",i+1,tmp_strtok);
       
     if(NULL!=tmp_strtok) 
     {
     diag2(DEBUG) printf(" line %d: A condition task name is ||%s||\n",i+1,tmp_strtok);
    int  match=funcToId(tmp_strtok,pTable);
     
     if (-1==match)
     {
        printf(" line %d: error no task named ||%s||  SKIPING LINE\n",i+1,tmp_strtok);
        error_detected++;
           continue;
     }
     
     diag2(DEBUG) printf(" line %d: A condition task %s type  is %d\n",i+1,tmp_strtok,Atype);
     pTable[i].A_ID=match;
     Atype=pTable[match].type;
     }
     else
        {
           printf(" line %d: error expecting task name for A condition SKIPPING LINE\n",i+1);
           error_detected++;
           continue;
        }

     tmp_strtok=strtok(NULL, "");
      diag3(DEBUG) printf("line %d:_____ ______ ___XX]/fu & (Bcond | Ccond))\n ||%s||\n",i+1,tmp_strtok);
    
     if((NULL!=tmp_strtok)&&(tmp_strtok[0]=='<'))
     {
          pTable[i].A=FINISH_ALL_IT_BEFORE;
          diag2(DEBUG) printf(" line %d: < op detected detected  ||%s||\n",i+1,tmp_strtok);
        tmp_strtok++;
        
        tmp_strtok=strtok(tmp_strtok, "]");
        diag2(DEBUG) printf(" line %d: < ||%s|| detected ']'  \n",i+1,tmp_strtok);
           pTable[i].A_IT=atoi(tmp_strtok);//check number
        tmp_strtok=strtok(NULL, "");
        diag3(DEBUG) printf("line %d:_____ ______ ___/fu & (Bcond | Ccond))\n ||%s||\n",i+1,tmp_strtok);
     if ('/'==tmp_strtok[0])
       {
       tmp_strtok++;
       diag3(DEBUG) printf("line %d:_____ ______ ___fu & (Bcond | Ccond))\n ||%s||\n",i+1,tmp_strtok);
       }else 
          {
           printf("\nERROR\nERROR : line %d: '/' expected SKIPING LINE\n",i+1);
           error_detected++;
           continue;
          }
         
     }
     else if((NULL!=tmp_strtok)&&(tmp_strtok[0]=='@'))
     {
        pTable[i].A=FINISH_WINDOW;//default is finish might be changed when return token checked 
        tmp_strtok++;
        diag2(DEBUG) printf(" line %d: @ op detected detected  ||%s||\n",i+1,tmp_strtok);
      
        tmp_strtok=strtok(tmp_strtok, "]");
        diag2(DEBUG) printf(" line %d: @ ||%s|| detected  ']' \n",i+1,tmp_strtok);
          pTable[i].A_IT=atoi(tmp_strtok);
        tmp_strtok=strtok(NULL, "");
        diag3(DEBUG) printf("line %d:_____ ______ ___/fu & (Bcond | Ccond))\n ||%s||\n",i+1,tmp_strtok);
      if ('/'==tmp_strtok[0])
       {
       tmp_strtok++;
       diag3(DEBUG) printf("line %d:_____ ______ ___fu & (Bcond | Ccond))\n ||%s||\n",i+1,tmp_strtok);
       }else 
          {
           printf("\nERROR\nERROR : line %d: '/' expected SKIPING LINE\n",i+1);
           error_detected++;
           continue;
          }
        
     }
          else if((NULL!=tmp_strtok)&&(tmp_strtok[0]=='{'))
     {
        pTable[i].A=FINISH_GROUP;//default is finish might be changed when return token checked 
        tmp_strtok++;
        diag2(DEBUG) printf(" line %d: { op detected detected  ||%s||\n",i+1,tmp_strtok);
      
        tmp_strtok=strtok(tmp_strtok, ",");
        diag2(DEBUG) printf(" line %d: { ||%s|| detected   \n",i+1,tmp_strtok);
          printf("line %d:Matan add K=%d to table ! as group implementation \n",i+1,atoi(tmp_strtok));
          //pTable[i].A_IT=atoi(tmp_strtok); {k,_}
        tmp_strtok=strtok(NULL, "}");
        diag2(DEBUG) printf(" line %d: {k,||%s|| detected   \n",i+1,tmp_strtok);
          printf("line %d: Matan add m=%d to table ! as group implementation \n",i+1,atoi(tmp_strtok));
          //pTable[i].A_IT=atoi(tmp_strtok); {_,m}
        tmp_strtok=strtok(NULL, "]");
        diag3(DEBUG) printf("line %d:_____ ______ ___/fu & (Bcond | Ccond))\n ||%s||\n",i+1,tmp_strtok);
      if ('/'==tmp_strtok[0])
       {
       tmp_strtok++;
       diag3(DEBUG) printf("line %d:_____ ______ ___fu & (Bcond | Ccond))\n ||%s||\n",i+1,tmp_strtok);
       }else 
          {
           printf("\nERROR\nERROR : line %d: '/' expected SKIPING LINE\n",i+1);
           error_detected++;
           continue;
          }
        
     }




     else if((NULL!=tmp_strtok)&&(tmp_strtok[0]>='0')&&(tmp_strtok[0]<='9'))
     {
        pTable[i].A=FINISH_IT;//default is finish might be changed when return token checked
         tmp_strtok=strtok(tmp_strtok, "]");
        diag2(DEBUG) printf(" line %d: number  ||%s|| ']'\n",i+1,tmp_strtok);
         pTable[i].A_IT=atoi(tmp_strtok);
        tmp_strtok=strtok(NULL, "");
        diag3(DEBUG) printf("line %d:_____ ______ ___/fu & (Bcond | Ccond))\n ||%s||\n",i+1,tmp_strtok);
        if ('/'==tmp_strtok[0])
       {
       tmp_strtok++;
       diag3(DEBUG) printf("line %d:_____ ______ ___fu & (Bcond | Ccond))\n ||%s||\n",i+1,tmp_strtok);
       }else 
          {
           printf("\nERROR\nERROR : line %d: '/' expected SKIPING LINE\n",i+1);
           error_detected++;
           continue;
          }
       
     }
     else if((NULL!=tmp_strtok)&&(tmp_strtok[0]=='g'))
     {
        pTable[i].A=FINISH_GROUP;//default is finish might be changed when return token checked 
          tmp_strtok=strtok(tmp_strtok, "]");
          tmp_strtok=strtok(NULL, "");
        diag2(DEBUG) printf(" line %d: g op detected detected  ||%s||\n",i+1,tmp_strtok);
        diag3(DEBUG) printf("line %d:_____ ______ ___/fu & (Bcond | Ccond))\n ||%s||\n",i+1,tmp_strtok);
      if ('/'==tmp_strtok[0])
       {
       tmp_strtok++;
       diag3(DEBUG) printf("line %d:_____ ______ ___fu & (Bcond | Ccond))\n ||%s||\n",i+1,tmp_strtok);
       }else 
          {
           printf("\nERROR\nERROR : line %d: '/' expected SKIPING LINE\n",i+1);
           error_detected++;
           continue;
          }
        
     }
    
      if((NULL!=tmp_strtok)&&(tmp_strtok[0]=='f')) 
     {
        diag2(DEBUG) printf(" line %d: finish token detected\n",i+1);
        if ((FINISH_IT!=pTable[i].A)&&(FINISH_WINDOW!=pTable[i].A)&&(FINISH_ALL_IT_BEFORE!=pTable[i].A)&&(FINISH_GROUP!=pTable[i].A))
        {
          pTable[i].A=FINISH_U;
        }
        tmp_strtok++;
 
        if  (REGULAR==Atype)
         { 
         //stage 5.1  ret_val token------------------------------ ------------------------------------
          diag3(DEBUG) printf(" line %d: regular task checking ret val condition\n",i+1);
          
          if((tmp_strtok[0]=='1')&&(FINISH_U==pTable[i].A)) pTable[i].A=FINISH_1;
          else if((tmp_strtok[0]=='0')&&(FINISH_U==pTable[i].A)) pTable[i].A=FINISH_0;
          else if((tmp_strtok[0]=='u')&&(FINISH_U==pTable[i].A))pTable[i].A=FINISH_U;
           else { printf("\nERROR\nERROR : line %d: ret_val token expected before %c \n",i+1,tmp_strtok[0]); }
          diag2(DEBUG) printf(" line %d: ret_val token ||%c||\n",i+1,tmp_strtok[0]);
          tmp_strtok++;
                  
         }  //else    
             //{
              //  diag3(DEBUG) printf(" line %d: task is not regular no retval token needed\n",i+1);
             //}   
    
    
     }else
     if((NULL!=tmp_strtok)&&(tmp_strtok[0]=='s')) 
     {
        diag2(DEBUG) printf(" line %d: start token detected ||%c||\n",i+1,tmp_strtok[0]);
            if(FINISH_IT==pTable[i].A)              pTable[i].A=START_IT;
       else if(FINISH_WINDOW==pTable[i].A)          pTable[i].A=START_WINDOW;
       else if(FINISH_ALL_IT_BEFORE==pTable[i].A)   pTable[i].A=START_ALL_IT_BEFORE;
       else if(FINISH_GROUP==pTable[i].A)              pTable[i].A=START_GROUP;
       else                                          pTable[i].A=START;
        tmp_strtok++;    
     }else  
     {
      printf("\nERROR\nERROR : line %d: '[num]' , '[<num]' '[@num]'  '/f' or '/s'  expected SKIPPING LINE\n",i+1);
      error_detected++;
       continue;
      }

      tmp_strtok=strtok(tmp_strtok, " ");
        diag3(DEBUG) printf("  line %d:  ||%s|| \n",i+1,tmp_strtok);
      if((NULL!=tmp_strtok)&&(tmp_strtok[0]==')'))
      {
          diag1(DEBUG) printf(" line %d: condition compleated  f(A) type\n",i+1);
           pTable[i].B_ID = -1;
           pTable[i].B=ONE;
           pTable[i].C_ID = -1;
           pTable[i].C=ONE;
          
          continue;
      } else 
      if((NULL!=tmp_strtok)&&(tmp_strtok[0]=='&'))  
     {
         diag2(DEBUG) printf(" line %d: & operand detected no change to f( A&(B|C)) needed\n",i+1);    
     }else 
      if((NULL!=tmp_strtok)&&(tmp_strtok[0]=='|'))  
     {
         diag2(DEBUG) printf("  line %d: | operand detected A is infact B inf( A&(B|C)) \n",i+1);
         swapAB=1;

     }else 
        { 
           printf("\nERROR\nERROR : line %d: unknown operand on stage 6  ||%s|| SKIPING LINE\n",i+1,tmp_strtok);
           error_detected++;
           continue;
        }
    
  
     //cond B handle
      tmp_strtok=strtok(NULL, "");
       diag3(DEBUG) printf("  line %d:2rd operand handle ||%s|| \n",i+1,tmp_strtok);
     
     
     
    int open_B;
    open_B=0; 
     if('('==tmp_strtok[0])  
        {
        tmp_strtok++;
         diag3(DEBUG) printf("line %d:_____ ______ _____ (______ OPEN( \n ||%s||\n",i+1,tmp_strtok);
         open_B=1;
        }
     
      tmp_strtok=strtok(tmp_strtok, "/[( ");//stage 3
       diag3(DEBUG) printf("line %d: /[(  ||%s||\n",i+1,tmp_strtok);
       
     if(NULL!=tmp_strtok) 
     {
     diag3(DEBUG) printf(" line %d: B condition task name is ||%s||\n",i+1,tmp_strtok);
        int match=funcToId(tmp_strtok,pTable);
     
     if (-1==match)
     {
        printf(" line %d: error no task named ||%s||  SKIPING LINE\n",i+1,tmp_strtok);
        error_detected++;
           continue;
     }
     
     diag2(DEBUG) printf(" line %d: B condition task %s type  is %d\n",i+1,tmp_strtok,Atype);
     pTable[i].B_ID=match;
     Btype=pTable[match].type;
     }
     else
        {
           printf(" line %d: error expecting task name for B condition SKIPPING LINE\n",i+1);
           error_detected++;
           continue;
        }
  
     //stage 4.1   [] test---------------------------------------------------------
     tmp_strtok=strtok(NULL, "");
      diag3(DEBUG) printf("line %d:_____ ______ ______ (_____XX]/fu  | Ccond)\n ||%s||\n",i+1,tmp_strtok);
     if((NULL!=tmp_strtok)&&(tmp_strtok[0]=='<'))
     {
         pTable[i].B=FINISH_ALL_IT_BEFORE;
        diag2(DEBUG) printf(" line %d: B cond '<' op detected detected\n",i+1);
        tmp_strtok++;
        
        tmp_strtok=strtok(tmp_strtok, "]");
        diag2(DEBUG) printf(" line %d: < ||%s|| detected ']'  \n",i+1,tmp_strtok);
        pTable[i].B_IT=atoi(tmp_strtok);
  
        tmp_strtok=strtok(NULL, "");
        diag3(DEBUG) printf("line %d:_____ ______ ___(_____/fu  | Ccond)\n ||%s||\n",i+1,tmp_strtok);
      if ('/'==tmp_strtok[0])
       {
       tmp_strtok++;
        diag3(DEBUG) printf("line %d:_____ ______ ___(_____fu  | Ccond)\n ||%s||\n",i+1,tmp_strtok);
       } else 
          {
           printf("\nERROR\nERROR : line %d: '/' expected SKIPING LINE\n",i+1);
           error_detected++;
           continue;
          }
         
     }
     else if((NULL!=tmp_strtok)&&(tmp_strtok[0]=='@'))
     {
        
        diag2(DEBUG) printf(" line %d: @ op detected detected  ||%s||\n",i+1,tmp_strtok);
         pTable[i].B=FINISH_WINDOW;
         tmp_strtok++;
       
         
         
        tmp_strtok=strtok(tmp_strtok, "]");
        diag2(DEBUG) printf(" line %d: @ ||%s|| detected  ']' \n",i+1,tmp_strtok);
        pTable[i].B_IT=atoi(tmp_strtok);
        
        
        tmp_strtok=strtok(NULL, "");
         diag3(DEBUG) printf("line %d:_____ ______ ___(_____/fu  | Ccond)\n ||%s||\n",i+1,tmp_strtok);
        if ('/'==tmp_strtok[0])
       {
       tmp_strtok++;
        diag3(DEBUG) printf("line %d:_____ ______ ___(_____fu  | Ccond)\n ||%s||\n",i+1,tmp_strtok);
       } else 
          {
           printf("\nERROR\nERROR : line %d: '/' expected SKIPING LINE\n",i+1);
           error_detected++;
           continue;
          }
     }
            else if((NULL!=tmp_strtok)&&(tmp_strtok[0]=='{'))
     {
        pTable[i].B=FINISH_GROUP;//default is finish might be changed when return token checked 
        tmp_strtok++;
        diag2(DEBUG) printf(" line %d: { op detected detected  ||%s||\n",i+1,tmp_strtok);
      
        tmp_strtok=strtok(tmp_strtok, ",");
        diag2(DEBUG) printf(" line %d: { ||%s|| detected   \n",i+1,tmp_strtok);
          printf("line %d:Uriel add K=%d to table ! as group implementation \n",i+1,atoi(tmp_strtok));
          //pTable[i].A_IT=atoi(tmp_strtok); {k,_}
        tmp_strtok=strtok(NULL, "}");
        diag2(DEBUG) printf(" line %d: {k,||%s|| detected   \n",i+1,tmp_strtok);
          printf("line %d: Uriel add m=%d to table ! as group implementation \n",i+1,atoi(tmp_strtok));
          //pTable[i].A_IT=atoi(tmp_strtok); {_,m}
        tmp_strtok=strtok(NULL, "]");
        diag3(DEBUG) printf("line %d:_____ ______ ___/fu & (Bcond | Ccond))\n ||%s||\n",i+1,tmp_strtok);
      if ('/'==tmp_strtok[0])
       {
       tmp_strtok++;
       diag3(DEBUG) printf("line %d:_____ ______ ___fu & (Bcond | Ccond))\n ||%s||\n",i+1,tmp_strtok);
       }else 
          {
           printf("\nERROR\nERROR : line %d: '/' expected SKIPING LINE\n",i+1);
           error_detected++;
           continue;
          }
        
     }
     else if((NULL!=tmp_strtok)&&(tmp_strtok[0]>='0')&&(tmp_strtok[0]<='9'))
     {
        
        
        tmp_strtok=strtok(tmp_strtok, "]");
        diag2(DEBUG) printf(" line %d: number  ||%s|| ']'\n",i+1,tmp_strtok);
        pTable[i].B_IT=atoi(tmp_strtok);
        tmp_strtok=strtok(NULL, "");
        diag3(DEBUG) printf("line %d:_____ ______ ___(_____/fu  | Ccond)\n ||%s||\n",i+1,tmp_strtok);
       if ('/'==tmp_strtok[0])
       {
       tmp_strtok++;
        diag3(DEBUG) printf("line %d:_____ ______ ___(_____fu  | Ccond)\n ||%s||\n",i+1,tmp_strtok);
       } else 
          {
           printf("\nERROR\nERROR : line %d: '/' expected SKIPING LINE\n",i+1);
           error_detected++;
           continue;
          }
       
     }
        
       else if((NULL!=tmp_strtok)&&(tmp_strtok[0]=='g'))
     {
        pTable[i].B=FINISH_GROUP;//default is finish might be changed when return token checked 
          tmp_strtok=strtok(tmp_strtok, "]");
          tmp_strtok=strtok(NULL, "");
        diag2(DEBUG) printf(" line %d: g op detected detected  ||%s||\n",i+1,tmp_strtok);
        diag3(DEBUG) printf("line %d:_____ ______ ___/fu & (Bcond | Ccond))\n ||%s||\n",i+1,tmp_strtok);
      if ('/'==tmp_strtok[0])
       {
       tmp_strtok++;
       diag3(DEBUG) printf("line %d:_____ ______ ___fu & (Bcond | Ccond))\n ||%s||\n",i+1,tmp_strtok);
       }else 
          {
           printf("\nERROR\nERROR : line %d: '/' expected SKIPING LINE\n",i+1);
           error_detected++;
           continue;
          }
        
     }
      if((NULL!=tmp_strtok)&&(tmp_strtok[0]=='f')) 
     {
         if ((FINISH_IT!=pTable[i].B)&&(FINISH_WINDOW!=pTable[i].B)&&(FINISH_ALL_IT_BEFORE!=pTable[i].B))
        {
          pTable[i].B=FINISH_U;
        }
        diag2(DEBUG) printf(" line %d: finish token detected\n",i+1);
        tmp_strtok++;
        
        if  (REGULAR==Btype)
         { 
         //stage 5.1  ret_val token------------------------------ ------------------------------------
          diag3(DEBUG) printf(" line %d: regular task checking ret val condition\n",i+1);
          
          if((tmp_strtok[0]=='1')&&(FINISH_U==pTable[i].B)) pTable[i].B=FINISH_1;
          else if((tmp_strtok[0]=='0')&&(FINISH_U==pTable[i].B)) pTable[i].B=FINISH_0;
          else if((tmp_strtok[0]=='u')&&(FINISH_U==pTable[i].B)) pTable[i].B=FINISH_U;
           else { printf("\nERROR\nERROR : line %d: ret_val token expected before %c \n",i+1,tmp_strtok[0]); }
          diag2(DEBUG) printf(" line %d: ret_val token ||%c||\n",i+1,tmp_strtok[0]);
          tmp_strtok++;
         
         }       
          else    
             {
                diag3(DEBUG) printf(" line %d: task is not regular no retval token needed\n",i+1);
             }
      
     }else
     if((NULL!=tmp_strtok)&&(tmp_strtok[0]=='s')) 
     {
        diag2(DEBUG) printf(" line %d: start token detected ||%c||\n",i+1,tmp_strtok[0]);
            if(FINISH_IT==pTable[i].B)              pTable[i].B=START_IT;
       else if(FINISH_WINDOW==pTable[i].B)          pTable[i].B=START_WINDOW;
       else if(FINISH_ALL_IT_BEFORE==pTable[i].B)   pTable[i].B=START_ALL_IT_BEFORE;
       else if(FINISH_GROUP==pTable[i].B)           pTable[i].B=START_GROUP;
       else                                          pTable[i].B=START;
        tmp_strtok++;    
     }else  
     {
      printf("\nERROR\nERROR : line %d cond B: '[num]' , '[<num]' '[@num]'  '/f' or '/s'  expected SKIPPING LINE\n",i+1);
      error_detected++;
       continue;
      }
      
         tmp_strtok=strtok(tmp_strtok, " ");
      if((NULL!=tmp_strtok)&&(tmp_strtok[0]==')'))
      {
          diag1(DEBUG) printf(" line %d: condition compleated  f(A op B) type\n",i+1);
          if (swapAB==1)//'|' op
          {
              pTable[i].C=pTable[i].B;
              pTable[i].C_ID=pTable[i].B_ID;
              pTable[i].C_IT=pTable[i].C_IT;
              
              pTable[i].B=pTable[i].A;
              pTable[i].B_ID=pTable[i].A_ID;
              pTable[i].B_IT=pTable[i].A_IT;
              pTable[i].A=ONE;
              pTable[i].A_ID=-1;
              pTable[i].A_IT=0;
          }else
          {
             pTable[i].C=ZERO;
              pTable[i].C_ID=-1;
          }
          continue;
      } else 
      if((NULL!=tmp_strtok)&&(tmp_strtok[0]=='|'))  
     {
     if (0==open_B)
     {
      printf("  ERROR line %d: '|' operand detected must have '(' after & !!!   SKIPPING LINE\n",i+1);
      error_detected++;
      continue;
     }
         diag2(DEBUG) printf("  line %d: '|' operand detected \n",i+1);    
     }else
      {
        printf("\nERROR\nERROR : line %d:  f(A & ( B ____ only '|' OP is allowed at this point ||%s|| SKIPING LINE\n",i+1,tmp_strtok);
        error_detected++;
           continue;
      }       
     
       
         //cond C handle 
        tmp_strtok=strtok(NULL, "");
        diag3(DEBUG) printf("  line %d:3rd operand handle ||%s|| \n",i+1,tmp_strtok);
     
      tmp_strtok=strtok(tmp_strtok, "/[( ");//stage 3
       diag3(DEBUG) printf("line %d: /[(  ||%s||\n",i+1,tmp_strtok);
       
     if(NULL!=tmp_strtok) 
     {
       diag3(DEBUG) printf(" line %d: C condition task name is ||%s||\n",i+1,tmp_strtok);
       int  match=funcToId(tmp_strtok,pTable);
     
       if (-1==match)
       {
           printf(" ERROR: line %d:  no task named ||%s||  SKIPING LINE\n",i+1,tmp_strtok);
           error_detected++;
           continue;
       }
     
       diag2(DEBUG) printf(" line %d: C condition task %s type  is %d\n",i+1,tmp_strtok,Atype);
       pTable[i].C_ID=match;
       Ctype=pTable[match].type;
     }
     else
        {
           printf(" ERROR: line %d: expecting task name for C condition SKIPPING LINE\n",i+1);
           error_detected++;
           continue;
        }
       //stage 4.1   [] test---------------------------------------------------------
     tmp_strtok=strtok(NULL, "");
      diag3(DEBUG) printf("line %d:_____ ______ ______ _____XX]/fu)\n ||%s||\n",i+1,tmp_strtok);
     if((NULL!=tmp_strtok)&&(tmp_strtok[0]=='<'))
     {
        pTable[i].C=FINISH_ALL_IT_BEFORE;
        diag2(DEBUG) printf(" line %d: < op detected detected  ||%s||\n",i+1,tmp_strtok);
        tmp_strtok++;
       
        tmp_strtok=strtok(tmp_strtok, "]");
        diag2(DEBUG) printf(" line %d: < ||%s|| detected ']'  \n",i+1,tmp_strtok);
        pTable[i].C_IT=atoi(tmp_strtok);
        
        tmp_strtok=strtok(NULL, "");
        diag3(DEBUG) printf("line %d:_____ ______ ______ _____/fu)\n ||%s||\n",i+1,tmp_strtok);
        if ('/'==tmp_strtok[0])
       {
       tmp_strtok++;
        diag3(DEBUG) printf("line %d:_____ ______ ___(_____fu)\n ||%s||\n",i+1,tmp_strtok);
       } else 
          {
           printf("\nERROR\nERROR : line %d: '/' expected SKIPING LINE\n",i+1);
           error_detected++;
           continue;
          }
         
     }
     else if((NULL!=tmp_strtok)&&(tmp_strtok[0]=='@'))
     {
        pTable[i].C=FINISH_WINDOW;
        diag2(DEBUG) printf(" line %d: @ op detected detected  ||%s||\n",i+1,tmp_strtok);
        tmp_strtok++;
         
         
        tmp_strtok=strtok(tmp_strtok, "]");
        diag2(DEBUG) printf(" line %d: @ ||%s|| detected  ']' \n",i+1,tmp_strtok);
       pTable[i].C_IT=atoi(tmp_strtok);
       
        tmp_strtok=strtok(NULL, "");
         diag3(DEBUG) printf("line %d:_____ ______ ___(_____/fu)\n ||%s||\n",i+1,tmp_strtok);
      if ('/'==tmp_strtok[0])
       {
       tmp_strtok++;
        diag3(DEBUG) printf("line %d:_____ ______ ___(_____fu)\n ||%s||\n",i+1,tmp_strtok);
       } else 
          {
           printf("\nERROR\nERROR : line %d: '/' expected SKIPING LINE\n",i+1);
           error_detected++;
           continue;
          }
        
     }
            else if((NULL!=tmp_strtok)&&(tmp_strtok[0]=='{'))
     {
        pTable[i].C=FINISH_GROUP;//default is finish might be changed when return token checked 
        tmp_strtok++;
        diag2(DEBUG) printf(" line %d: { op detected detected  ||%s||\n",i+1,tmp_strtok);
      
        tmp_strtok=strtok(tmp_strtok, ",");
        diag2(DEBUG) printf(" line %d: { ||%s|| detected   \n",i+1,tmp_strtok);
          printf("line %d:Uriel add K=%d to table ! as group implementation \n",i+1,atoi(tmp_strtok));
          //pTable[i].A_IT=atoi(tmp_strtok); {k,_}
        tmp_strtok=strtok(NULL, "}");
        diag2(DEBUG) printf(" line %d: {k,||%s|| detected   \n",i+1,tmp_strtok);
          printf("line %d: Uriel add m=%d to table ! as group implementation \n",i+1,atoi(tmp_strtok));
          //pTable[i].A_IT=atoi(tmp_strtok); {_,m}
        tmp_strtok=strtok(NULL, "]");
        diag3(DEBUG) printf("line %d:_____ ______ ___/fu & (Bcond | Ccond))\n ||%s||\n",i+1,tmp_strtok);
      if ('/'==tmp_strtok[0])
       {
       tmp_strtok++;
       diag3(DEBUG) printf("line %d:_____ ______ ___fu & (Bcond | Ccond))\n ||%s||\n",i+1,tmp_strtok);
       }else 
          {
           printf("\nERROR\nERROR : line %d: '/' expected SKIPING LINE\n",i+1);
           error_detected++;
           continue;
          }
        
     }
     else if((NULL!=tmp_strtok)&&(tmp_strtok[0]>='0')&&(tmp_strtok[0]<='9'))
     {
        pTable[i].C=FINISH_IT;
        pTable[i].C_IT=atoi(tmp_strtok);
        
        tmp_strtok=strtok(tmp_strtok, "]");
        diag2(DEBUG) printf(" line %d: number  ||%s|| ']'\n",i+1,tmp_strtok);
        tmp_strtok=strtok(NULL, "");
        diag3(DEBUG) printf("line %d:_____ ______ ___(_____/fu)\n ||%s||\n",i+1,tmp_strtok);
      if ('/'==tmp_strtok[0])
       {
       tmp_strtok++;
        diag3(DEBUG) printf("line %d:_____ ______ ___(_____fu)\n ||%s||\n",i+1,tmp_strtok);
       } else 
          {
           printf("\nERROR\nERROR : line %d: '/' expected SKIPING LINE\n",i+1);
           error_detected++;
           continue;
          }
       
     }
     
      else if((NULL!=tmp_strtok)&&(tmp_strtok[0]=='g'))
     {
        pTable[i].C=FINISH_GROUP;//default is finish might be changed when return token checked 
          tmp_strtok=strtok(tmp_strtok, "]");
          tmp_strtok=strtok(NULL, "");
        diag2(DEBUG) printf(" line %d: g op detected detected  ||%s||\n",i+1,tmp_strtok);
        diag3(DEBUG) printf("line %d:_____ ______ ___/fu & (Bcond | Ccond))\n ||%s||\n",i+1,tmp_strtok);
      if ('/'==tmp_strtok[0])
       {
       tmp_strtok++;
       diag3(DEBUG) printf("line %d:_____ ______ ___fu & (Bcond | Ccond))\n ||%s||\n",i+1,tmp_strtok);
       }else 
          {
           printf("\nERROR\nERROR : line %d: '/' expected SKIPING LINE\n",i+1);
           error_detected++;
           continue;
          }
        
     }     
  //-------------------------------------------------------------------
     if((NULL!=tmp_strtok)&&(tmp_strtok[0]=='f')) 
     {
        if ((FINISH_IT!=pTable[i].C)&&(FINISH_WINDOW!=pTable[i].C)&&(FINISH_ALL_IT_BEFORE!=pTable[i].C))
        {
          pTable[i].C=FINISH_U;
        }
        diag2(DEBUG) printf(" line %d: finish token detected\n",i+1);
        tmp_strtok++;
        
        if  (REGULAR==Ctype)
         { 
           //stage 5.1  ret_val token------------------------------ ------------------------------------
          diag3(DEBUG) printf(" line %d: regular task checking ret val condition\n",i+1);
          
          if((tmp_strtok[0]=='1')&&(FINISH_U==pTable[i].C)) pTable[i].C=FINISH_1;
          else if((tmp_strtok[0]=='0')&&(FINISH_U==pTable[i].C)) pTable[i].C=FINISH_0;
          else if((tmp_strtok[0]=='u')&&(FINISH_U==pTable[i].C)) pTable[i].C=FINISH_U;
           else { printf("\nERROR\nERROR : line %d: ret_val token expected before %c \n",i+1,tmp_strtok[0]); }
          diag2(DEBUG) printf(" line %d: ret_val token ||%c||\n",i+1,tmp_strtok[0]);
          tmp_strtok++;
         }       
          else    
             {
                diag3(DEBUG) printf(" line %d: task is not regular no retval token needed\n",i+1);
             }
      
     }else
     if((NULL!=tmp_strtok)&&(tmp_strtok[0]=='s')) 
     {
        diag2(DEBUG) printf(" line %d: start token detected ||%c||\n",i+1,tmp_strtok[0]);
            if(FINISH_IT==pTable[i].C)              pTable[i].C=START_IT;
       else if(FINISH_WINDOW==pTable[i].C)          pTable[i].C=START_WINDOW;
       else if(FINISH_ALL_IT_BEFORE==pTable[i].C)   pTable[i].C=START_ALL_IT_BEFORE;
       else if(FINISH_GROUP==pTable[i].C)           pTable[i].C=START_GROUP;
       else                                          pTable[i].C=START;
        tmp_strtok++;    
     }else  
     {
      printf("\nERROR\nERROR : line %d cond C: '[num]' , '[<num]' '[@num]'  '/f' or '/s'  expected SKIPPING LINE\n",i+1);
      error_detected++;
       continue;
      }
  //-------------------------------------------------------------------   
      if(tmp_strtok[0]==')')
            {
             diag3(DEBUG) printf(" line %d: ) CLOSE  \n",i+1);
               tmp_strtok++;
            }
      if(tmp_strtok[0]==')')
            {
             diag3(DEBUG) printf(" line %d: ) CLOSE \n",i+1);
              diag1(DEBUG) printf(" line %d: done type f(A&(B|C))\n",i+1);
             continue;
            }else    
            {
              printf("\nERROR\nERROR : line %d: expecting ')' ||%s|| SKIPPING LINE\n",i+1,tmp_strtok);
              error_detected++;
              continue;
            }
  }
	fclose(fp);
 if (error_detected)   printf("----------------finished task map condition handle  TOTAL ERRORS=%d ---------------------\n",error_detected);
 else 
 {
   diag2(DEBUG)  printf("----------------finished task map condition handle  ----------------------------\n");
   for (i=0;i<table_size;i++)
    diag2(DEBUG)		printf(" line %d is : %d %d %d %d %d %d \n",i, pTable[i].A_ID, pTable[i].A, pTable[i].B_ID, pTable[i].B, pTable[i].C_ID, pTable[i].C);
 }
	return error_detected;
}	

    
	

int reflection(TABLE_LINE* pTable,int DEBUG)
{
int error_detected=0;
 //------------------reflection ------------------------------------------------
   FILE *lut=fopen(lut_name, "r");
   char lutline[100];
   char* tmp_strtok=NULL;
	//initialize static table
 int i=0,j=0,match=0;
 
 if (NULL==lut)
 {
    diag2(DEBUG)   printf(" cant open file ||%s|| \n",lut_name);
    return -1;
 }
 diag2(DEBUG)   printf(" get functions addresses \n");
 diag1(DEBUG)   printf(" table_size = %d \n",table_size);
	for(i = 0; i < table_size; i++)
	{	
    
    fgets(lutline, 100, lut);
    tmp_strtok=strtok(lutline, " \n");
    diag1(DEBUG) printf("line %d: task  %s",i, tmp_strtok);
    match=0;
    for(j = 0; j < table_size; j++)
    {
      
      if(!strcmp(pTable[j].func,tmp_strtok))
       {
       match=1;
       if(pTable[j].type == DUMMY) 
       {
       diag1(DEBUG)   printf(" type is DUMMY: no address needed\n" );
       
         break;
       }
       tmp_strtok=strtok(NULL, " \n");
        pTable[j].funcAddress = (int)strtol(tmp_strtok, NULL, 16);   
        diag1(DEBUG)   printf(" address is %d\n",pTable[j].funcAddress );
        break;
       }                             
    }
    if(0==match) 
      {
        printf("error : line %d: func name is: ||%s|| no matching symbol found\n",i, pTable[i].func );
        error_detected++;
      }
  }
 
 fclose(lut);
 diag2(DEBUG)	printf("created table \n");
return error_detected;
}
             



     
   
