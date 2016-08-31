#include "io_handle.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <e-hal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h> /* mmap() is defined in this header */
#include <fcntl.h>


int read_name(int input_fd , char * res)
  {
     int i=0;
     read (input_fd, &(res[i]),1);
     if (res[i]==EOF) return -1;
     while(res[i]!=' ')
     {
        i++;
        read (input_fd, &(res[i]),1);
        if(res[i]==EOF)return -1;
     }
     res[i]='\0';
     //fprintf(stderr,"name=%s\n",res);
  }
int read_address(int input_fd , unsigned * res)
  {
    int i=0;
    unsigned char buf[10];
    read (input_fd, &(buf[i]),1);
    if(res[i]==EOF)return -1;
    while(buf[i]!=' ')
     {
       if ((buf[i]<='9')&&(buf[i]>='0')){(*res)=(*res)*16+(buf[i]-'0');}
       else if ((buf[i]<='f')&&(buf[i]>='a')){(*res)=(*res)*16+(buf[i]-'a'+10);}
       ////fprintf(stderr,"res=%x\n",*res);
       i++;
       read (input_fd, &(buf[i]),1);
       if(res[i]==EOF)return -1;
     }
    buf[i]='\0';
    //fprintf(stderr,"address=%s ",buf);
    //*res= (int)strtol(buf,NULL, 16);
    //fprintf(stderr,"res=%x\n",*res);
  }
int read_size(int input_fd , unsigned * res)
  {
    int i=0;
    unsigned char  buf[10];
     *res=0;
    read (input_fd, &(buf[i]),1);
    if(res[i]==EOF)return -1;
       while(buf[i]!='\n')
       {
        if ((buf[i]<='9')&&(buf[i]>='0')){(*res)=(*res)*16+(buf[i]-'0');}
        else if ((buf[i]<='f')&&(buf[i]>='a')){(*res)=(*res)*16+(buf[i]-'a'+10);}
       // //fprintf(stderr,"res=%x\n",*res);
       i++;
          read (input_fd, &(buf[i]),1);
          if(res[i]==EOF)return -1;
       }
       buf[i]='\0';
        //fprintf(stderr,"size=%s ",buf);
        //  *res= (int)strtol(buf, NULL, 16);
        //fprintf(stderr,"res=%x\n",*res);
  }
int handle_output(char * lable)
{
	static int version=0;
  	e_mem_t emem;
  char emsg[0x4000];
int input_fd,output_fd;
char name[20];
char pathname[60];
unsigned addr,size;
if (lable==NULL) return 0;
 int memfd = open("/dev/mem", O_RDWR | O_SYNC);
        char *pmem = mmap((void *)0x8e000000, 32*1024*1024, PROT_READ|PROT_WRITE, MAP_SHARED,memfd, 0x3e000000);
 input_fd = open ("dumpTable.txt", O_RDONLY);
    if (input_fd == -1) {
           fprintf(stderr,"dumpTable.txt open failed");
            return 2;
    }
    int fres=0;
    while(fres!=-1)
    {
    fres=read_name(input_fd,name);
    if (fres==-1)break;
    fres=read_address(input_fd,&addr);
    if (fres==-1)break;
    fres=read_size(input_fd,&size);
    if (fres==-1)break;
     //fprintf(stderr,"\nopening output file");
     snprintf(pathname,60, "%s%03d_%s_%s.txt","..//dump//",version,name,lable);
     //fprintf(stderr," %s\n",pathname);
      output_fd = open(pathname, O_WRONLY | O_CREAT , 0666);
    if(output_fd == -1){
          fprintf(stderr,"%s open failed",pathname);
        return 3;
        }
    //fprintf(stderr,"\ndumping data :\n");
        char* add;
        unsigned char ch;
    for (add=(char *)addr;add<((char *)(size+addr));add++)    
      {
      ch=*add;
       // //fprintf(stderr,"%x = %d = |%c|\n",add,ch,ch);
        write(output_fd,&ch,1);
      }
    close(output_fd);
    }
    close(input_fd);
       close(memfd);
    //fprintf(stderr,"output %d_%s ended OK",version,lable);
    version++;
        return 0;
}

int handle_input(char * lable)
{
		e_mem_t emem;
  char emsg[0x4000];
int input_fd,output_fd;
char name[20];
char pathname[60];
unsigned addr,size;
if (lable==NULL) return 0;
 int memfd = open("/dev/mem", O_RDWR | O_SYNC);
        char *pmem = mmap((void *)0x8e000000, 32*1024*1024, PROT_READ|PROT_WRITE, MAP_SHARED,memfd, 0x3e000000);
 input_fd = open ("inputTable.txt", O_RDONLY);
    if (input_fd == -1) {
           fprintf(stderr,"inputTable.txt open failed");
            return 2;
    }
    int fres=0;
    while(fres!=-1)
    {
    fres=read_name(input_fd,name);
    if (fres==-1)break;
    fres=read_address(input_fd,&addr);
    if (fres==-1)break;
    fres=read_size(input_fd,&size);
    if (fres==-1)break;
     fprintf(stderr,"\nopening input file");
     snprintf(pathname,60, "%s%s_%s.txt","..//input//",name,lable);
     fprintf(stderr," %s\n",pathname);
      output_fd = open(pathname,O_RDONLY);
    if(output_fd == -1){
          fprintf(stderr,"%s open failed",pathname);
        return 3;
        }
    //fprintf(stderr,"\n loading data :\n");
        char* add;
        unsigned char ch;
        add=(char *)addr;
        
     while(0<read(output_fd,&ch,1))
      {
        ////fprintf(stderr,"%x = %d = |%c|\n",add,ch,ch);
        *add=ch;
        add++;  
      }
    close(output_fd);
    }
    close(input_fd);
    fprintf(stderr,"%s ended OK",lable);
        return 0;
    
}