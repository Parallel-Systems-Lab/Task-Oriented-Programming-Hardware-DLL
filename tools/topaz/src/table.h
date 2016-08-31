#ifndef _STATIC_TABLE_H
#define _STATIC_TABLE_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#define max_func_name_langth 20
int table_size;

            //  0   1      2      3     4         5                    6          7           8        9                    
typedef enum {ZERO, ONE, INIT, START,START_IT,START_ALL_IT_BEFORE,START_WINDOW,START_GROUP,FINISH_0, FINISH_1, 
            //  10        11         12                    13            14         
              FINISH_U,FINISH_IT, FINISH_ALL_IT_BEFORE,FINISH_WINDOW,FINISH_GROUP} retVal;//GROUP NOT IMPLEMENTED YET
              // 0         1         2        3
typedef enum {REGULAR, DUPLICABLE, DUMMY, HARDWARE} taskType;

typedef struct _TABLE_LINE
{
	int A_ID, B_ID, C_ID;
	retVal A, B, C;    //assuming function is A&(B|C)
  int A_ID_iteration, B_ID_iteration, C_ID_iteration;
	retVal exec_state; //defines current execution state of program
	taskType type;
	char func[max_func_name_langth];
  int funcAddress;
	int dupIterations;
	int lastIterationStarted;
	int counter;
 int A_IT, B_IT, C_IT;
 int A_FLAG,B_FLAG,C_FLAG;
 int lastEnded;
} TABLE_LINE;

int getTableSize(int DEBUG);
int funcToId(char* func, TABLE_LINE* pTable);
int CreateTable_stg1(TABLE_LINE* pTable,int DEBUG);
int CreateTable_stg2(TABLE_LINE* pTable,int DEBUG);
int reflection(TABLE_LINE* pTable,int DEBUG);




#endif
