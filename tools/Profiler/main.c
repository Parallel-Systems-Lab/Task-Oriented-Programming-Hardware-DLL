/*
 * test.c
 *
 *  Created on: 24 בנוב 2015
 *      Author: Alon
 */
 

#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>


#include <time.h>
#include <stdlib.h>


#define NUM_OF_CORES 16

#define NUM_OF_PARTITIONS 16

int movingDelay=0;
bool displayDll=true;
double t=0;
double t2=0;
int totalEndTime=200000000;
int numTask=0;
double avgUse=0;
double avgUse2=0;
int window_1, window_C;
int xOrigin = -1,yOrigin = -1;
double startPoint=0,xStartPointTree=0,yStartPointTree=0;
int subWindow1_1,subWindow1_2,subWindow1_3,subWindow1_4,subWindow1_5,subWindow1_6;
int subWindowC_1,subWindowC_2;
int subWindow2_1,subWindow2_2,subWindow2_3,subWindow2_4,subWindow2_5;
bool buttons[7];
bool hidebuttons[11];
bool windowState[11];
bool resizeUP=false,resizeDOWN=false,resizeLEFT=false,resizeRIGHT=false,moveWindow=false;
int playOrStop=1;
double speed=10;
double scale=2000;
double dllTimeLapse=0;

double windowW=1290,windowH=600;


double subWindow1_1x=350,subWindow1_1y=0,subWindow1_1w=100,subWindow1_1h=300;//task list
double subWindow1_2x=0,subWindow1_2y=300,subWindow1_2w=595,subWindow1_2h=150;//per cores graph
double subWindow1_3x=100,subWindow1_3y=0,subWindow1_3w=250,subWindow1_3h=300;//cores
double subWindow1_4x=0,subWindow1_4y=0,subWindow1_4w=100,subWindow1_4h=300;//avguse
double subWindow1_5x=0,subWindow1_5y=450,subWindow1_5w=595,subWindow1_5h=150;//all cores graph
double subWindow1_6x=450,subWindow1_6y=0,subWindow1_6w=490,subWindow1_6h=300;//task tree
double subWindow2_1x=1190,subWindow2_1y=0,subWindow2_1w=100,subWindow2_1h=300;//dlls list
double subWindow2_2x=940,subWindow2_2y=0,subWindow2_2w=250,subWindow2_2h=300;//partitions
double subWindow2_3x=595,subWindow2_3y=300,subWindow2_3w=595,subWindow2_3h=150;//partitions graph
double subWindow2_4x=595,subWindow2_4y=450,subWindow2_4w=595,subWindow2_4h=150;//partitions graph
double subWindow2_5x=1190,subWindow2_5y=300,subWindow2_5w=100,subWindow2_5h=300;//partitions avg







struct dependency_list{
	struct task_t *first;
	char* firstType;
	int firstAndsec;
	struct task_t *sec;
	char* secType;
	struct task_t *third;
	char* thirdType;
};

typedef struct dependency_list *DependencyList;

struct duplicable_task{
	int startTime;
	int endTime;
	int activeCopies;
	int finishedCopies;
	int allCopies;
	struct duplicable_task *nextIteration;
};

typedef struct duplicable_task *DuplicableTask;

struct task_runs{
	int startTime;
	int endTime;
	int retVal;
	struct task_runs *next;
};

typedef struct task_runs *TaskRuns;


struct task_t {
	char* name;
	int type;
	float red;
	float green;
	float blue;
	struct task_runs* runs;
	struct task_main_tree *treePos;
	struct duplicable_task *iterations;
	struct dependency_list *dep;
	struct task_t *next;
};

typedef struct task_t *TaskList;



struct core_task_list {
	struct task_t *task;
	int itr;
	int startTime;
	int endTime;
	struct core_task_list *next;
};

typedef struct core_task_list *CoreTask;

struct core_t {
	int coreNum;
	int taskTotalNum;
	CoreTask CoreTaskList;
};

typedef struct core_t *Core;

struct time_line { 
	int coresActive;
	int startTime;
	int endTime;
	struct time_line *next;
};

typedef struct time_line *TimeLine;

struct coresEn_time_line { 
	int numCoresEn;
	int startTime;
	int endTime;
	struct coresEn_time_line *next;
};

typedef struct coresEn_time_line *CoresEnTimeLine;



struct task_main_tree {
	struct task_t *task;
	double xPos;
	double yPos;
	struct task_main_tree *next;
	struct task_main_tree *nextLevel;
};

typedef struct task_main_tree *TaskMainTree;
//////////////////////////////////////////////////////////////////
struct dll_timeline {
	int numOfPar;
	double startTime;
	double endTime;
	struct dll_timeline *next;
};
typedef struct dll_timeline *DLLTimeline;

struct dll_task_list {
	char* name;
	double startTime;
	double endTime;
	struct dll_task_list *next;
};
typedef struct dll_task_list *DLLTask;

struct dll_task_t {
	char* name;
	float red;
	float green;
	float blue;
	struct dll_task_t *next;
};

typedef struct dll_task_t *DLLTaskList;

///////////////////////////////////////////////////////////////////
TaskMainTree rootTaskTree;

TaskList tasks;

DLLTaskList dllTasks;

Core cores[NUM_OF_CORES];
DLLTask dlls[NUM_OF_PARTITIONS];
TimeLine timeLine;
DLLTimeline dllTimeline;

CoresEnTimeLine coresEn; 

double max (double a,double b){
if (a>b)
	return a;
return b;
}
void scene(){
	t+=speed*playOrStop;
	t2+=0.0001*speed*playOrStop;
	if(displayDll){
	if(t>totalEndTime+1000 &&t2>dllTimeLapse+0.5*dllTimeLapse){
		t=0;
		startPoint=0;
		t2=0;
	}
	}else{
	if(t>totalEndTime+1000){
		t=0;
		startPoint=0;
		t2=0;
	}
	}
}





/*----------------------------------------------------------------------------------------
 *	\brief	This function draws a text string to the screen using glut bitmap fonts.
 *	\param	font	-	the font to use. it can be one of the following : 
 *
 *					GLUT_BITMAP_9_BY_15		
 *					GLUT_BITMAP_8_BY_13			
 *					GLUT_BITMAP_TIMES_ROMAN_10	
 *					GLUT_BITMAP_TIMES_ROMAN_24	
 *					GLUT_BITMAP_HELVETICA_10	
 *					GLUT_BITMAP_HELVETICA_12	
 *					GLUT_BITMAP_HELVETICA_18	
 *
 *	\param	text	-	the text string to output
 *	\param	x		-	the x co-ordinate
 *	\param	y		-	the y co-ordinate
 */
void Font(void *font,char *text,double x,double y){
	glRasterPos3f(x, y,0);

	while( *text != '\0' )
	{
		glutBitmapCharacter( font, *text );
		++text;
	}
}

bool isTaskOnTree(TaskList task,int currentLevel){
	TaskMainTree indexTaskTree=rootTaskTree;
	TaskMainTree indexLineTaskTree=indexTaskTree;
	for(int i=0;i<currentLevel;i++){
		while(indexTaskTree!=NULL){
			if(strcmp(indexTaskTree->task->name,task->name)==0){
				return true;
			}
			indexTaskTree=indexTaskTree->next;
		}
		indexLineTaskTree=indexLineTaskTree->nextLevel;
		indexTaskTree=indexLineTaskTree;
	}
	return false;
}


bool checkDependencyList(TaskList task,int currentLevel){
	if(task->dep==NULL)
		return true;
	int counter=0;
	TaskMainTree indexTaskTree=rootTaskTree;
	TaskMainTree indexLineTaskTree=indexTaskTree;
	for(int i=0;i<currentLevel;i++){
		while(indexTaskTree!=NULL){
			if(indexTaskTree->task==task->dep->first||indexTaskTree->task==task->dep->sec||indexTaskTree->task==task->dep->third){
				counter++;
			}
			indexTaskTree=indexTaskTree->next;
		}
		indexLineTaskTree=indexLineTaskTree->nextLevel;
		indexTaskTree=indexLineTaskTree;
	}
	if(counter==0&&task->dep->first==NULL)
		return true;
	if(counter==1&&task->dep->sec==NULL)
		return true;
	if(counter==2&&task->dep->third==NULL)
		return true;
	if(counter==3)
		return true;
	return false;
}


void findDependencyList(TaskList task,char* line,int lineIndex){
	int x=0;
	char* temptype=malloc(20*sizeof(char));
	if (line[lineIndex+1]==')'){
		task->dep=NULL;
	}else{
		task->dep=malloc(sizeof(*task->dep));
		task->dep->first=NULL;
		task->dep->firstType=NULL;
		task->dep->firstAndsec=0;
		task->dep->sec=NULL;
		task->dep->secType=NULL;
		task->dep->third=NULL;
		task->dep->thirdType=NULL;
		x=lineIndex+1;
		while(line[x]!=')'&&line[x]!='['&&line[x]!='/'&&line[x]!='&'&&line[x]!='|')
			x++;
		if(line[x-1]==' ')
			x--;
		TaskList tempT=tasks;
		while(strncmp(&line[lineIndex+1],tempT->name,x-lineIndex-1)!=0)
			tempT=tempT->next;
		task->dep->first=tempT;
		lineIndex=x;
		while(line[lineIndex]!=')'&&line[lineIndex]!='&'&&line[lineIndex]!='|'){
			temptype[lineIndex-x]=line[lineIndex];
			lineIndex++;
		}
		temptype[lineIndex-x]='\0';
			task->dep->firstType=malloc(sizeof(char)*(strlen(temptype)+1));
			strcpy(task->dep->firstType,temptype);
			task->dep->firstType[strlen(temptype)]='\0';
		if(line[lineIndex]=='&')
			task->dep->firstAndsec=1;
		else if (line[lineIndex]=='|')
			task->dep->firstAndsec=2;
		if(line[lineIndex]!=')'){
			lineIndex++;
			if(line[lineIndex+1]=='(')
				lineIndex++;
			x=lineIndex;
			while(line[x]!=')'&&line[x]!='['&&line[x]!='/'&&line[x]!='&'&&line[x]!='|')
				x++;
			tempT=tasks;
			while(strncmp(&line[lineIndex+1],tempT->name,x-lineIndex-1)!=0)
				tempT=tempT->next;
			task->dep->sec=tempT;
			lineIndex=x;
			while(line[lineIndex]!=')'&&line[lineIndex]!='&'&&line[lineIndex]!='|'){
				temptype[lineIndex-x]=line[lineIndex];
				lineIndex++;
			}
			temptype[lineIndex-x]='\0';
			task->dep->secType=malloc(sizeof(char)*(strlen(temptype)+1));
			strcpy(task->dep->secType,temptype);
			task->dep->secType[strlen(temptype)]='\0';
		}
		if(line[lineIndex]!=')'){
			lineIndex++;
			if(line[lineIndex+1]=='(')
				lineIndex++;
			x=lineIndex;
			while(line[x]!=')'&&line[x]!='['&&line[x]!='/'&&line[x]!='&'&&line[x]!='|')
				x++;
			tempT=tasks;
			while(strncmp(&line[lineIndex+1],tempT->name,x-lineIndex-1)!=0)
				tempT=tempT->next;
			task->dep->third=tempT;
			lineIndex=x;
			while(line[lineIndex]!=')'&&line[lineIndex]!='&'&&line[lineIndex]!='|'){
				temptype[lineIndex-x]=line[lineIndex];
				lineIndex++;
			}
			temptype[lineIndex-x]='\0';
			task->dep->thirdType=malloc(sizeof(char)*(strlen(temptype)+1));
			strcpy(task->dep->thirdType,temptype);
			task->dep->thirdType[strlen(temptype)]='\0';			
		}
	}
	free(temptype);
}




//------------------------------------------------------------	OnReshape()
//
// This function is called when the window gets resized. It
// allows us to set up the camera projection for the scene.
//
void OnReshape(int w, int h){
	// set the drawable region of the window to the maximum
	// available window size
	glViewport(0,0,w,h);
	
	// set up the projection matrix
	glMatrixMode(GL_PROJECTION);

	// clear any previous transform and set to the identity matrix
	glLoadIdentity();

	// just use an orthographic projection
	glOrtho(-12,12,-12,12,-1,1);

	// go back to modelview matrix so we can move the objects about
	glMatrixMode(GL_MODELVIEW);
	
subWindow1_1x*=(w/windowW);
subWindow1_1y*=(h/windowH);
subWindow1_1w*=(w/windowW);
subWindow1_1h*=(h/windowH);
subWindow1_2x*=(w/windowW);
subWindow1_2y*=(h/windowH);
subWindow1_2w*=(w/windowW);
subWindow1_2h*=(h/windowH);
subWindow1_3x*=(w/windowW);
subWindow1_3y*=(h/windowH);
subWindow1_3w*=(w/windowW);
subWindow1_3h*=(h/windowH);
subWindow1_4x*=(w/windowW);
subWindow1_4y*=(h/windowH);
subWindow1_4w*=(w/windowW);
subWindow1_4h*=(h/windowH);
subWindow1_5x*=(w/windowW);
subWindow1_5y*=(h/windowH);
subWindow1_5w*=(w/windowW);
subWindow1_5h*=(h/windowH);
subWindow1_6x*=(w/windowW);
subWindow1_6y*=(h/windowH);
subWindow1_6w*=(w/windowW);
subWindow1_6h*=(h/windowH);
subWindow2_1x*=(w/windowW);
subWindow2_1y*=(h/windowH);
subWindow2_1w*=(w/windowW);
subWindow2_1h*=(h/windowH);
subWindow2_2x*=(w/windowW);
subWindow2_2y*=(h/windowH);
subWindow2_2w*=(w/windowW);
subWindow2_2h*=(h/windowH);
subWindow2_3x*=(w/windowW);
subWindow2_3y*=(h/windowH);
subWindow2_3w*=(w/windowW);
subWindow2_3h*=(h/windowH);
subWindow2_4x*=(w/windowW);
subWindow2_4y*=(h/windowH);
subWindow2_4w*=(w/windowW);
subWindow2_4h*=(h/windowH);
subWindow2_5x*=(w/windowW);
subWindow2_5y*=(h/windowH);
subWindow2_5w*=(w/windowW);
subWindow2_5h*=(h/windowH);
glutSetWindow(subWindow1_1);
glutPositionWindow(subWindow1_1x,subWindow1_1y);
glutReshapeWindow(subWindow1_1w,subWindow1_1h);
glutSetWindow(subWindow1_2);
glutPositionWindow(subWindow1_2x,subWindow1_2y);
glutReshapeWindow(subWindow1_2w,subWindow1_2h);
glutSetWindow(subWindow1_3);
glutPositionWindow(subWindow1_3x,subWindow1_3y);
glutReshapeWindow(subWindow1_3w,subWindow1_3h);
glutSetWindow(subWindow1_4);
glutPositionWindow(subWindow1_4x,subWindow1_4y);
glutReshapeWindow(subWindow1_4w,subWindow1_4h);
glutSetWindow(subWindow1_5);
glutPositionWindow(subWindow1_5x,subWindow1_5y);
glutReshapeWindow(subWindow1_5w,subWindow1_5h);	
glutSetWindow(subWindow1_6);
glutPositionWindow(subWindow1_6x,subWindow1_6y);
glutReshapeWindow(subWindow1_6w,subWindow1_6h);
glutSetWindow(subWindow2_1);
glutPositionWindow(subWindow2_1x,subWindow2_1y);
glutReshapeWindow(subWindow2_1w,subWindow2_1h);
glutSetWindow(subWindow2_2);
glutPositionWindow(subWindow2_2x,subWindow2_2y);
glutReshapeWindow(subWindow2_2w,subWindow2_2h);
glutSetWindow(subWindow2_3);
glutPositionWindow(subWindow2_3x,subWindow2_3y);
glutReshapeWindow(subWindow2_3w,subWindow2_3h);
glutSetWindow(subWindow2_4);
glutPositionWindow(subWindow2_4x,subWindow2_4y);
glutReshapeWindow(subWindow2_4w,subWindow2_4h);
glutSetWindow(subWindow2_5);
glutPositionWindow(subWindow2_5x,subWindow2_5y);
glutReshapeWindow(subWindow2_5w,subWindow2_5h);

windowW=w;
windowH=h;

	
	
}


//------------------------------------------------------------	OnReshape()
//
// This function is called when the window gets resized. It
// allows us to set up the camera projection for the scene.
//
void OnReshape2(int w, int h){
	// set the drawable region of the window to the maximum
	// available window size
	glViewport(0,0,w,h);

	// set up the projection matrix
	glMatrixMode(GL_PROJECTION);

	// clear any previous transform and set to the identity matrix
	glLoadIdentity();

	// just use an orthographic projection
	glOrtho(-12,12,-12,12,-1,1);

	// go back to modelview matrix so we can move the objects about
	glMatrixMode(GL_MODELVIEW);
}

//------------------------------------------------------------	OnDraw()
//
// glut will call this function whenever the window needs to be
// redrawn.
//

void OnDraw2_1() {
glutSetWindow(subWindow2_1);
	// clear the screen & depth buffer
	glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);

	// clear the previous transform
	glLoadIdentity();
		glColor3f(1, 1, 1);
	Font(GLUT_BITMAP_HELVETICA_18,"dlls list",-10,10);
	glBegin(GL_QUADS);

	int i=1;
	DLLTaskList tempTask=dllTasks;
	while(tempTask!=NULL){
		glColor3f(0.1f, 0.1f, 0.1f);
		glVertex3f(-10.2, 10.5-i,-0.1);
		//glColor3f(0.0f, 0.0f, 0.0f);
		glVertex3f(-10.2, 9.9-i,-0.1);
		//glColor3f(0.0f, 0.0f, 0.0f); 
		glVertex3f(10, 9.9-i,-0.1);
		//glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3f(10, 10.5-i,-0.1);
		tempTask=tempTask->next;
		i++;
	}
	glEnd();
	tempTask=dllTasks;
	i=1;
	while(tempTask!=NULL){
		glColor3f(tempTask->red, tempTask->green, tempTask->blue);
		 Font(GLUT_BITMAP_HELVETICA_10,tempTask->name,-10,10-i);
		 tempTask=tempTask->next;
		 i++;
	}
	
	
		glBegin(GL_LINES);
	glColor3f(1, 1, 1);
	glVertex3f(11.9, 11.9,-0.1);
	glVertex3f(11.9, -11.9,-0.1);
	glVertex3f(-11.9, 11.9,-0.1);
	glVertex3f(-11.9, -11.9,-0.1);
	glVertex3f(11.9, 11.9,-0.1);
	glVertex3f(-11.9, 11.9,-0.1);
	glVertex3f(11.9, -11.9,-0.1);
	glVertex3f(-11.9, -11.9,-0.1);
	glEnd();
		glutSwapBuffers();
	glutPostRedisplay();
}


void OnDraw2_2() {
glutSetWindow(subWindow2_2);
	// clear the screen & depth buffer
	glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);

	// clear the previous transform
	glLoadIdentity();
	//
	// The simplest way to render geometry within openGL is to use
	// whats known as immediate mode. The drawing takes place between
	// glBegin() and glEnd(). glBegin takes the type of geometry you
	// want to draw, these can be on of the following :
	//
	//  	GL_POINTS
	//  	GL_LINES
	//  	GL_LINE_LOOP
	// 		GL_LINE_STRIP
	//  	GL_TRIANGLES
	//  	GL_TRIANGLE_STRIP
	//  	GL_TRIANGLE_FAN
	//  	GL_QUADS
	//  	GL_QUAD_STRIP
	//  	GL_POLYGON
	//
	// You can place as many calls to glVertex3f inbetween glBegin() and
	// glEnd(). For example, 6 calls to glVertex* between glBegin and glEnd
	// would produce 2 triangles....
	//
	/////////////////////////////////////////////////////////////

	Font(GLUT_BITMAP_HELVETICA_18,"partitions",-10,10);
	glBegin(GL_QUADS);
	
	for(int i=0;i<NUM_OF_PARTITIONS;i++){
		glColor3f(1,1,1);
		DLLTask dllTaskList=dlls[i];
		while(dllTaskList!=NULL){
		DLLTaskList tempTask=dllTasks;
			if(dllTaskList->startTime>-1){
				if(dllTaskList->startTime<t2&&dllTaskList->endTime>t2){
				while(strcmp(tempTask->name,dllTaskList->name)!=0)
					tempTask=tempTask->next;
				glColor3f(tempTask->red, tempTask->green, tempTask->blue);
				}
			}
			dllTaskList=dllTaskList->next;
			}
		glVertex3f(-10+4.2*(i%4)+3.7,5-(i/4)*5.2,-0.1);

		//glColor3f(0,1,0);
		glVertex3f(-10+4.2*(i%4),5-(i/4)*5.2,-0.1);

		//glColor3f(0,0,1);
		glVertex3f(-10+4.2*(i%4),5-(i/4)*5.2+4.7,-0.1);

		glVertex3f(-10+4.2*(i%4)+3.7,5-(i/4)*5.2+4.7,-0.1);
	}
	glEnd();
	for(int i=0;i<NUM_OF_PARTITIONS;i++){
		glColor3f(1,1,1);
		DLLTask dllTaskList=dlls[i];
		while(dllTaskList!=NULL){
			if(dllTaskList->startTime>-1){
				if(dllTaskList->startTime<t2&&dllTaskList->endTime>t2){
						Font(GLUT_BITMAP_HELVETICA_10,dllTaskList->name,-10+4.2*(i%4)+0.2,5-(i/4)*5.2+2.35);
				}
			}
			dllTaskList=dllTaskList->next;
			}
			
	}
	
	////////////////////////////////////////////////////////////////////////////////////
	glBegin(GL_LINES);
	glColor3f(1, 1, 1);
	glVertex3f(11.9, 11.9,-0.1);
	glVertex3f(11.9, -11.9,-0.1);
	glVertex3f(-11.9, 11.9,-0.1);
	glVertex3f(-11.9, -11.9,-0.1);
	glVertex3f(11.9, 11.9,-0.1);
	glVertex3f(-11.9, 11.9,-0.1);
	glVertex3f(11.9, -11.9,-0.1);
	glVertex3f(-11.9, -11.9,-0.1);
	glEnd();
	// currently we've been drawing to the back buffer, we need
	// to swap the back buffer with the front one to make the image visible
	glutSwapBuffers();
	glutPostRedisplay();
}



void OnDraw2_3() {
glutSetWindow(subWindow2_3);
	// clear the screen & depth buffer
	glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);

	// clear the previous transform
	glLoadIdentity();
	//
	// The simplest way to render geometry within openGL is to use
	// whats known as immediate mode. The drawing takes place between
	// glBegin() and glEnd(). glBegin takes the type of geometry you
	// want to draw, these can be on of the following :
	//
	//  	GL_POINTS
	//  	GL_LINES
	//  	GL_LINE_LOOP
	// 		GL_LINE_STRIP
	//  	GL_TRIANGLES
	//  	GL_TRIANGLE_STRIP
	//  	GL_TRIANGLE_FAN
	//  	GL_QUADS
	//  	GL_QUAD_STRIP
	//  	GL_POLYGON
	//
	// You can place as many calls to glVertex3f inbetween glBegin() and
	// glEnd(). For example, 6 calls to glVertex* between glBegin and glEnd
	// would produce 2 triangles....
	//
	/////////////////////////////////////////////////////////////

	Font(GLUT_BITMAP_HELVETICA_18,"per partion graph",-10,10);
	glBegin(GL_QUADS);

	for(int i=0;i<NUM_OF_PARTITIONS;i++){
		DLLTask dllTaskList=dlls[i];
		while(dllTaskList!=NULL){
		DLLTaskList tempTask=dllTasks;
			if(dllTaskList->startTime>-1){
			if(dllTaskList->startTime<t2&&dllTaskList->endTime<t2){
				while(strcmp(tempTask->name,dllTaskList->name)!=0)
					tempTask=tempTask->next;
				glColor3f(tempTask->red, tempTask->green, tempTask->blue);
				glVertex3f((double)startPoint-10.2+(double)dllTaskList->startTime*20000/scale, -10.5+1.2*i,-0.1);
				glVertex3f((double)startPoint-10.2+(double)dllTaskList->startTime*20000/scale,  -9.5+1.2*i,-0.1);
				glVertex3f((double)startPoint-10.2+(double)dllTaskList->endTime*20000/scale,  -9.5+1.2*i,-0.1);
				glVertex3f((double)startPoint-10.2+(double)dllTaskList->endTime*20000/scale,-10.5+1.2*i,-0.1);
			}else if (dllTaskList->startTime<t2){
				while(strcmp(tempTask->name,dllTaskList->name)!=0)
					tempTask=tempTask->next;
				glColor3f(tempTask->red, tempTask->green, tempTask->blue);
				glVertex3f((double)startPoint-10.2+(double)dllTaskList->startTime*20000/scale,-10.5+1.2*i,-0.1);
				glVertex3f((double)startPoint-10.2+(double)dllTaskList->startTime*20000/scale,-9.5+1.2*i,-0.1);
				glVertex3f((double)startPoint-10.2+(double)t2*20000/scale,-9.5+1.2*i,-0.1);
				glVertex3f((double)startPoint-10.2+(double)t2*20000/scale,-10.5+1.2*i,-0.1);
			}
		}
		dllTaskList=dllTaskList->next;
		}
		}
	glEnd();
	////////////////////////////////////////////////////////////////////////////////////
	glBegin(GL_LINES);
	glColor3f(1, 1, 1);
	glVertex3f(11.9, 11.9,-0.1);
	glVertex3f(11.9, -11.9,-0.1);
	glVertex3f(-11.9, 11.9,-0.1);
	glVertex3f(-11.9, -11.9,-0.1);
	glVertex3f(11.9, 11.9,-0.1);
	glVertex3f(-11.9, 11.9,-0.1);
	glVertex3f(11.9, -11.9,-0.1);
	glVertex3f(-11.9, -11.9,-0.1);
	glEnd();
	// currently we've been drawing to the back buffer, we need
	// to swap the back buffer with the front one to make the image visible
	glutSwapBuffers();
	glutPostRedisplay();
}

void OnDraw2_4() {
glutSetWindow(subWindow2_4);
	// clear the screen & depth buffer
	glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);

	// clear the previous transform
	glLoadIdentity();
	//
	// The simplest way to render geometry within openGL is to use
	// whats known as immediate mode. The drawing takes place between
	// glBegin() and glEnd(). glBegin takes the type of geometry you
	// want to draw, these can be on of the following :
	//
	//  	GL_POINTS
	//  	GL_LINES
	//  	GL_LINE_LOOP
	// 		GL_LINE_STRIP
	//  	GL_TRIANGLES
	//  	GL_TRIANGLE_STRIP
	//  	GL_TRIANGLE_FAN
	//  	GL_QUADS
	//  	GL_QUAD_STRIP
	//  	GL_POLYGON
	//
	// You can place as many calls to glVertex3f inbetween glBegin() and
	// glEnd(). For example, 6 calls to glVertex* between glBegin and glEnd
	// would produce 2 triangles....
	//
	/////////////////////////////////////////////////////////////
Font(GLUT_BITMAP_HELVETICA_18,"uti partion graph",-10,10);
	glBegin(GL_QUADS);	
	DLLTimeline tempTimeLine=dllTimeline;
	while(tempTimeLine!=NULL){
		if(tempTimeLine->startTime<t2&&tempTimeLine->endTime<t2){
			glColor3f(0.5, 0.5, 0.5);
			glVertex3f((double)startPoint-10.2+(double)tempTimeLine->startTime*20000/scale, -10.5,0);
			glVertex3f((double)startPoint-10.2+(double)tempTimeLine->startTime*20000/scale, -10.5+0.7*tempTimeLine->numOfPar,0);
			glVertex3f((double)startPoint-10.2+(double)tempTimeLine->endTime*20000/scale, -10.5+0.7*tempTimeLine->numOfPar,0);
			glVertex3f((double)startPoint-10.2+(double)tempTimeLine->endTime*20000/scale, -10.5,0);	
		}else if (tempTimeLine->startTime<t2){
			glColor3f(0.5, 0.5, 0.5);
			glVertex3f((double)startPoint-10.2+(double)tempTimeLine->startTime*20000/scale, -10.5,0);
			glVertex3f((double)startPoint-10.2+(double)tempTimeLine->startTime*20000/scale, -10.5+0.7*tempTimeLine->numOfPar,0);
			glVertex3f((double)startPoint-10.2+(double)t2*20000/scale, -10.5+0.7*tempTimeLine->numOfPar,0);
			glVertex3f((double)startPoint-10.2+(double)t2*20000/scale,-10.5,0);
		}
		tempTimeLine=tempTimeLine->next;
	}
	

	glEnd();
glBegin(GL_LINES);
	glColor3f(1, 1, 1);
	glVertex3f(11.9, 11.9,-0.1);
	glVertex3f(11.9, -11.9,-0.1);
	glVertex3f(-11.9, 11.9,-0.1);
	glVertex3f(-11.9, -11.9,-0.1);
	glVertex3f(11.9, 11.9,-0.1);
	glVertex3f(-11.9, 11.9,-0.1);
	glVertex3f(11.9, -11.9,-0.1);
	glVertex3f(-11.9, -11.9,-0.1);
	glEnd();
	////////////////////////////////////////////////////////////////////////////////////

	// currently we've been drawing to the back buffer, we need
	// to swap the back buffer with the front one to make the image visible
	glutSwapBuffers();
	glutPostRedisplay();
}



void OnDraw2_5(){
	glutSetWindow(subWindow2_5);
	// clear the screen & depth buffer
	glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);

	// clear the previous transform
	glLoadIdentity();
	Font(GLUT_BITMAP_HELVETICA_12,"partion avguse",-10,10);
	glBegin(GL_QUADS);
	DLLTimeline tempTimeLine=dllTimeline;
	while(tempTimeLine!=NULL){
		if(tempTimeLine->startTime<t2&&tempTimeLine->endTime<t2){
			avgUse2+=(double)tempTimeLine->numOfPar*(tempTimeLine->endTime-tempTimeLine->startTime);
		}else if (tempTimeLine->startTime<t2){
			avgUse2+=(double)tempTimeLine->numOfPar*(t2-tempTimeLine->startTime);
		}
		tempTimeLine=tempTimeLine->next;
	}
	if(t2!=0){
		if(t2<dllTimeLapse)
			avgUse2=(double)avgUse2/t2;
		else
			avgUse2=(double)avgUse2/dllTimeLapse;
		glColor3f(0.5, 0.5, 0.5);
		glVertex3f(-9.5, -10,0);
		glVertex3f(-9.5, -10+avgUse2,0);
		glVertex3f(-3, -10+avgUse2,0);
		glVertex3f(-3, -10,0);
		glColor3f(1, 1, 1);
		glVertex3f(-9.5, -10,0);
		glVertex3f(-9.5, -10+NUM_OF_PARTITIONS,0);
		glVertex3f(-3, -10+NUM_OF_PARTITIONS,0);
		glVertex3f(-3, -10,0);
		avgUse2=avgUse2/NUM_OF_PARTITIONS;
		avgUse2=avgUse2*100;
	}
	glEnd();
	glColor3f(1, 1, 1);
	char str[15];
	sprintf(str, "%f", avgUse2);
	strcat(str,"%");
	Font(GLUT_BITMAP_HELVETICA_10,str,-12,8);
	avgUse2=0;
	glBegin(GL_LINES);
	glColor3f(1, 1, 1);
	glVertex3f(11.9, 11.9,-0.1);
	glVertex3f(11.9, -11.9,-0.1);
	glVertex3f(-11.9, 11.9,-0.1);
	glVertex3f(-11.9, -11.9,-0.1);
	glVertex3f(11.9, 11.9,-0.1);
	glVertex3f(-11.9, 11.9,-0.1);
	glVertex3f(11.9, -11.9,-0.1);
	glVertex3f(-11.9, -11.9,-0.1);
	glEnd();
	glutSwapBuffers();
	glutPostRedisplay();
}



void OnDrawC_2() {
	glutSetWindow(subWindowC_2);
	// clear the screen & depth buffer
	glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);

	// clear the previous transform
	glLoadIdentity();
	int k=0;
if(displayDll)
	k=11;
else
	k=6;
	glBegin(GL_QUADS);
		
		for (int i=0;i<k;i++)
		{
		if(windowState[i]==true)
			glColor3f(0,1,0);
		else
			glColor3f(1,0,0);
		glVertex3f(-10+3,10-i*2,-0.1);
		
		glVertex3f(-10,10-i*2,-0.1);

		glVertex3f(-10,10-i*2+1,-0.1);

		glVertex3f(-10+3,10-i*2+1,-0.1);
		}

	glEnd();
	glColor3f(1,1,1);
	Font(GLUT_BITMAP_HELVETICA_10,"task list",-6,10);
	Font(GLUT_BITMAP_HELVETICA_10,"per core graph",-6,8);
	Font(GLUT_BITMAP_HELVETICA_10,"cores",-6,6);
	Font(GLUT_BITMAP_HELVETICA_10,"cores avguse",-6,4);
	Font(GLUT_BITMAP_HELVETICA_10,"uti graph",-6,2);
	Font(GLUT_BITMAP_HELVETICA_10,"task tree",-6,0);
	if(displayDll){
	Font(GLUT_BITMAP_HELVETICA_10,"dlls list",-6,-2);
	Font(GLUT_BITMAP_HELVETICA_10,"partitions",-6,-4);
	Font(GLUT_BITMAP_HELVETICA_10,"per partion graph",-6,-6);
	Font(GLUT_BITMAP_HELVETICA_10,"uti partion graph",-6,-8);
	Font(GLUT_BITMAP_HELVETICA_10,"partion avguse",-6,-10);
	}
	glutSwapBuffers();
	glutPostRedisplay();
}


void OnDrawC_1() {
	glutSetWindow(subWindowC_1);
	// clear the screen & depth buffer
	glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);

	// clear the previous transform
	glLoadIdentity();
scene();
	glBegin(GL_QUADS);
		glColor3f(1,1,1);
		for (int i=0;i<7;i++)
		{
		glVertex3f(-10+4*(i%4)+3,7-(i/4)*5,-0.1);
		
		glVertex3f(-10+4*(i%4),7-(i/4)*5,-0.1);

		glVertex3f(-10+4*(i%4),7-(i/4)*5+4,-0.1);

		glVertex3f(-10+4*(i%4)+3,7-(i/4)*5+4,-0.1);
	}
	glColor3f(1,0,0);
		glVertex3f(-10+2.5,-5+7+1,0);
		glVertex3f(-10+0.5,-5+7+1,0);
		glVertex3f(-10+0.5,-5+7+3,0);
		glVertex3f(-10+2.5,-5+7+3,0);
		
		glVertex3f(4-10+1.4,-5+7+1,0);
		glVertex3f(4-10+0.4,-5+7+1,0);
		glVertex3f(4-10+0.4,-5+7+3,0);
		glVertex3f(4-10+1.4,-5+7+3,0);
		glVertex3f(4-10+2.6,-5+7+1,0);
		glVertex3f(4-10+1.6,-5+7+1,0);
		glVertex3f(4-10+1.6,-5+7+3,0);
		glVertex3f(4-10+2.6,-5+7+3,0);
	
		glVertex3f(-10+2.5,7+1.7,0);
		glVertex3f(-10+0.5,7+1.7,0);
		glVertex3f(-10+0.5,7+2.3,0);
		glVertex3f(-10+2.5,7+2.3,0);
		glVertex3f(-10+1.8,7+1,0);
		glVertex3f(-10+1.2,7+1,0);
		glVertex3f(-10+1.2,7+3,0);
		glVertex3f(-10+1.8,7+3,0);
	
		glVertex3f(4-10+2.5,7+1.7,0);
		glVertex3f(4-10+0.5,7+1.7,0);
		glVertex3f(4-10+0.5,7+2.3,0);
		glVertex3f(4-10+2.5,7+2.3,0);
	
	
	glEnd();
	
	glBegin(GL_TRIANGLES);
		glVertex3f(8-10+2.5,-5+7+2,0);
		glVertex3f(8-10+0.5,-5+7+1,0);
		glVertex3f(8-10+0.5,-5+7+3,0);
	
		glVertex3f(8-10+2,7+2,0);
		glVertex3f(8-10+0.5,7+1,0);
		glVertex3f(8-10+0.5,7+3,0);
		glVertex3f(8-10+2.5,7+2,0);
		glVertex3f(8-10+1.5,7+1,0);
		glVertex3f(8-10+1.5,7+3,0);
		
		glVertex3f(12-10+0.5,7+2,0);
		glVertex3f(12-10+1.8,7+1,0);
		glVertex3f(12-10+1.8,7+3,0);
		glVertex3f(12-10+1.5,7+2,0);
		glVertex3f(12-10+2.5,7+1,0);
		glVertex3f(12-10+2.5,7+3,0);
	glEnd();
	
	
	
	glutSwapBuffers();
	glutPostRedisplay();
}

void OnDrawC() {
glutSetWindow(window_C);
	// clear the screen & depth buffer
	glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);

	// clear the previous transform
	glLoadIdentity();
	// currently we've been drawing to the back buffer, we need
	// to swap the back buffer with the front one to make the image visible
	glutSwapBuffers();
	glutPostRedisplay();
}



void OnDraw1_6() {
glutSetWindow(subWindow1_6);
	// clear the screen & depth buffer
	glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);

	// clear the previous transform
	glLoadIdentity();
Font(GLUT_BITMAP_HELVETICA_18,"task tree",-10,10);
	int i=0,j=0;
	TaskMainTree indexTaskTree=rootTaskTree;
	TaskMainTree indexLineTaskTree=indexTaskTree;
	while(indexLineTaskTree!=NULL){
		i=0;
		while(indexTaskTree!=NULL){
			glColor3f(1, 1, 1);
			if(indexTaskTree->task->type==0)
				glColor3f(0, 0, 0);
			Font(GLUT_BITMAP_HELVETICA_10,indexTaskTree->task->name,(double)(-i*4+(j%2)+0.1+xStartPointTree),-yStartPointTree+11-j*2.5);
			indexTaskTree->xPos=-i*4+(j%2)+xStartPointTree;
			indexTaskTree->yPos=(double)11-j*2.5-yStartPointTree;
			indexTaskTree=indexTaskTree->next;
			i++;
		}
		indexLineTaskTree=indexLineTaskTree->nextLevel;
		indexTaskTree=indexLineTaskTree;
		j++;
	}
	
	
	
	glColor3f(0.5, 0.5, 0.5);
	TaskList tempA=tasks;
	double rightMarrgin=3,upDownMarrgin=0.05;
	for (int i=0;i<numTask;i++){
		if(tempA->dep!=NULL){
			if(tempA->dep->firstAndsec!=2){
			if(tempA->dep->first!=NULL){
				if(tempA->treePos->yPos>tempA->dep->first->treePos->yPos){
				glBegin(GL_LINE_STRIP);
					glVertex3f(tempA->treePos->xPos+0.2,tempA->treePos->yPos+0.5,-0.2);
					glVertex3f(tempA->treePos->xPos+0.2,tempA->treePos->yPos+1.25-upDownMarrgin,-0.2);
					glVertex3f(rightMarrgin,tempA->treePos->yPos+1.25-upDownMarrgin,-0.2);
					glVertex3f(rightMarrgin,tempA->dep->first->treePos->yPos-1.25+upDownMarrgin,-0.2);
					glVertex3f(tempA->dep->first->treePos->xPos+0.2,tempA->dep->first->treePos->yPos-1.25+upDownMarrgin,-0.2);
					glVertex3f(tempA->dep->first->treePos->xPos+0.2,tempA->dep->first->treePos->yPos-0.3,-0.2);
					glEnd();
					rightMarrgin+=0.05;
					upDownMarrgin+=0.05;
				}else{
				glBegin(GL_LINE_STRIP);
					glVertex3f(tempA->treePos->xPos+0.2,tempA->treePos->yPos+0.5,-0.2);
					glVertex3f(tempA->dep->first->treePos->xPos+0.2,tempA->dep->first->treePos->yPos-0.3,-0.2);
					glEnd();
				}
			}
			if(tempA->dep->sec!=NULL){
				if(tempA->treePos->yPos>tempA->dep->sec->treePos->yPos){
				glBegin(GL_LINE_STRIP);
					glVertex3f(tempA->treePos->xPos+0.2,tempA->treePos->yPos+0.5,-0.2);
					glVertex3f(tempA->treePos->xPos+0.2,tempA->treePos->yPos+1.25-upDownMarrgin,-0.2);
					glVertex3f(rightMarrgin,tempA->treePos->yPos+1.25-upDownMarrgin,-0.2);
					glVertex3f(rightMarrgin,tempA->dep->sec->treePos->yPos-1.25+upDownMarrgin,-0.2);
					glVertex3f(tempA->dep->sec->treePos->xPos+0.2,tempA->dep->sec->treePos->yPos-1.25+upDownMarrgin,-0.2);
					glVertex3f(tempA->dep->sec->treePos->xPos+0.2,tempA->dep->sec->treePos->yPos-0.3,-0.2);
					rightMarrgin+=0.05;
					upDownMarrgin+=0.05;
					glEnd();
				}else{
				glBegin(GL_LINE_STRIP);
					glVertex3f(tempA->treePos->xPos+0.2,tempA->treePos->yPos+0.5,-0.2);
					glVertex3f(tempA->dep->sec->treePos->xPos+0.2,tempA->dep->sec->treePos->yPos-0.3,-0.2);
					glEnd();
				}
			}
			}else{
						if(tempA->dep->first!=NULL){
				if(tempA->treePos->yPos>tempA->dep->first->treePos->yPos){
				glBegin(GL_LINE_STRIP);
					glVertex3f(tempA->treePos->xPos+0.2,tempA->treePos->yPos+1.25,-0.2);
					glVertex3f(rightMarrgin+xStartPointTree,tempA->treePos->yPos+1.25,-0.2);
					glVertex3f(rightMarrgin+xStartPointTree,tempA->dep->first->treePos->yPos-1.25+upDownMarrgin,-0.2);
					glVertex3f(tempA->dep->first->treePos->xPos+0.2,tempA->dep->first->treePos->yPos-1.25+upDownMarrgin,-0.2);
					glVertex3f(tempA->dep->first->treePos->xPos+0.2,tempA->dep->first->treePos->yPos-0.3,-0.2);
					glEnd();
					rightMarrgin+=0.05;
					upDownMarrgin+=0.05;
				}else{
				glBegin(GL_LINE_STRIP);
					glVertex3f(tempA->treePos->xPos+0.2,tempA->treePos->yPos+1.25,-0.2);
					glVertex3f(tempA->dep->first->treePos->xPos+0.2,tempA->dep->first->treePos->yPos-0.3,-0.2);
					glEnd();
				}
			}
			if(tempA->dep->sec!=NULL){
				if(tempA->treePos->yPos>tempA->dep->sec->treePos->yPos){
				glBegin(GL_LINE_STRIP);
					glVertex3f(tempA->treePos->xPos+0.2,tempA->treePos->yPos+1.25,-0.2);
					glVertex3f(rightMarrgin+xStartPointTree,tempA->treePos->yPos+1.25,-0.2);
					glVertex3f(rightMarrgin+xStartPointTree,tempA->dep->sec->treePos->yPos-1.25+upDownMarrgin,-0.2);
					glVertex3f(tempA->dep->sec->treePos->xPos+0.2,tempA->dep->sec->treePos->yPos-1.25+upDownMarrgin,-0.2);
					glVertex3f(tempA->dep->sec->treePos->xPos+0.2,tempA->dep->sec->treePos->yPos-0.3,-0.2);
					rightMarrgin+=0.05;
					upDownMarrgin+=0.05;
					glEnd();
				}else{
				glBegin(GL_LINE_STRIP);
					glVertex3f(tempA->treePos->xPos+0.2,tempA->treePos->yPos+1.25,-0.2);
					glVertex3f(tempA->dep->sec->treePos->xPos+0.2,tempA->dep->sec->treePos->yPos-0.3,-0.2);
					glEnd();
				}
			}
			glPointSize(4.0); 
			glBegin ( GL_POINTS );
			glColor3f(1, 0, 0);
			glVertex3f(tempA->treePos->xPos+0.2,tempA->treePos->yPos+1.25,0);
			glEnd();
			glColor3f(0.5, 0.5, 0.5);
			glBegin(GL_LINE_STRIP);
			glVertex3f(tempA->treePos->xPos+0.2,tempA->treePos->yPos+1.25,-0.2);
			glVertex3f(tempA->treePos->xPos+0.2,tempA->treePos->yPos+0.5,-0.2);
			glEnd();
			}
			if(tempA->dep->third!=NULL){
				if(tempA->treePos->yPos>tempA->dep->third->treePos->yPos){
				glBegin(GL_LINE_STRIP);
					glVertex3f(tempA->treePos->xPos+0.2,tempA->treePos->yPos+0.5,-0.2);
					glVertex3f(tempA->treePos->xPos+0.2,tempA->treePos->yPos+1.25-upDownMarrgin,-0.2);
					glVertex3f(rightMarrgin+xStartPointTree,tempA->treePos->yPos+1.25-upDownMarrgin,-0.2);
					glVertex3f(rightMarrgin+xStartPointTree,tempA->dep->third->treePos->yPos-1.25+upDownMarrgin,-0.2);
					glVertex3f(tempA->dep->third->treePos->xPos+0.2,tempA->dep->third->treePos->yPos-1.25+upDownMarrgin,-0.2);
					glVertex3f(tempA->dep->third->treePos->xPos+0.2,tempA->dep->third->treePos->yPos-0.3,-0.2);
					glEnd();
					rightMarrgin+=0.05;
					upDownMarrgin+=0.05;
				}else{
				glBegin(GL_LINE_STRIP);
					glVertex3f(tempA->treePos->xPos+0.2,tempA->treePos->yPos+0.5,-0.2);
					glVertex3f(tempA->dep->third->treePos->xPos+0.2,tempA->dep->third->treePos->yPos-0.3,-0.2);
					glEnd();
				}
			}
		}
		tempA=tempA->next;
	}


	
	glBegin(GL_QUADS);
	indexTaskTree=rootTaskTree;
	indexLineTaskTree=indexTaskTree;
	while(indexLineTaskTree!=NULL){
		while(indexTaskTree!=NULL){
			DuplicableTask tempD=indexTaskTree->task->iterations;
			int activeCopies=0;
			int finishedCopies=0;
			int allCopies=1;
			int dupItrEndTime=0;
			TaskRuns tempR=indexTaskTree->task->runs;
			
			switch(indexTaskTree->task->type){
			case 0://dummy task
			glColor3f(0.5, 0.5, 0.5);
				while(tempR!=NULL){
					if(tempR->startTime<=t&&tempR->endTime>=t)
						glColor3f(indexTaskTree->task->red, indexTaskTree->task->green, indexTaskTree->task->blue);
					else if (tempR->endTime<=t&&tempR->endTime+500>=t)
						glColor3f(indexTaskTree->task->red-0.2, indexTaskTree->task->green-0.2, indexTaskTree->task->blue-0.2);
					tempR=tempR->next;
				}
			glVertex3f(-0.1+indexTaskTree->xPos,indexTaskTree->yPos+0.7,-0.15);
			glVertex3f(-0.1+indexTaskTree->xPos+(double)strlen(indexTaskTree->task->name)*0.25+0.1,indexTaskTree->yPos+0.7,-0.15);
			glVertex3f(-0.1+indexTaskTree->xPos+(double)strlen(indexTaskTree->task->name)*0.25+0.1,indexTaskTree->yPos-0.3,-0.15);
			glVertex3f(-0.1+indexTaskTree->xPos,indexTaskTree->yPos-0.3,-0.15);
			glColor3f(1, 1, 1);
			glVertex3f(indexTaskTree->xPos-0.05,indexTaskTree->yPos+0.6,-0.1);
			glVertex3f(-0.1+indexTaskTree->xPos+(double)strlen(indexTaskTree->task->name)*0.25,indexTaskTree->yPos+0.6,-0.1);
			glVertex3f(-0.1+indexTaskTree->xPos+(double)strlen(indexTaskTree->task->name)*0.25,indexTaskTree->yPos-0.2,-0.1);
			glVertex3f(indexTaskTree->xPos-0.05,indexTaskTree->yPos-0.2,-0.1);
			break;
			case 1://regular task
			glColor3f(0.5, 0.5, 0.5);
				while(tempR!=NULL){
					if(tempR->startTime<=t&&tempR->endTime>=t)
						glColor3f(indexTaskTree->task->red, indexTaskTree->task->green, indexTaskTree->task->blue);
					else if (tempR->endTime<=t&&tempR->endTime+500>=t)
						glColor3f(indexTaskTree->task->red-0.2, indexTaskTree->task->green-0.2, indexTaskTree->task->blue-0.2);
					tempR=tempR->next;
				}
			glVertex3f(-0.1+indexTaskTree->xPos,indexTaskTree->yPos+0.7,-0.1);
			glVertex3f(-0.1+indexTaskTree->xPos+(double)strlen(indexTaskTree->task->name)*0.25+0.1,indexTaskTree->yPos+0.7,-0.1);
			glVertex3f(-0.1+indexTaskTree->xPos+(double)strlen(indexTaskTree->task->name)*0.25+0.1,indexTaskTree->yPos-0.3,-0.1);
			glVertex3f(-0.1+indexTaskTree->xPos,indexTaskTree->yPos-0.3,-0.1);
			break;
			case 2://duplicable task
			while(tempD!=NULL){
				if(tempD->startTime<=t){
					activeCopies=tempD->activeCopies;
					finishedCopies=tempD->finishedCopies;
					allCopies=tempD->allCopies;
					dupItrEndTime=tempD->startTime;
				}
				tempD=tempD->nextIteration;
			}
			if(finishedCopies!=allCopies||dupItrEndTime+500>=t){
			glColor3f(max(indexTaskTree->task->red-0.2,0),max(indexTaskTree->task->green-0.2,0),max(indexTaskTree->task->blue-0.2,0));
			
			glVertex3f(indexTaskTree->xPos,indexTaskTree->yPos+0.6,-0.1);
			glVertex3f(indexTaskTree->xPos+(-0.1+(double)strlen(indexTaskTree->task->name)*0.25)*((double)finishedCopies/allCopies),indexTaskTree->yPos+0.6,-0.1);
			glVertex3f(indexTaskTree->xPos+(-0.1+(double)strlen(indexTaskTree->task->name)*0.25)*((double)finishedCopies/allCopies),indexTaskTree->yPos-0.2,-0.1);
			glVertex3f(indexTaskTree->xPos,indexTaskTree->yPos-0.2,-0.1);
			
			glVertex3f(indexTaskTree->xPos,indexTaskTree->yPos+0.6,-0.1);
			glVertex3f(indexTaskTree->xPos+(-0.1+(double)strlen(indexTaskTree->task->name)*0.25)*((double)finishedCopies/allCopies),indexTaskTree->yPos+0.6,-0.1);
			glVertex3f(0.1+indexTaskTree->xPos+(-0.1+(double)strlen(indexTaskTree->task->name)*0.25)*((double)finishedCopies/allCopies),indexTaskTree->yPos+0.8,-0.1);
			glVertex3f(0.1+indexTaskTree->xPos,indexTaskTree->yPos+0.8,-0.1);
			
			glColor3f(indexTaskTree->task->red, indexTaskTree->task->green, indexTaskTree->task->blue);
			
			glVertex3f(indexTaskTree->xPos+(-0.1+(double)strlen(indexTaskTree->task->name)*0.25)*((double)finishedCopies/allCopies),indexTaskTree->yPos+0.6,-0.1);
			glVertex3f(indexTaskTree->xPos+(-0.1+(double)strlen(indexTaskTree->task->name)*0.25)*((double)(activeCopies+finishedCopies)/allCopies),indexTaskTree->yPos+0.6,-0.1);
			glVertex3f(indexTaskTree->xPos+(-0.1+(double)strlen(indexTaskTree->task->name)*0.25)*((double)(activeCopies+finishedCopies)/allCopies),indexTaskTree->yPos-0.2,-0.1);
			glVertex3f(indexTaskTree->xPos+(-0.1+(double)strlen(indexTaskTree->task->name)*0.25)*((double)finishedCopies/allCopies),indexTaskTree->yPos-0.2,-0.1);
			
			glVertex3f(indexTaskTree->xPos+(-0.1+(double)strlen(indexTaskTree->task->name)*0.25)*((double)finishedCopies/allCopies),indexTaskTree->yPos+0.6,-0.1);
			glVertex3f(indexTaskTree->xPos+(-0.1+(double)strlen(indexTaskTree->task->name)*0.25)*((double)(activeCopies+finishedCopies)/allCopies),indexTaskTree->yPos+0.6,-0.1);
			glVertex3f(0.1+indexTaskTree->xPos+(-0.1+(double)strlen(indexTaskTree->task->name)*0.25)*((double)(activeCopies+finishedCopies)/allCopies),indexTaskTree->yPos+0.8,-0.1);
			glVertex3f(0.1+indexTaskTree->xPos+(-0.1+(double)strlen(indexTaskTree->task->name)*0.25)*((double)finishedCopies/allCopies),indexTaskTree->yPos+0.8,-0.1);
			
			glColor3f(0.5, 0.5, 0.5);
			glVertex3f(indexTaskTree->xPos+(-0.1+(double)strlen(indexTaskTree->task->name)*0.25)*((double)(activeCopies+finishedCopies)/allCopies),indexTaskTree->yPos+0.6,-0.1);
			glVertex3f(-0.1+indexTaskTree->xPos+(double)strlen(indexTaskTree->task->name)*0.25,indexTaskTree->yPos+0.6,-0.1);
			glVertex3f(-0.1+indexTaskTree->xPos+(double)strlen(indexTaskTree->task->name)*0.25,indexTaskTree->yPos-0.2,-0.1);
			glVertex3f(indexTaskTree->xPos+(-0.1+(double)strlen(indexTaskTree->task->name)*0.25)*((double)(activeCopies+finishedCopies)/allCopies),indexTaskTree->yPos-0.2,-0.1);
			
			glVertex3f(indexTaskTree->xPos+(-0.1+(double)strlen(indexTaskTree->task->name)*0.25)*((double)(activeCopies+finishedCopies)/allCopies),indexTaskTree->yPos+0.6,-0.1);
			glVertex3f(indexTaskTree->xPos-0.1+(double)strlen(indexTaskTree->task->name)*0.25,indexTaskTree->yPos+0.6,-0.1);
			glVertex3f(0.1+indexTaskTree->xPos-0.1+(double)strlen(indexTaskTree->task->name)*0.25,indexTaskTree->yPos+0.8,-0.1);
			glVertex3f(0.1+indexTaskTree->xPos+(-0.1+(double)strlen(indexTaskTree->task->name)*0.25)*((double)(activeCopies+finishedCopies)/allCopies),indexTaskTree->yPos+0.8,-0.1);
			
			
			glVertex3f(-0.1+indexTaskTree->xPos+(double)strlen(indexTaskTree->task->name)*0.25,indexTaskTree->yPos+0.6,-0.1);
			glVertex3f(indexTaskTree->xPos+(double)strlen(indexTaskTree->task->name)*0.25,indexTaskTree->yPos+0.8,-0.1);
			glVertex3f(indexTaskTree->xPos+(double)strlen(indexTaskTree->task->name)*0.25,indexTaskTree->yPos-0,-0.1);
			glVertex3f(-0.1+indexTaskTree->xPos+(double)strlen(indexTaskTree->task->name)*0.25,indexTaskTree->yPos-0.2,-0.1);
			}else{
			glColor3f(0.5, 0.5, 0.5);
			glVertex3f(indexTaskTree->xPos,indexTaskTree->yPos+0.6,-0.1);
			glVertex3f(-0.1+indexTaskTree->xPos+(double)strlen(indexTaskTree->task->name)*0.25,indexTaskTree->yPos+0.6,-0.1);
			glVertex3f(-0.1+indexTaskTree->xPos+(double)strlen(indexTaskTree->task->name)*0.25,indexTaskTree->yPos-0.2,-0.1);
			glVertex3f(indexTaskTree->xPos,indexTaskTree->yPos-0.2,-0.1);
			
			glVertex3f(indexTaskTree->xPos,indexTaskTree->yPos+0.6,-0.1);
			glVertex3f(indexTaskTree->xPos-0.1+(double)strlen(indexTaskTree->task->name)*0.25,indexTaskTree->yPos+0.6,-0.1);
			glVertex3f(0.1+indexTaskTree->xPos-0.1+(double)strlen(indexTaskTree->task->name)*0.25,indexTaskTree->yPos+0.8,-0.1);
			glVertex3f(0.1+indexTaskTree->xPos,indexTaskTree->yPos+0.8,-0.1);
			
			
			glVertex3f(-0.1+indexTaskTree->xPos+(double)strlen(indexTaskTree->task->name)*0.25,indexTaskTree->yPos+0.6,-0.1);
			glVertex3f(indexTaskTree->xPos+(double)strlen(indexTaskTree->task->name)*0.25,indexTaskTree->yPos+0.8,-0.1);
			glVertex3f(indexTaskTree->xPos+(double)strlen(indexTaskTree->task->name)*0.25,indexTaskTree->yPos-0,-0.1);
			glVertex3f(-0.1+indexTaskTree->xPos+(double)strlen(indexTaskTree->task->name)*0.25,indexTaskTree->yPos-0.2,-0.1);
			
			}
			break;
			}

		indexTaskTree=indexTaskTree->next;
		}
		indexLineTaskTree=indexLineTaskTree->nextLevel;
		indexTaskTree=indexLineTaskTree;
	}
	glEnd();
	glColor3f(1, 1, 1);
	indexTaskTree=rootTaskTree;
	indexLineTaskTree=indexTaskTree;
	while(indexLineTaskTree!=NULL){
		while(indexTaskTree!=NULL){
			TaskRuns tempR=indexTaskTree->task->runs;
			DuplicableTask tempD=indexTaskTree->task->iterations;
		while(tempR!=NULL&&(tempR->endTime>t||tempR->endTime+500<t)){
			tempR=tempR->next;
		}
		if(indexTaskTree->task->type==1&&tempR!=NULL){
			char str[15];
			sprintf(str, "%d", tempR->retVal);
			Font(GLUT_BITMAP_HELVETICA_10,str,indexTaskTree->xPos+(double)strlen(indexTaskTree->task->name)*0.25+0.1,indexTaskTree->yPos+0.7);
		}else if(indexTaskTree->task->type==2){
			int activeCopies=0;
			int finishedCopies=0;
			int allCopies=0;
			while(tempD!=NULL){
				if(tempD->startTime<=t){
					activeCopies=tempD->activeCopies;
					finishedCopies=tempD->finishedCopies;
					allCopies=tempD->allCopies;
				}
				tempD=tempD->nextIteration;
			}
						char str[15];
			sprintf(str, "%d/%d/%d",activeCopies, finishedCopies,allCopies);
			Font(GLUT_BITMAP_HELVETICA_10,str,(double)indexTaskTree->xPos-1.5,indexTaskTree->yPos);
		}
			indexTaskTree=indexTaskTree->next;
		}
		indexLineTaskTree=indexLineTaskTree->nextLevel;
		indexTaskTree=indexLineTaskTree;
	}
	
	tempA=tasks;
	rightMarrgin=3,upDownMarrgin=0.05;
	glColor3f(1, 1, 1);
	for (int i=0;i<numTask;i++){
		if(tempA->dep!=NULL){
			if(tempA->dep->first!=NULL){
				if(tempA->treePos->yPos>tempA->dep->first->treePos->yPos){
					Font(GLUT_BITMAP_HELVETICA_10,tempA->dep->firstType,rightMarrgin+xStartPointTree+0.1,(double)(tempA->treePos->yPos+tempA->dep->first->treePos->yPos)/2);
					rightMarrgin+=0.05;
					upDownMarrgin+=0.05;
				}else{
				if(tempA->dep->firstAndsec!=2)
				Font(GLUT_BITMAP_HELVETICA_10,tempA->dep->firstType,(double)(tempA->treePos->xPos+0.2+tempA->dep->first->treePos->xPos+0.2)/2+0.1,(double)(tempA->treePos->yPos+0.5+tempA->dep->first->treePos->yPos-0.3)/2);
				else
				Font(GLUT_BITMAP_HELVETICA_10,tempA->dep->firstType,(double)(tempA->treePos->xPos+0.2+tempA->dep->first->treePos->xPos+0.2)/2+0.1,(double)(tempA->treePos->yPos+1.25+tempA->dep->first->treePos->yPos-0.3)/2);
				}
			}
			if(tempA->dep->sec!=NULL){
				if(tempA->treePos->yPos>tempA->dep->sec->treePos->yPos){
				Font(GLUT_BITMAP_HELVETICA_10,tempA->dep->secType,rightMarrgin+xStartPointTree+0.1,(double)(tempA->treePos->yPos+tempA->dep->sec->treePos->yPos)/2);
					rightMarrgin+=0.05;
					upDownMarrgin+=0.05;
				}else{
				if(tempA->dep->firstAndsec!=2)
					Font(GLUT_BITMAP_HELVETICA_10,tempA->dep->secType,(double)(tempA->treePos->xPos+0.2+tempA->dep->sec->treePos->xPos+0.2)/2+0.1,(double)(tempA->treePos->yPos+0.5+tempA->dep->sec->treePos->yPos-0.3)/2);
				else
					Font(GLUT_BITMAP_HELVETICA_10,tempA->dep->secType,(double)(tempA->treePos->xPos+0.2+tempA->dep->sec->treePos->xPos+0.2)/2+0.1,(double)(tempA->treePos->yPos+1.25+tempA->dep->sec->treePos->yPos-0.3)/2);
				}
			}
			if(tempA->dep->third!=NULL){
				if(tempA->treePos->yPos>tempA->dep->third->treePos->yPos){
				Font(GLUT_BITMAP_HELVETICA_10,tempA->dep->thirdType,rightMarrgin+xStartPointTree+0.1,(double)(tempA->treePos->yPos+tempA->dep->third->treePos->yPos)/2);
					rightMarrgin+=0.05;
					upDownMarrgin+=0.05;
				}else{
					Font(GLUT_BITMAP_HELVETICA_10,tempA->dep->thirdType,(double)(tempA->treePos->xPos+0.2+tempA->dep->third->treePos->xPos+0.2)/2+0.1,(double)(tempA->treePos->yPos+0.5+tempA->dep->third->treePos->yPos-0.3)/2);
				}
			}
		}
		tempA=tempA->next;
	}
	glBegin(GL_LINES);
	glColor3f(1, 1, 1);
	glVertex3f(11.9, 11.9,-0.1);
	glVertex3f(11.9, -11.9,-0.1);
	glVertex3f(-11.9, 11.9,-0.1);
	glVertex3f(-11.9, -11.9,-0.1);
	glVertex3f(11.9, 11.9,-0.1);
	glVertex3f(-11.9, 11.9,-0.1);
	glVertex3f(11.9, -11.9,-0.1);
	glVertex3f(-11.9, -11.9,-0.1);
	glEnd();
	glutSwapBuffers();
	glutPostRedisplay();
}

void OnDraw1_5(){
glutSetWindow(subWindow1_5);
	// clear the screen & depth buffer
	glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);

	// clear the previous transform
	glLoadIdentity();
	//
	// The simplest way to render geometry within openGL is to use
	// whats known as immediate mode. The drawing takes place between
	// glBegin() and glEnd(). glBegin takes the type of geometry you
	// want to draw, these can be on of the following :
	//
	//  	GL_POINTS
	//  	GL_LINES
	//  	GL_LINE_LOOP
	// 		GL_LINE_STRIP
	//  	GL_TRIANGLES
	//  	GL_TRIANGLE_STRIP
	//  	GL_TRIANGLE_FAN
	//  	GL_QUADS
	//  	GL_QUAD_STRIP
	//  	GL_POLYGON
	//
	// You can place as many calls to glVertex3f inbetween glBegin() and
	// glEnd(). For example, 6 calls to glVertex* between glBegin and glEnd
	// would produce 2 triangles....
	//
	/////////////////////////////////////////////////////////////

Font(GLUT_BITMAP_HELVETICA_18,"uti graph",-10,10);
	glBegin(GL_QUADS);	
	TimeLine tempTimeLine=timeLine;
	while(tempTimeLine!=NULL){
		if(tempTimeLine->startTime<t&&tempTimeLine->endTime<t){
			glColor3f(0.5, 0.5, 0.5);
			glVertex3f((double)startPoint-10.2+(double)tempTimeLine->startTime/scale, -10.5,0);
			glVertex3f((double)startPoint-10.2+(double)tempTimeLine->startTime/scale, -10.5+0.7*tempTimeLine->coresActive,0);
			glVertex3f((double)startPoint-10.2+(double)tempTimeLine->endTime/scale, -10.5+0.7*tempTimeLine->coresActive,0);
			glVertex3f((double)startPoint-10.2+(double)tempTimeLine->endTime/scale, -10.5,0);	
		}else if (tempTimeLine->startTime<t){
			glColor3f(0.5, 0.5, 0.5);
			glVertex3f((double)startPoint-10.2+(double)tempTimeLine->startTime/scale, -10.5,0);
			glVertex3f((double)startPoint-10.2+(double)tempTimeLine->startTime/scale, -10.5+0.7*tempTimeLine->coresActive,0);
			glVertex3f((double)startPoint-10.2+(double)t/scale, -10.5+0.7*tempTimeLine->coresActive,0);
			glVertex3f((double)startPoint-10.2+(double)t/scale,-10.5,0);
		}
		tempTimeLine=tempTimeLine->next;
	}
	

	glEnd();
glBegin(GL_LINES);
	glColor3f(1, 1, 1);
	glVertex3f(11.9, 11.9,-0.1);
	glVertex3f(11.9, -11.9,-0.1);
	glVertex3f(-11.9, 11.9,-0.1);
	glVertex3f(-11.9, -11.9,-0.1);
	glVertex3f(11.9, 11.9,-0.1);
	glVertex3f(-11.9, 11.9,-0.1);
	glVertex3f(11.9, -11.9,-0.1);
	glVertex3f(-11.9, -11.9,-0.1);
	glEnd();
	// currently we've been drawing to the back buffer, we need
	// to swap the back buffer with the front one to make the image visible
	glutSwapBuffers();
	glutPostRedisplay();

}

void OnDraw1_4(){
	glutSetWindow(subWindow1_4);
	// clear the screen & depth buffer
	glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
Font(GLUT_BITMAP_HELVETICA_12,"cores avguse",-10,10);
	// clear the previous transform
	glLoadIdentity();
	glBegin(GL_QUADS);
	TimeLine tempTimeLine=timeLine;
	while(tempTimeLine!=NULL){
		if(tempTimeLine->startTime<t&&tempTimeLine->endTime<t){
			avgUse+=tempTimeLine->coresActive*(tempTimeLine->endTime-tempTimeLine->startTime);
		}else if (tempTimeLine->startTime<t){
			avgUse+=tempTimeLine->coresActive*(t-tempTimeLine->startTime);
		}
		tempTimeLine=tempTimeLine->next;
	}
	if(t!=0){
		if(t<totalEndTime)
			avgUse=(double)avgUse/t;
		else
			avgUse=(double)avgUse/totalEndTime;
		glColor3f(0.5, 0.5, 0.5);
		glVertex3f(-9.5, -10,0);
		glVertex3f(-9.5, -10+avgUse,0);
		glVertex3f(-3, -10+avgUse,0);
		glVertex3f(-3, -10,0);
		glColor3f(1, 1, 1);
		glVertex3f(-9.5, -10,0);
		glVertex3f(-9.5, -10+NUM_OF_CORES,0);
		glVertex3f(-3, -10+NUM_OF_CORES,0);
		glVertex3f(-3, -10,0);
		avgUse=avgUse/NUM_OF_CORES;
		avgUse=avgUse*100;
	}
	glEnd();
	glColor3f(1, 1, 1);
	char str[15];
	sprintf(str, "%f", avgUse);
	strcat(str,"%");
	Font(GLUT_BITMAP_HELVETICA_10,str,-12,8);
	avgUse=0;
	glBegin(GL_LINES);
	glColor3f(1, 1, 1);
	glVertex3f(11.9, 11.9,-0.1);
	glVertex3f(11.9, -11.9,-0.1);
	glVertex3f(-11.9, 11.9,-0.1);
	glVertex3f(-11.9, -11.9,-0.1);
	glVertex3f(11.9, 11.9,-0.1);
	glVertex3f(-11.9, 11.9,-0.1);
	glVertex3f(11.9, -11.9,-0.1);
	glVertex3f(-11.9, -11.9,-0.1);
	glEnd();
	glutSwapBuffers();
	glutPostRedisplay();
}


void OnDraw1_3() {
	
	glutSetWindow(subWindow1_3);
	// clear the screen & depth buffer
	glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);

	// clear the previous transform
	glLoadIdentity();
Font(GLUT_BITMAP_HELVETICA_18,"cores",-10,10);
	glBegin(GL_QUADS);
	int x=0;
	CoresEnTimeLine tempCoresEn=coresEn;
	while (tempCoresEn!=NULL && tempCoresEn->startTime<t){
		x=tempCoresEn->numCoresEn;
		tempCoresEn=tempCoresEn->next;
	}
	for(int i=0;i<NUM_OF_CORES;i++){
	if(i<x)
		glColor3f(1,1,1);
	else
		glColor3f(0.5,0.5,0.5);
		CoreTask CoreTaskList=cores[i]->CoreTaskList;
		for (int k=cores[i]->taskTotalNum;k>0;k--){
			if(CoreTaskList->startTime<=t&&CoreTaskList->endTime>=t){
				glColor3f(CoreTaskList->task->red, CoreTaskList->task->green, CoreTaskList->task->blue);
			}
			CoreTaskList=CoreTaskList->next;
		}

		glVertex3f(-10+4.2*(i%4)+3.7,5-(i/4)*5.2,-0.1);

		//glColor3f(0,1,0);
		glVertex3f(-10+4.2*(i%4),5-(i/4)*5.2,-0.1);

		//glColor3f(0,0,1);
		glVertex3f(-10+4.2*(i%4),5-(i/4)*5.2+4.7,-0.1);

		glVertex3f(-10+4.2*(i%4)+3.7,5-(i/4)*5.2+4.7,-0.1);
	}
	glEnd();
	for(int i=0;i<NUM_OF_CORES;i++){
		glColor3f(1,1,1);
		CoreTask CoreTaskList=cores[i]->CoreTaskList;
		for (int k=cores[i]->taskTotalNum;k>0;k--){
			if(CoreTaskList->startTime<=t&&CoreTaskList->endTime>t){
				Font(GLUT_BITMAP_HELVETICA_10,CoreTaskList->task->name,-10+4.2*(i%4)+0.2,5-(i/4)*5.2+2.35);
				if(CoreTaskList->task->type==2){
					char str[15];
					sprintf(str, "%d", CoreTaskList->itr);
					Font(GLUT_BITMAP_HELVETICA_10,str,-10+4.2*(i%4)+1.2,5-(i/4)*5.2+1.35);
				}
			}

			CoreTaskList=CoreTaskList->next;
		}
	}
	glBegin(GL_LINES);
	glColor3f(1, 1, 1);
	glVertex3f(11.9, 11.9,-0.1);
	glVertex3f(11.9, -11.9,-0.1);
	glVertex3f(-11.9, 11.9,-0.1);
	glVertex3f(-11.9, -11.9,-0.1);
	glVertex3f(11.9, 11.9,-0.1);
	glVertex3f(-11.9, 11.9,-0.1);
	glVertex3f(11.9, -11.9,-0.1);
	glVertex3f(-11.9, -11.9,-0.1);
	glEnd();
	glutSwapBuffers();
	glutPostRedisplay();
}

void OnDraw1_2() {
glutSetWindow(subWindow1_2);
	// clear the screen & depth buffer
	glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);

	// clear the previous transform
	glLoadIdentity();
	Font(GLUT_BITMAP_HELVETICA_18,"per core graph",-10,10);
		glBegin(GL_QUADS);	
	for(int i=0;i<NUM_OF_CORES;i++){
		CoreTask CoreTaskList=cores[i]->CoreTaskList;
		for (int k=cores[i]->taskTotalNum;k>0;k--){
			if(CoreTaskList->startTime<t&&CoreTaskList->endTime<t){
				glColor3f(CoreTaskList->task->red, CoreTaskList->task->green, CoreTaskList->task->blue);
				//glColor3f(0.5, 0.5, 0.5);
				glVertex3f((double)startPoint-10.2+(double)CoreTaskList->startTime/scale, -10.5+1.2*i,0.1);
				glVertex3f((double)startPoint-10.2+(double)CoreTaskList->startTime/scale, -9.5+1.2*i,0.1);
				glVertex3f((double)startPoint-10.2+(double)CoreTaskList->endTime/scale, -9.5+1.2*i,0.1);
				glVertex3f((double)startPoint-10.2+(double)CoreTaskList->endTime/scale, -10.5+1.2*i,0.1);
				CoreTaskList=CoreTaskList->next;
			}else if (CoreTaskList->startTime<t){
				glColor3f(CoreTaskList->task->red, CoreTaskList->task->green, CoreTaskList->task->blue);
				//glColor3f(0.5, 0.5, 0.5);
				glVertex3f((double)startPoint-10.2+(double)CoreTaskList->startTime/scale, -10.5+1.2*i,0.1);
				glVertex3f((double)startPoint-10.2+(double)CoreTaskList->startTime/scale, -9.5+1.2*i,0.1);
				glVertex3f((double)startPoint-10.2+(double)t/scale, -9.5+1.2*i,0.1);
				glVertex3f((double)startPoint-10.2+(double)t/scale, -10.5+1.2*i,0.1);
				CoreTaskList=CoreTaskList->next;
			}
		}
	}
	glEnd();
	glBegin(GL_LINES);
	glColor3f(1, 1, 1);
	glVertex3f(11.9, 11.9,-0.1);
	glVertex3f(11.9, -11.9,-0.1);
	glVertex3f(-11.9, 11.9,-0.1);
	glVertex3f(-11.9, -11.9,-0.1);
	glVertex3f(11.9, 11.9,-0.1);
	glVertex3f(-11.9, 11.9,-0.1);
	glVertex3f(11.9, -11.9,-0.1);
	glVertex3f(-11.9, -11.9,-0.1);
	glEnd();
	glutSwapBuffers();
	glutPostRedisplay();
}

void OnDraw1_1() {
glutSetWindow(subWindow1_1);
	// clear the screen & depth buffer
	glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);

	// clear the previous transform
	glLoadIdentity();
	Font(GLUT_BITMAP_HELVETICA_18,"task list",-10,10);
	glBegin(GL_QUADS);

	for(int i=1;i<numTask+1;i++){
		glColor3f(0.1f, 0.1f, 0.1f);
		glVertex3f(-10.2, 10.5-i,-0.1);
		//glColor3f(0.0f, 0.0f, 0.0f);
		glVertex3f(-10.2, 9.9-i,-0.1);
		//glColor3f(0.0f, 0.0f, 0.0f); 
		glVertex3f(10, 9.9-i,-0.1);
		//glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3f(10, 10.5-i,-0.1);
	}
	glEnd();
	TaskList tempT=tasks;
	for(int i=1;i<numTask+1;i++){
		glColor3f(tempT->red, tempT->green, tempT->blue);
		 Font(GLUT_BITMAP_HELVETICA_10,tempT->name,-10,10-i);
		 tempT=tempT->next;
	}
glBegin(GL_LINES);
	glColor3f(1, 1, 1);
	glVertex3f(11.9, 11.9,-0.1);
	glVertex3f(11.9, -11.9,-0.1);
	glVertex3f(-11.9, 11.9,-0.1);
	glVertex3f(-11.9, -11.9,-0.1);
	glVertex3f(11.9, 11.9,-0.1);
	glVertex3f(-11.9, 11.9,-0.1);
	glVertex3f(11.9, -11.9,-0.1);
	glVertex3f(-11.9, -11.9,-0.1);
	glEnd();
		glutSwapBuffers();
	glutPostRedisplay();
}

void OnDraw() {
glutSetWindow(window_1);
	// clear the screen & depth buffer
	glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);

	// clear the previous transform
	glLoadIdentity();
	// currently we've been drawing to the back buffer, we need
	// to swap the back buffer with the front one to make the image visible
	glutSwapBuffers();
	glutPostRedisplay();
}

//------------------------------------------------------------	OnInit()
//
void OnInit2() {
	glEnable(GL_DEPTH_TEST);
	srand(time(NULL));
	dllTimeline=malloc(sizeof(*dllTimeline));
	dllTimeline->numOfPar=0;
	dllTimeline->startTime=0;
	dllTimeline->endTime=0;
	dllTimeline->next=NULL;
	int numOfPar=0;
	char* line = (char*) malloc(257 * sizeof(char));
	FILE* logFile = fopen("log", "r");
	bool first=true;
	fgets(line, 256, logFile);
	while (line[0]>'9'||line[0]<'0'){
	fgets(line, 256, logFile);
	}
	while (!(line[0]>'9'||line[0]<'0')){
	DLLTaskList tempTask=dllTasks;
	if(first){
	dllTasks=malloc(sizeof(*dllTasks));
	tempTask=dllTasks;
	first=false;
	}else{
	while(tempTask->next!=NULL)
		tempTask=tempTask->next;
	tempTask->next=malloc(sizeof(*tempTask->next));
	tempTask=tempTask->next;
	}
	tempTask->name=malloc(sizeof(char)*4);
	strncpy(tempTask->name, line, 3);
	tempTask->name[3]='\0';
	float a = rand();
	float c = rand();
	float b = rand();
	a=a/RAND_MAX;
	b=b/RAND_MAX;
	c=c/RAND_MAX;
	tempTask->red=a;
	tempTask->green=b;
	tempTask->blue=c;
	tempTask->next=NULL;
	fgets(line, 256, logFile);
	}
	for(int i=0;i<NUM_OF_PARTITIONS;i++){
	dlls[i]=malloc(sizeof(*dlls[i]));
	dlls[i]->startTime=-1;
	dlls[i]->endTime=-1;
	dlls[i]->next=NULL;
	}

	char* prevLine = (char*) malloc(257 * sizeof(char));
	int j=0;
	FILE* recordFile = fopen("record", "r");
	char *p =line+3;
	char *end;
	int i;
	////////////////////////////////////////
	while(fgets(line, 256, recordFile)!=NULL){
		//i = strtol(p, &end, 10);
		//p =line+18;
		//long coreNum = strtol(p, &end, 10);
		//tempTimeLine=timeLine;	
		if(strncmp(&line[13],"accelerator",11)==0&&strlen(line)>20){		
			int n=25,accindex=25;
			while(line[n]==' '||line[n]==','){
			n++;
			accindex++;
			}
			while (line[n]!='l'&&line[n]!='u')
				n++;
			if(line[n]=='l'){
				n+=36;
				while(line[n]<'0'||line[n]>'9')
					n++;
				n--;
				while(line[n]!='r'&&line[n]!='.'){
					n++;
					i = strtol(line+n, &end, 10);
					DLLTask tempDLL=dlls[i];
					if(tempDLL->startTime!=-1){
						while (tempDLL->next!=NULL)
							tempDLL=tempDLL->next;
						tempDLL->next=malloc(sizeof(*tempDLL->next));
						tempDLL=tempDLL->next;
					}
					numOfPar++;
					tempDLL->name=malloc(sizeof(char)*4);
					strncpy(tempDLL->name, line+accindex, 3);
					tempDLL->name[3]='\0';
					tempDLL->startTime=dllTimeLapse+strtod(prevLine+72,NULL);
					tempDLL->endTime=-1;
					tempDLL->next=NULL;
					accindex+=5;
					n++;
					if(line[n]>='0'&&line[n]<='9')
						n++;
					if(line[n]!='.')
						n++;
				}
			}else if (line[n]=='u'){
				n+=38;
				while(line[n]<'0'||line[n]>'9')
					n++;
				n--;
				while(line[n]!='r'&&line[n]!='.'){
					n++;
					i = strtol(line+n, &end, 10);
					DLLTask tempDLL=dlls[i];
					while (tempDLL->next!=NULL)
						tempDLL=tempDLL->next;
					numOfPar--;
					tempDLL->endTime=dllTimeLapse+strtod(prevLine+72,NULL);
					n++;
					if(line[n]>='0'&&line[n]<='9')
						n++;
						if(line[n]!='.')
					n++;
				}
			}
			
			DLLTimeline tempDllTimeline=dllTimeline;
			while (tempDllTimeline->next!=NULL)
				tempDllTimeline=tempDllTimeline->next;
			tempDllTimeline->next=malloc(sizeof(*tempDllTimeline->next));
			tempDllTimeline->next->numOfPar=numOfPar;
			tempDllTimeline->next->startTime=dllTimeLapse+strtod(prevLine+72,NULL);
			tempDllTimeline->next->endTime=-1;
			tempDllTimeline->next->next=NULL;
			tempDllTimeline->endTime=tempDllTimeline->next->startTime;
			}else if(strncmp(line+37,"destroyed",9)==0){
			numOfPar=0;
			dllTimeLapse+=strtod(prevLine+72,NULL);
			for(int i=0;i<NUM_OF_PARTITIONS;i++){
			DLLTask tempDLL=dlls[i];
			if(tempDLL->startTime!=-1){
			while (tempDLL->next!=NULL)
				tempDLL=tempDLL->next;
				if(tempDLL->endTime==-1)
			tempDLL->endTime=dllTimeLapse;
			}
			}
			DLLTimeline tempDllTimeline=dllTimeline;
			while (tempDllTimeline->next!=NULL)
				tempDllTimeline=tempDllTimeline->next;
			tempDllTimeline->next=malloc(sizeof(*tempDllTimeline->next));
			tempDllTimeline->next->numOfPar=numOfPar;
			tempDllTimeline->next->startTime=dllTimeLapse;
			tempDllTimeline->next->endTime=-1;
			tempDllTimeline->next->next=NULL;
			tempDllTimeline->endTime=tempDllTimeline->next->startTime;
			}
		p =line+3;
		strcpy(prevLine,line);
	}
		for(int i=0;i<NUM_OF_PARTITIONS;i++){
		DLLTask dllTaskList=dlls[i];
		while(dllTaskList->next!=NULL)
			dllTaskList=dllTaskList->next;
		if(dllTaskList->endTime==-1&&dllTaskList->startTime!=-1)
			dllTaskList->endTime=dllTimeLapse;
		}
	fclose(recordFile);
	///////////////////////////////////////////////////////////////////////
	free(line);
	free(prevLine);
	//free(tempName);


}

void OnInitC_1() {
	glEnable(GL_DEPTH_TEST);
	for(int i=0;i<7;i++)
		buttons[i]=false;
}

void OnInitC_2() {
	glEnable(GL_DEPTH_TEST);
	for(int i=0;i<11;i++){
		hidebuttons[i]=false;
		windowState[i]=true;
	}
}

void OnInit1_2() {
	glEnable(GL_DEPTH_TEST);
}
void OnInit1_1() {
	glEnable(GL_DEPTH_TEST);
}
void OnInit() {
	// enable depth testing
	glEnable(GL_DEPTH_TEST);
	srand(time(NULL));

	char* line = (char*) malloc(257 * sizeof(char));	
	TaskList tempA;
	char* tempName=(char*) malloc(257 * sizeof(char));
	FILE* taskFile1 = fopen("task.map", "r");
	int j=0,x=0;
	while (fgets(line, 256, taskFile1) != NULL) {
		j=0;
		while(line[j]!=' ')
			j++;
		j++;
		while(line[j]!=' ')
			j++;
		j++;
		x=0;
		while (line[j+x]!='('){
			tempName[x]=line[j+x];
			x++;
		}
		tempName[x]='\0';
		float a = rand();
		float c = rand();
		float b = rand();
		a=a/RAND_MAX;
		b=b/RAND_MAX;
		c=c/RAND_MAX;
		if(numTask==0){
			tempA=malloc(sizeof(*tempA));
			tasks=tempA;
			tempA->name=malloc(sizeof(char)*(strlen(tempName)+1));
			tempA->red=a;
			tempA->green=b;
			tempA->blue=c;
			tempA->iterations=NULL;
			strncpy(tempA->name,tempName,strlen(tempName));
			tempA->name[strlen(tempName)]='\0';
			tempA->runs=NULL;
			if(strncmp(&line[0],"dummy",5)==0)
				tempA->type=0;
			else if(strncmp(&line[0],"regular",7)==0)
				tempA->type=1;
			else if(strncmp(&line[0],"duplicable",10)==0)
				tempA->type=2;
		}else{
			tempA->next=malloc(sizeof(*tempA->next));
			tempA->next->name=malloc(sizeof(char)*(strlen(tempName)+1));
			tempA->next->red=a;
			tempA->next->green=b;
			tempA->next->blue=c;
			tempA->next->iterations=NULL;
			strncpy(tempA->next->name,tempName,strlen(tempName));
			tempA->next->name[strlen(tempName)]='\0';
			tempA=tempA->next;
			tempA->runs=NULL;
			if(strncmp(&line[0],"dummy",5)==0)
				tempA->type=0;
			else if(strncmp(&line[0],"regular",7)==0)
				tempA->type=1;
			else if(strncmp(&line[0],"duplicable",10)==0)
				tempA->type=2;
		}
		numTask++;
	}
	fclose(taskFile1);
	taskFile1 = fopen("task.map", "r");
	tempA=tasks;
	for (int i=0;i<numTask;i++){
		fgets(line, 256, taskFile1);
		j=0;
		while(line[j]!=' ')
			j++;
		j++;
		while(line[j]!=' ')
			j++;
		j++;
		x=0;
		while (line[j+x]!='(')
			x++;
		findDependencyList(tempA,line,j+x);
		tempA=tempA->next;
	}
	
	fclose(taskFile1);
	
	
	//////////////////////////////////////////////////////////////////////////
	int currentLevel=0;
	int numOfTasksOnTree=0;
	tempA=tasks;
	rootTaskTree=malloc(sizeof(*rootTaskTree));
	TaskMainTree indexTaskTree=rootTaskTree;
	indexTaskTree->next=NULL;
	indexTaskTree->nextLevel=NULL;
	for(int i=0;i<numTask;i++){
		if (tempA->dep==NULL){
			if (numOfTasksOnTree!=0){
				indexTaskTree->next=malloc(sizeof(*indexTaskTree->next));
				indexTaskTree=indexTaskTree->next;
			}
			indexTaskTree->task=tempA;
			tempA->treePos=indexTaskTree;
			indexTaskTree->next=NULL;
			numOfTasksOnTree++;
		}
		tempA=tempA->next;
	}
	if(numOfTasksOnTree!=numTask){
		currentLevel++;
		rootTaskTree->nextLevel=malloc(sizeof(*indexTaskTree->nextLevel));
		indexTaskTree=rootTaskTree->nextLevel;
		indexTaskTree->nextLevel=NULL;
	}
	TaskMainTree indexLineTaskTree=rootTaskTree->nextLevel;
	while(numOfTasksOnTree!=numTask){
		tempA=tasks;
		bool firstInLine=true;
		for(int i=0;i<numTask;i++){
			if ((!isTaskOnTree(tempA,currentLevel))&&checkDependencyList(tempA,currentLevel)){
				if(firstInLine){
					firstInLine=false;
				}else{
					indexTaskTree->next=malloc(sizeof(*indexTaskTree->next));
					indexTaskTree=indexTaskTree->next;
				}
				indexTaskTree->task=tempA;
				tempA->treePos=indexTaskTree;
				indexTaskTree->next=NULL;
				numOfTasksOnTree++;
			}
			tempA=tempA->next;
		}
		if (firstInLine&&numOfTasksOnTree!=numTask){
			tempA=tasks;
			while(isTaskOnTree(tempA,currentLevel)){
				tempA=tempA->next;
			}
			indexTaskTree->task=tempA;
			tempA->treePos=indexTaskTree;
			indexTaskTree->next=NULL;
			numOfTasksOnTree++;
		}
		if(numOfTasksOnTree!=numTask){
			currentLevel++;
			indexLineTaskTree->nextLevel=malloc(sizeof(*indexLineTaskTree->nextLevel));
			indexLineTaskTree=indexLineTaskTree->nextLevel;
			indexTaskTree=indexLineTaskTree;
			indexTaskTree->nextLevel=NULL;
		}
	}
	///////////////////////////////////////////////////////////////////////////
	bool first=true;
	bool coreActive[NUM_OF_CORES];
	int numOfCoresActive=0;
	for (int i=0;i<NUM_OF_CORES;i++)
		coreActive[i]=false;
	///////////////////////////////////////////////////////////////////////
	for (int i=0;i<NUM_OF_CORES;i++){
		cores[i]=malloc(sizeof(*cores[i]));
		cores[i]->coreNum=i;
		cores[i]->taskTotalNum=0;
	}
	///////////////////////////////////////////////////////////////////////
	TimeLine tempTimeLine=timeLine;	
	FILE* dataFile = fopen("text.txt", "r");
	fgets(line, 256, dataFile);
	while(line[0]!='a'){
		fgets(line, 256, dataFile);
	}
	
	coresEn=malloc(sizeof(*coresEn)); 
	coresEn->numCoresEn=NUM_OF_CORES;
	coresEn->startTime=0;
	coresEn->endTime=-1;
	coresEn->next=NULL;
	
	char *p =line+3;
	char *end;
	long i=0;
	int coreNumIndex=0,taskNameIndex=0,actionIndex=0,index=3;
	////////////////////////////////////////
	while(fgets(line, 256, dataFile)!=NULL){
		if(coreNumIndex==0){
			while (i<3){
				if(line[index]==' '&&line[index+1]!=' ')
					i++;
				index++;
			}
			coreNumIndex=index-1;
			while (i<6){
				if(line[index]==' '&&line[index+1]!=' ')
					i++;
				index++;
			}
			taskNameIndex=index;
			while (!(line[index]==' '&&line[index+1]!=' '))
				index++;
			index++;
			actionIndex=index;
		}
		if(line[0]=='a'&&line[1]=='t'&&line[2]==':'){
		if(line[actionIndex]=='s'&&line[actionIndex+1]=='c'){
			i = strtol(p, &end, 10);
			p =line+coreNumIndex;
			long coreNum = strtol(p, &end, 10);
			tempTimeLine=timeLine;	
			TaskList tempT=tasks;
			if(first){
				timeLine=malloc(sizeof(*timeLine));
				timeLine->next=NULL;
				tempTimeLine=timeLine;
				first=false;
			}else{
				while(tempTimeLine->next!=NULL)
					tempTimeLine=tempTimeLine->next;
				tempTimeLine->next=malloc(sizeof(*tempTimeLine->next));
				tempTimeLine->next->next=NULL;
				tempTimeLine->endTime=i;
				tempTimeLine=tempTimeLine->next;
			}
			coreActive[coreNum]=true;
			numOfCoresActive++;
			tempTimeLine->startTime=i;
			tempTimeLine->endTime=-1;
			tempTimeLine->coresActive=numOfCoresActive;
			///////////////////////////////////////////////////////////////
			if (cores[coreNum]->taskTotalNum==0){
				cores[coreNum]->CoreTaskList=malloc(sizeof(*cores[coreNum]->CoreTaskList));
				for(j=0;line[taskNameIndex+j]!=' ';j++)
					tempName[j]=line[taskNameIndex+j];
				tempName[j]='\0';
				while(strcmp(tempName,tempT->name)!=0)
					tempT=tempT->next;
				cores[coreNum]->CoreTaskList->task=tempT;
				cores[coreNum]->CoreTaskList->startTime=i;
				if(tempT->type==2)
					cores[coreNum]->CoreTaskList->itr=strtol(line+actionIndex+21, &end, 10);
				else
					cores[coreNum]->CoreTaskList->itr=-1;
				cores[coreNum]->CoreTaskList->endTime=-1;
				cores[coreNum]->taskTotalNum=1;
			}else{
				CoreTask newCoreTask=cores[coreNum]->CoreTaskList;
				for (int k=cores[coreNum]->taskTotalNum;k>1;k--)
					newCoreTask=newCoreTask->next;
				newCoreTask->next=malloc(sizeof(*newCoreTask->next));
				for(j=0;line[taskNameIndex+j]!=' ';j++)
					tempName[j]=line[taskNameIndex+j];
				tempName[j]='\0';
				while(strcmp(tempName,tempT->name)!=0)
					tempT=tempT->next;
				newCoreTask->next->task=tempT;
				newCoreTask->next->startTime=i;
				if(tempT->type==2)
					newCoreTask->next->itr=strtol(line+actionIndex+21, &end, 10);
				else
					newCoreTask->next->itr=-1;
				newCoreTask->next->endTime=-1;
				cores[coreNum]->taskTotalNum++;
			}
			
			if (tempT->type==2){
			DuplicableTask tempD=tempT->iterations;
			while(tempD->nextIteration!=NULL)
				tempD=tempD->nextIteration;
			if(tempD->finishedCopies+tempD->activeCopies!=tempD->allCopies){	
			tempD->endTime=i;
			tempD->nextIteration=malloc(sizeof(*tempD->nextIteration));
			tempD->nextIteration->startTime=i;
			tempD->nextIteration->endTime=-1;
			tempD->nextIteration->activeCopies=tempD->activeCopies+1;
			tempD->nextIteration->finishedCopies=tempD->finishedCopies;
			tempD->nextIteration->allCopies=tempD->allCopies;
			tempD->nextIteration->nextIteration=NULL;
			}
			}else{
				if(tempT->runs==NULL){
					tempT->runs=malloc(sizeof(*tempT->runs));
					tempT->runs->startTime=i;
					tempT->runs->endTime=-1;
					tempT->runs->retVal=0;
					tempT->runs->next=NULL;
				}else{
					TaskRuns tempR=tempT->runs;
					while(tempR->next!=NULL)
						tempR=tempR->next;
					tempR->next=malloc(sizeof(*tempR->next));
					tempR->next->startTime=i;
					tempR->next->endTime=-1;
					tempR->next->retVal=0;
					tempR->next->next=NULL;
				}
			}
		} else if(line[actionIndex]=='t'){
			//////////////////////////////////////////////////////
			i = strtol(p, &end, 10);
			p =line+coreNumIndex;
			long coreNum = strtol(p, &end, 10);
			tempTimeLine=timeLine;	
			TaskList tempT=tasks;
			if(coreActive[coreNum]){
				while(tempTimeLine->next!=NULL)
					tempTimeLine=tempTimeLine->next;
				tempTimeLine->next=malloc(sizeof(*tempTimeLine->next));
				tempTimeLine->next->next=NULL;
				tempTimeLine->endTime=i;
				tempTimeLine=tempTimeLine->next;
				coreActive[coreNum]=false;
				numOfCoresActive--;
				tempTimeLine->startTime=i;
				tempTimeLine->endTime=-1;
				tempTimeLine->coresActive=numOfCoresActive;
				totalEndTime=i;
			}
			CoreTask newCoreTask=cores[coreNum]->CoreTaskList;
			for (int k=cores[coreNum]->taskTotalNum;k>1;k--)
				newCoreTask=newCoreTask->next;
			if (newCoreTask->endTime==-1)
				newCoreTask->endTime=i;
			for(j=0;line[taskNameIndex+j]!=' ';j++)
				tempName[j]=line[taskNameIndex+j];
			tempName[j]='\0';
			while(strcmp(tempName,tempT->name)!=0)
				tempT=tempT->next;
			if (tempT->type==2){
			DuplicableTask tempD=tempT->iterations;
			while(tempD->nextIteration!=NULL)
				tempD=tempD->nextIteration;
			tempD->endTime=i;
			if(tempD->finishedCopies!=tempD->allCopies){
			tempD->nextIteration=malloc(sizeof(*tempD->nextIteration));
			tempD->nextIteration->startTime=i;
			tempD->nextIteration->endTime=-1;
			tempD->nextIteration->activeCopies=tempD->activeCopies-1;
			tempD->nextIteration->finishedCopies=tempD->finishedCopies+1;
			tempD->nextIteration->allCopies=tempD->allCopies;
			tempD->nextIteration->nextIteration=NULL;
			}
			}else{
			
				TaskRuns tempR=tempT->runs;
				while(tempR->next!=NULL)
					tempR=tempR->next;
				tempR->endTime=i;
				if(line[actionIndex+12]=='r')
					tempR->retVal=strtol(line+actionIndex+19, &end, 10);
			}
			}else if(line[actionIndex]=='S'){
				i = strtol(p, &end, 10);
				p =line+coreNumIndex;
				long coreNum = strtol(p, &end, 10);
				tempTimeLine=timeLine;	
				TaskList tempT=tasks;
				for(j=0;line[taskNameIndex+j]!=' ';j++)
					tempName[j]=line[taskNameIndex+j];
				tempName[j]='\0';
				while(strcmp(tempName,tempT->name)!=0)
					tempT=tempT->next;
				if(tempT->iterations==NULL){
					tempT->iterations=malloc(sizeof(*tempT->iterations));
						tempT->iterations->startTime=i;
						tempT->iterations->endTime=i;
						tempT->iterations->activeCopies=0;
						tempT->iterations->finishedCopies=0;
						tempT->iterations->allCopies=strtol(line+actionIndex+8, &end, 10);
						tempT->iterations->nextIteration=NULL;
				}else{
					DuplicableTask tempD=tempT->iterations;
					while(tempD->nextIteration!=NULL){
						tempD=tempD->nextIteration;
					}
					tempD->nextIteration=malloc(sizeof(*tempD->nextIteration));
					tempD->nextIteration->allCopies=strtol(line+actionIndex+8, &end, 10);
					tempD->nextIteration->startTime=i;
					tempD->nextIteration->endTime=i;
					tempD->nextIteration->activeCopies=0;
					tempD->nextIteration->finishedCopies=0;
					tempD->nextIteration->nextIteration=NULL;
				}
			}else if(line[actionIndex]=='E'){
				i = strtol(p, &end, 10);
				p =line+coreNumIndex;
				long coreNum = strtol(p, &end, 10);
				tempTimeLine=timeLine;	
				TaskList tempT=tasks;
				for(j=0;line[taskNameIndex+j]!=' ';j++)
					tempName[j]=line[taskNameIndex+j];
				tempName[j]='\0';
				while(strcmp(tempName,tempT->name)!=0)
					tempT=tempT->next;
				if(tempT->runs==NULL){
					tempT->runs=malloc(sizeof(*tempT->runs));
					tempT->runs->startTime=i;
					tempT->runs->endTime=i;
					tempT->runs->retVal=0;
					tempT->runs->next=NULL;
				}else{
					TaskRuns tempR=tempT->runs;
					while(tempR->next!=NULL)
						tempR=tempR->next;
					tempR->next=malloc(sizeof(*tempR->next));
					tempR->next->startTime=i;
					tempR->next->retVal=0;
					tempR->next->endTime=i;
					tempR->next->next=NULL;
				}
			}else if(line[coreNumIndex-4]=='n'){
			i = strtol(p, &end, 10);
			int z = strtol(line+coreNumIndex+15, &end, 10);
				CoresEnTimeLine tempCoresEn=coresEn;
				while (tempCoresEn->next!=NULL)
					tempCoresEn=tempCoresEn->next;
				tempCoresEn->endTime=i;
				tempCoresEn->next=malloc(sizeof(*tempCoresEn->next));
				tempCoresEn->next->numCoresEn=z;
				tempCoresEn->next->startTime=i;
				tempCoresEn->next->endTime=-1;
				tempCoresEn->next->next=NULL;	
			}
			//////////////////////////////////////////////////////
		p =line+3;
		}
	}
	while(tempTimeLine->next!=NULL)
		tempTimeLine=tempTimeLine->next;
	tempTimeLine->endTime=i;
	CoresEnTimeLine tempCoresEn=coresEn;
	while (tempCoresEn->next!=NULL)
		tempCoresEn=tempCoresEn->next;
	tempCoresEn->endTime=i;
	fclose(dataFile);
	///////////////////////////////////////////////////////////////////////
	free(line);
	free(tempName);
}

//------------------------------------------------------------	OnExit()
//
void OnExit() {
if(displayDll){
	DLLTimeline tempdlltime=dllTimeline->next;
	while(dllTimeline->next!=NULL){
	free(dllTimeline);
	dllTimeline=tempdlltime;
	tempdlltime=tempdlltime->next;
	}
	free(dllTimeline);
	
	DLLTaskList tempdlltask=dllTasks->next;
	while(dllTasks->next!=NULL){
	free(dllTasks->name);
	free(dllTasks);
	dllTasks=tempdlltask;
	tempdlltask=tempdlltask->next;
	}
		free(dllTasks->name);
	free(dllTasks);
	
	for(int i=0;i<NUM_OF_PARTITIONS;i++){
		DLLTask dllTaskList=dlls[i];
		DLLTask dllTaskList2=dlls[i]->next;
		while(dllTaskList2!=NULL){
		dllTaskList=dllTaskList2;
		dllTaskList2=dllTaskList->next;
		free(dllTaskList->name);
		free(dllTaskList);
		}
	}
	}
	CoresEnTimeLine tempCoresEn=coresEn->next;
	while (coresEn->next!=NULL){
	free(coresEn);
		coresEn=tempCoresEn;
		tempCoresEn=tempCoresEn->next;
	}
	free(coresEn);
	TaskMainTree temptree1=rootTaskTree;
	TaskMainTree temptree2=rootTaskTree;
	TaskMainTree temptree3=NULL;
	while(temptree2!=NULL){
		if(temptree2->nextLevel!=NULL)
			temptree3=temptree2->nextLevel;
		else
			temptree3=NULL;
		while(temptree2!=NULL){
			temptree1=temptree2;
			temptree2=temptree1->next;
			free(temptree1);
		}
		temptree2=temptree3;
	}
		for(int i=0;i<NUM_OF_CORES;i++){
		CoreTask  coreTaskList=cores[i]->CoreTaskList;
		CoreTask  coreTaskList2=cores[i]->CoreTaskList;
		while(coreTaskList2!=NULL){
		coreTaskList=coreTaskList2;
		coreTaskList2=coreTaskList->next;
		free(coreTaskList);
		}
		}
		TimeLine  ttimeLine=timeLine;
		TimeLine  ttimeLine2=timeLine;
		while(ttimeLine2!=NULL){
		ttimeLine=ttimeLine2;
		ttimeLine2=ttimeLine->next;
		free(ttimeLine);
		}
		
		TaskList  ttask=tasks;
		TaskList  ttask2=tasks;
		while(ttask2!=NULL){
		ttask=ttask2;
		ttask2=ttask->next;
		
		TaskRuns truns =ttask->runs;
		TaskRuns truns2 =ttask->runs;
		while(truns2!=NULL){
		truns=truns2;
		truns2=truns->next;
		free(truns);
		}
		
		DuplicableTask tdup =ttask->iterations;
		DuplicableTask tdup2 =ttask->iterations;
		while(truns2!=NULL){
		tdup=tdup2;
		tdup2=tdup->nextIteration;
		free(tdup);
		}
		if(ttask->dep!=NULL){
			free(ttask->dep->firstType);
			free(ttask->dep->secType);
			free(ttask->dep->thirdType);
			free(ttask->dep);
		}
		free(ttask->name);
		free(ttask);
		}
		
		TaskList tasks;	
}



void subwindowResize (int window,double* subWindowX,double* subWindowY,double* subWindowW,double* subWindowH,int x,int y){
	
	if(resizeRIGHT){
		glutSetWindow(window);
		*subWindowW=x;
	}
	if(resizeLEFT){
		glutSetWindow(window);
		if(x>0){
		(*subWindowX)++;
		(*subWindowW)--;
		}else{
		*subWindowX=x+*subWindowX;
		*subWindowW=*subWindowW-x;
		}
	}
	if (resizeUP){
		glutSetWindow(window);
		if(y>0){
		(*subWindowY)++;
		(*subWindowH)--;
		}else{
		*subWindowY=y+*subWindowY;
		*subWindowH=*subWindowH-y;
		}
	}
	if(resizeDOWN){
	glutSetWindow(window);
	*subWindowH=y;
	}
	if(resizeUP||resizeLEFT||resizeRIGHT||resizeDOWN){
		glutReshapeWindow(*subWindowW,*subWindowH);
		if(resizeUP||resizeLEFT){
		glutPositionWindow(*subWindowX,*subWindowY);
		}
	}
	if(movingDelay==9){
	if(moveWindow){
	glutSetWindow(window);
	*subWindowX=*subWindowX+x-xOrigin;
	*subWindowY=*subWindowY+y-yOrigin;
	glutPositionWindow(*subWindowX,*subWindowY);
	movingDelay=0;
	}
	}else{
	movingDelay++;
	}
	
}


// -----------------------------------
//             MOUSE
// -----------------------------------

void mouseMove1_1(int x, int y) {
	// this will only be true when the left button is down
subwindowResize(subWindow1_1,&subWindow1_1x,&subWindow1_1y,&subWindow1_1w,&subWindow1_1h,x,y);
}

void mouseButton1_1(int button, int state, int x, int y) {
	// only start motion if the left button is pressed
	if (button == GLUT_LEFT_BUTTON) {

		// when the button is released
		if (state == GLUT_UP) {
			xOrigin = -1;
			yOrigin = -1;
			resizeUP=false;
			resizeDOWN=false;
			resizeLEFT=false;
			resizeRIGHT=false;
		}
		else  {// state = GLUT_DOWN
			if (x>=subWindow1_1w-10){
				resizeRIGHT=true;
			}else if(x<=10)  {
				resizeLEFT=true;
			}
			if (y>=subWindow1_1h-10){
				resizeDOWN=true;
			}else if(y<=10){
				resizeUP=true;
			}
		}
		}else if(GLUT_RIGHT_BUTTON){
	if (state == GLUT_UP) {
			xOrigin = -1;
			yOrigin = -1;
			moveWindow=false;
		}
		else  {// state = GLUT_DOWN
			xOrigin = x;
			yOrigin = y;
			moveWindow=true;
	}
	}
}

void mouseMove1_2(int x, int y) {
	// this will only be true when the left button is down
	if (xOrigin >= 0 && !resizeUP && !resizeDOWN && !resizeLEFT && !resizeRIGHT&& !moveWindow) {
		startPoint+=(x - xOrigin)*0.01;
		xOrigin = x;
	}
	subwindowResize(subWindow1_2,&subWindow1_2x,&subWindow1_2y,&subWindow1_2w,&subWindow1_2h,x,y);
}

void mouseButton1_2(int button, int state, int x, int y) {
	// only start motion if the left button is pressed
	if (button == GLUT_LEFT_BUTTON) {

		// when the button is released
		if (state == GLUT_UP) {
			xOrigin = -1;
			yOrigin = -1;
			resizeUP=false;
			resizeDOWN=false;
			resizeLEFT=false;
			resizeRIGHT=false;
		}else {// state = GLUT_DOWN
			xOrigin = x;
			yOrigin = y;
			if (x>=subWindow1_2w-10){
				resizeRIGHT=true;
			}else if(x<=10)  {
				resizeLEFT=true;
			}
			if (y>=subWindow1_2h-10){
				resizeDOWN=true;
			}else if(y<=10){
				resizeUP=true;
			}
		}
		}else if(GLUT_RIGHT_BUTTON){
	if (state == GLUT_UP) {
			xOrigin = -1;
			yOrigin = -1;
			moveWindow=false;
		}
		else  {// state = GLUT_DOWN
			xOrigin = x;
			yOrigin = y;
			moveWindow=true;
	}
	}
}

void mouseMove1_3(int x, int y) {
	// this will only be true when the left button is down
	subwindowResize(subWindow1_3,&subWindow1_3x,&subWindow1_3y,&subWindow1_3w,&subWindow1_3h,x,y);
}

void mouseButton1_3(int button, int state, int x, int y) {
	// only start motion if the left button is pressed
	if (button == GLUT_LEFT_BUTTON) {

		// when the button is released
		if (state == GLUT_UP) {
			xOrigin = -1;
			yOrigin = -1;
			resizeUP=false;
			resizeDOWN=false;
			resizeLEFT=false;
			resizeRIGHT=false;
			moveWindow=false;
		}
		else  {// state = GLUT_DOWN
			xOrigin = x;
			yOrigin = y;
			if (x>=subWindow1_3w-10){
				resizeRIGHT=true;
			}else if(x<=10)  {
				resizeLEFT=true;
			}
			if (y>=subWindow1_3h-10){
				resizeDOWN=true;
			}else if(y<=10){
				resizeUP=true;
			}
		}
	}else if(GLUT_RIGHT_BUTTON){
	if (state == GLUT_UP) {
			xOrigin = -1;
			yOrigin = -1;
			moveWindow=false;
		}
		else  {// state = GLUT_DOWN
			xOrigin = x;
			yOrigin = y;
			moveWindow=true;
	}
}
}


void mouseMove1_4(int x, int y) {
	subwindowResize(subWindow1_4,&subWindow1_4x,&subWindow1_4y,&subWindow1_4w,&subWindow1_4h,x,y);
}

void mouseButton1_4(int button, int state, int x, int y) {
	// only start motion if the left button is pressed
	if (button == GLUT_LEFT_BUTTON) {

		// when the button is released
		if (state == GLUT_UP) {
			xOrigin = -1;
			yOrigin = -1;
			resizeUP=false;
			resizeDOWN=false;
			resizeLEFT=false;
			resizeRIGHT=false;
		}
		else  {// state = GLUT_DOWN
			if (x>=subWindow1_4w-10){
				resizeRIGHT=true;
			}else if(x<=10)  {
				resizeLEFT=true;
			}
			if (y>=subWindow1_4h-10){
				resizeDOWN=true;
			}else if(y<=10){
				resizeUP=true;
			}
		}
		}else if(GLUT_RIGHT_BUTTON){
	if (state == GLUT_UP) {
			xOrigin = -1;
			yOrigin = -1;
			moveWindow=false;
		}
		else  {// state = GLUT_DOWN
			xOrigin = x;
			yOrigin = y;
			moveWindow=true;
	}
	}
}

void mouseMove1_5(int x, int y) {
	// this will only be true when the left button is down
	if (xOrigin >= 0 && !resizeUP && !resizeDOWN && !resizeLEFT && !resizeRIGHT&& !moveWindow) {
		startPoint+=(x - xOrigin)*0.01;
		xOrigin = x;
	}
	subwindowResize(subWindow1_5,&subWindow1_5x,&subWindow1_5y,&subWindow1_5w,&subWindow1_5h,x,y);
}

void mouseButton1_5(int button, int state, int x, int y) {
	// only start motion if the left button is pressed
	if (button == GLUT_LEFT_BUTTON) {

		// when the button is released
		if (state == GLUT_UP) {
			xOrigin = -1;
			yOrigin = -1;
			resizeUP=false;
			resizeDOWN=false;
			resizeLEFT=false;
			resizeRIGHT=false;
		}else {// state = GLUT_DOWN
			xOrigin = x;
			yOrigin = y;
			if (x>=subWindow1_5w-10){
				resizeRIGHT=true;
			}else if(x<=10)  {
				resizeLEFT=true;
			}
			if (y>=subWindow1_5h-10){
				resizeDOWN=true;
			}else if(y<=10){
				resizeUP=true;
			}
		}
		}else if(GLUT_RIGHT_BUTTON){
	if (state == GLUT_UP) {
			xOrigin = -1;
			yOrigin = -1;
			moveWindow=false;
		}
		else  {// state = GLUT_DOWN
			xOrigin = x;
			yOrigin = y;
			moveWindow=true;
	}
	}
}
void mouseMove1_6(int x, int y) {
if (yOrigin >= 0&& !resizeUP && !resizeDOWN && !resizeLEFT && !resizeRIGHT&& !moveWindow) {
		yStartPointTree+=(y - yOrigin)*0.01;
		yOrigin = y;
	}
	if (xOrigin >= 0&& !resizeUP && !resizeDOWN && !resizeLEFT && !resizeRIGHT&& !moveWindow) {
		xStartPointTree+=(x - xOrigin)*0.01;
		xOrigin = x;
	}
	subwindowResize(subWindow1_6,&subWindow1_6x,&subWindow1_6y,&subWindow1_6w,&subWindow1_6h,x,y);
}

void mouseButton1_6(int button, int state, int x, int y) {
	// only start motion if the left button is pressed
	if (button == GLUT_LEFT_BUTTON) {

		// when the button is released
		if (state == GLUT_UP) {
			xOrigin = -1;
			yOrigin = -1;
			resizeUP=false;
			resizeDOWN=false;
			resizeLEFT=false;
			resizeRIGHT=false;
		}else {// state = GLUT_DOWN
			xOrigin = x;
			yOrigin = y;
			if (x>=subWindow1_6w-10){
				resizeRIGHT=true;
			}else if(x<=10)  {
				resizeLEFT=true;
			}
			if (y>=subWindow1_6h-10){
				resizeDOWN=true;
			}else if(y<=10){
				resizeUP=true;
			}
		}
		}else if(GLUT_RIGHT_BUTTON){
	if (state == GLUT_UP) {
			xOrigin = -1;
			yOrigin = -1;
			moveWindow=false;
		}
		else  {// state = GLUT_DOWN
			xOrigin = x;
			yOrigin = y;
			moveWindow=true;
	}
	}
}

void mouseMove2_1(int x, int y) {
	subwindowResize(subWindow2_1,&subWindow2_1x,&subWindow2_1y,&subWindow2_1w,&subWindow2_1h,x,y);
}

void mouseButton2_1(int button, int state, int x, int y) {
	// only start motion if the left button is pressed
	if (button == GLUT_LEFT_BUTTON) {

		// when the button is released
		if (state == GLUT_UP) {
			xOrigin = -1;
			yOrigin = -1;
			resizeUP=false;
			resizeDOWN=false;
			resizeLEFT=false;
			resizeRIGHT=false;
		}
		else  {// state = GLUT_DOWN
			if (x>=subWindow2_1w-10){
				resizeRIGHT=true;
			}else if(x<=10)  {
				resizeLEFT=true;
			}
			if (y>=subWindow2_1h-10){
				resizeDOWN=true;
			}else if(y<=10){
				resizeUP=true;
			}
		}
		}else if(GLUT_RIGHT_BUTTON){
	if (state == GLUT_UP) {
			xOrigin = -1;
			yOrigin = -1;
			moveWindow=false;
		}
		else  {// state = GLUT_DOWN
			xOrigin = x;
			yOrigin = y;
			moveWindow=true;
	}
	}
}

void mouseMove2_2(int x, int y) {
	subwindowResize(subWindow2_2,&subWindow2_2x,&subWindow2_2y,&subWindow2_2w,&subWindow2_2h,x,y);
}

void mouseButton2_2(int button, int state, int x, int y) {
	// only start motion if the left button is pressed
	if (button == GLUT_LEFT_BUTTON) {

		// when the button is released
		if (state == GLUT_UP) {
			xOrigin = -1;
			yOrigin = -1;
			resizeUP=false;
			resizeDOWN=false;
			resizeLEFT=false;
			resizeRIGHT=false;
		}
		else  {// state = GLUT_DOWN
			if (x>=subWindow2_2w-10){
				resizeRIGHT=true;
			}else if(x<=10)  {
				resizeLEFT=true;
			}
			if (y>=subWindow2_2h-10){
				resizeDOWN=true;
			}else if(y<=10){
				resizeUP=true;
			}
		}
		}else if(GLUT_RIGHT_BUTTON){
	if (state == GLUT_UP) {
			xOrigin = -1;
			yOrigin = -1;
			moveWindow=false;
		}
		else  {// state = GLUT_DOWN
			xOrigin = x;
			yOrigin = y;
			moveWindow=true;
	}
	}
}

void mouseMove2_3(int x, int y) {
		if (xOrigin >= 0 && !resizeUP && !resizeDOWN && !resizeLEFT && !resizeRIGHT&& !moveWindow) {
		startPoint+=(x - xOrigin)*0.01;
		xOrigin = x;
	}
	subwindowResize(subWindow2_3,&subWindow2_3x,&subWindow2_3y,&subWindow2_3w,&subWindow2_3h,x,y);
}

void mouseButton2_3(int button, int state, int x, int y) {
	// only start motion if the left button is pressed
	if (button == GLUT_LEFT_BUTTON) {

		// when the button is released
		if (state == GLUT_UP) {
			xOrigin = -1;
			yOrigin = -1;
			resizeUP=false;
			resizeDOWN=false;
			resizeLEFT=false;
			resizeRIGHT=false;
		}
		else  {// state = GLUT_DOWN
			xOrigin = x;
			yOrigin = y;
			if (x>=subWindow2_3w-10){
				resizeRIGHT=true;
			}else if(x<=10)  {
				resizeLEFT=true;
			}
			if (y>=subWindow2_3h-10){
				resizeDOWN=true;
			}else if(y<=10){
				resizeUP=true;
			}
		}
		}else if(GLUT_RIGHT_BUTTON){
	if (state == GLUT_UP) {
			xOrigin = -1;
			yOrigin = -1;
			moveWindow=false;
		}
		else  {// state = GLUT_DOWN
			xOrigin = x;
			yOrigin = y;
			moveWindow=true;
	}
	}
}

void mouseMove2_4(int x, int y) {
		if (xOrigin >= 0 && !resizeUP && !resizeDOWN && !resizeLEFT && !resizeRIGHT&& !moveWindow) {
		startPoint+=(x - xOrigin)*0.01;
		xOrigin = x;
	}
	subwindowResize(subWindow2_4,&subWindow2_4x,&subWindow2_4y,&subWindow2_4w,&subWindow2_4h,x,y);
}

void mouseButton2_4(int button, int state, int x, int y) {
	// only start motion if the left button is pressed
	if (button == GLUT_LEFT_BUTTON) {

		// when the button is released
		if (state == GLUT_UP) {
			xOrigin = -1;
			yOrigin = -1;
			resizeUP=false;
			resizeDOWN=false;
			resizeLEFT=false;
			resizeRIGHT=false;
		}
		else  {// state = GLUT_DOWN
			xOrigin = x;
			yOrigin = y;
			if (x>=subWindow2_4w-10){
				resizeRIGHT=true;
			}else if(x<=10)  {
				resizeLEFT=true;
			}
			if (y>=subWindow2_4h-10){
				resizeDOWN=true;
			}else if(y<=10){
				resizeUP=true;
			}
		}
		}else if(GLUT_RIGHT_BUTTON){
	if (state == GLUT_UP) {
			xOrigin = -1;
			yOrigin = -1;
			moveWindow=false;
		}
		else  {// state = GLUT_DOWN
			xOrigin = x;
			yOrigin = y;
			moveWindow=true;
	}
	}
}

void mouseMove2_5(int x, int y) {
	subwindowResize(subWindow2_5,&subWindow2_5x,&subWindow2_5y,&subWindow2_5w,&subWindow2_5h,x,y);
}

void mouseButton2_5(int button, int state, int x, int y) {
	// only start motion if the left button is pressed
	if (button == GLUT_LEFT_BUTTON) {

		// when the button is released
		if (state == GLUT_UP) {
			xOrigin = -1;
			yOrigin = -1;
			resizeUP=false;
			resizeDOWN=false;
			resizeLEFT=false;
			resizeRIGHT=false;
		}
		else  {// state = GLUT_DOWN
			if (x>=subWindow2_5w-10){
				resizeRIGHT=true;
			}else if(x<=10)  {
				resizeLEFT=true;
			}
			if (y>=subWindow2_5h-10){
				resizeDOWN=true;
			}else if(y<=10){
				resizeUP=true;
			}
		}
		}else if(GLUT_RIGHT_BUTTON){
	if (state == GLUT_UP) {
			xOrigin = -1;
			yOrigin = -1;
			moveWindow=false;
		}
		else  {// state = GLUT_DOWN
			xOrigin = x;
			yOrigin = y;
			moveWindow=true;
	}
	}
}

void buttonsFunc(int index){
	if(index==0)
		scale/=2;
	else if(index==1)
		scale*=2;
	else if(index==2)
		speed*=2;
	else if(index==3)
		speed/=2;
	else if(index==4){
		t=0;
		t2=0;
	}else if(index==5)
		playOrStop=0;
	else if(index==6)
		playOrStop=1;
}

void mouseButtonC_1(int button, int state, int x, int y) {
	// only start motion if the left button is pressed
	double newX1=(double)6*x/25-12;
	double newY1=(double)-6*y/25+12;
	if (button == GLUT_LEFT_BUTTON) {

		// when the button is released
		if (state == GLUT_UP) {
		for (int i=0;i<7;i++)
		{
			if(newX1>=-10+4*(i%4)&&newX1<=-10+4*(i%4)+3&&newY1>=7-(i/4)*5&&newY1<=7-(i/4)*5+4&&buttons[i]==true)
				buttonsFunc(i);	
		}
		for(int i=0;i<7;i++)
			buttons[i]=false;
		}
		else  {// state = GLUT_DOWN
		for (int i=0;i<7;i++)
		{
			if(newX1>=-10+4*(i%4)&&newX1<=-10+4*(i%4)+3&&newY1>=7-(i/4)*5&&newY1<=7-(i/4)*5+4)
				buttons[i]=true;	
		}
		}
	}
}

void buttonsFunc2(int index){
	int window;
	if(index==0)
		window=subWindow1_1;
	else if(index==1)
		window=subWindow1_2;
	else if(index==2)
		window=subWindow1_3;
	else if(index==3)
		window=subWindow1_4;
	else if(index==4)
		window=subWindow1_5;
	else if(index==5)
		window=subWindow1_6;
	else if(index==6 &&displayDll)
		window=subWindow2_1;
	else if(index==7 &&displayDll)
		window=subWindow2_2;
	else if(index==8&&displayDll)
		window=subWindow2_3;
	else if(index==9&&displayDll)
		window=subWindow2_4;
	else if(index==10&&displayDll)
		window=subWindow2_5;			
	glutSetWindow(window);
	if (windowState[index]==true&&(displayDll||index<6))
		glutHideWindow();
	else if (displayDll||index<6)
		glutShowWindow();
	windowState[index]=!windowState[index];
}


void mouseButtonC_2(int button, int state, int x, int y) {
	// only start motion if the left button is pressed
	double newX1=(double)6*x/25-12;
	double newY1=(double)-6*y/25+12;
	if (button == GLUT_LEFT_BUTTON) {

		// when the button is released
		if (state == GLUT_UP) {
		for (int i=0;i<11;i++)
		{
			if(newX1>=-10&&newX1<=-10+20&&newY1>=10-i*2&&newY1<=10-i*2+1&&hidebuttons[i]==true)
				buttonsFunc2(i);	
		}
		for(int i=0;i<11;i++)
			hidebuttons[i]=false;
		}
		else  {// state = GLUT_DOWN
		for (int i=0;i<11;i++)
		{
			if(newX1>=-10&&newX1<=-10+20&&newY1>=10-i*2&&newY1<=10-i*2+1)
				hidebuttons[i]=true;	
		}
		}
	}
}

//------------------------------------------------------------	main()
//
int main(int argc,char** argv) {
	if((argc>1)&&(strcmp(argv[1],"0")==0))
  {
   
  } else
  {
		displayDll=false;
    subWindow1_2w=645;
    subWindow1_2h=300;//per cores graph
    subWindow1_5x=645;
    subWindow1_5y=300;
    subWindow1_5w=645;
    subWindow1_5h=300;//all cores graph
    subWindow1_6w=840;
	}
	// initialise glut
	glutInit(&argc,argv);

	// request a depth buffer, RGBA display mode, and we want double buffering
	glutInitDisplayMode(GLUT_DEPTH|GLUT_RGBA|GLUT_DOUBLE);

	// set the initial window size
	glutInitWindowSize(1290,600);

	// create the window
	window_1=glutCreateWindow("A basic primitive example");

	// set the function to use to draw our scene
	glutDisplayFunc(OnDraw);

	// set the function to handle changes in screen size
	glutReshapeFunc(OnReshape);
	// run our custom initialisation
	OnInit();
	subWindow1_1=glutCreateSubWindow(window_1, subWindow1_1x,subWindow1_1y,subWindow1_1w,subWindow1_1h);
	glutDisplayFunc(OnDraw1_1);
	glutReshapeFunc(OnReshape2);
	glutMouseFunc(mouseButton1_1);
	glutMotionFunc(mouseMove1_1);
	OnInit1_1();
	subWindow1_2=glutCreateSubWindow(window_1, subWindow1_2x,subWindow1_2y,subWindow1_2w,subWindow1_2h);
		glutDisplayFunc(OnDraw1_2);
		glutReshapeFunc(OnReshape2);
		glutMouseFunc(mouseButton1_2);
		glutMotionFunc(mouseMove1_2);
		OnInit1_2();
	subWindow1_3=glutCreateSubWindow(window_1, subWindow1_3x,subWindow1_3y,subWindow1_3w,subWindow1_3h);
	// set the function to use to draw our scene
	glutDisplayFunc(OnDraw1_3);
	// set the function to handle changes in screen size
	glutReshapeFunc(OnReshape2);
		glutMouseFunc(mouseButton1_3);
	glutMotionFunc(mouseMove1_3);
	// run our custom initialisation

	OnInit1_2();
	subWindow1_4=glutCreateSubWindow(window_1,subWindow1_4x,subWindow1_4y,subWindow1_4w,subWindow1_4h);
		glutDisplayFunc(OnDraw1_4);
		glutReshapeFunc(OnReshape2);
			glutMouseFunc(mouseButton1_4);
	glutMotionFunc(mouseMove1_4);
		OnInit1_2();
	
	subWindow1_5=glutCreateSubWindow(window_1,subWindow1_5x,subWindow1_5y,subWindow1_5w,subWindow1_5h);
		glutDisplayFunc(OnDraw1_5);
		glutReshapeFunc(OnReshape2);
		glutMouseFunc(mouseButton1_5);
		glutMotionFunc(mouseMove1_5);
		OnInit1_2();
	subWindow1_6=glutCreateSubWindow(window_1,subWindow1_6x,subWindow1_6y,subWindow1_6w,subWindow1_6h);
		glutDisplayFunc(OnDraw1_6);
		glutReshapeFunc(OnReshape2);
		glutMouseFunc(mouseButton1_6);
		glutMotionFunc(mouseMove1_6);
		OnInit1_2();
		if(displayDll){
		subWindow2_1=glutCreateSubWindow(window_1,subWindow2_1x,subWindow2_1y,subWindow2_1w,subWindow2_1h);
	// set the function to use to draw our scene
	glutDisplayFunc(OnDraw2_1);
	// set the function to handle changes in screen size
	glutReshapeFunc(OnReshape2);
			glutMouseFunc(mouseButton2_1);
		glutMotionFunc(mouseMove2_1);
	// run our custom initialisation

	OnInit2();
			subWindow2_2=glutCreateSubWindow(window_1,subWindow2_2x,subWindow2_2y,subWindow2_2w,subWindow2_2h);
	// set the function to use to draw our scene
	glutDisplayFunc(OnDraw2_2);
	// set the function to handle changes in screen size
	glutReshapeFunc(OnReshape2);
				glutMouseFunc(mouseButton2_2);
		glutMotionFunc(mouseMove2_2);
	// run our custom initialisation

	OnInit1_2();
	
				subWindow2_3=glutCreateSubWindow(window_1,subWindow2_3x,subWindow2_3y,subWindow2_3w,subWindow2_3h);
	// set the function to use to draw our scene
	glutDisplayFunc(OnDraw2_3);
	// set the function to handle changes in screen size
	glutReshapeFunc(OnReshape2);
				glutMouseFunc(mouseButton2_3);
		glutMotionFunc(mouseMove2_3);
	// run our custom initialisation

	OnInit1_2();
	
	subWindow2_4=glutCreateSubWindow(window_1,subWindow2_4x,subWindow2_4y,subWindow2_4w,subWindow2_4h);
	// set the function to use to draw our scene
	glutDisplayFunc(OnDraw2_4);
	// set the function to handle changes in screen size
	glutReshapeFunc(OnReshape2);
				glutMouseFunc(mouseButton2_4);
		glutMotionFunc(mouseMove2_4);
	// run our custom initialisation

	OnInit1_2();
	
		subWindow2_5=glutCreateSubWindow(window_1,subWindow2_5x,subWindow2_5y,subWindow2_5w,subWindow2_5h);
	// set the function to use to draw our scene
	glutDisplayFunc(OnDraw2_5);
	// set the function to handle changes in screen size
	glutReshapeFunc(OnReshape2);
				glutMouseFunc(mouseButton2_5);
		glutMotionFunc(mouseMove2_5);
	// run our custom initialisation

	OnInit1_2();
	}
	glutInitWindowSize(200,100);
	window_C=glutCreateWindow("A basic primitive example2");
	// set the function to use to draw our scene
	glutDisplayFunc(OnDrawC);
	// set the function to handle changes in screen size
	glutReshapeFunc(OnReshape2);
	// run our custom initialisation
		subWindowC_1=glutCreateSubWindow(window_C,0,0,100,100);
	glutDisplayFunc(OnDrawC_1);
	glutReshapeFunc(OnReshape2);
	glutMouseFunc(mouseButtonC_1);
	OnInitC_1();
	subWindowC_2=glutCreateSubWindow(window_C,100,0,100,100);
	glutDisplayFunc(OnDrawC_2);
	glutReshapeFunc(OnReshape2);
	glutMouseFunc(mouseButtonC_2);
	OnInitC_2();
	
	
	// set the function to be called when we exit
	atexit(OnExit);


	// this function runs a while loop to keep the program running.
	glutMainLoop();
	return 0;
}
