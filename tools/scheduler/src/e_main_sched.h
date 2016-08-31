//#define SECTION(x)  __attribute__ ((section (x)))
#define SetQuota(task,num) SetQuota_(#task,num);
#define SetGroup(task1,task2,num1,num2) SetGroup_(#task1,#task2,num1,num2);
//int volatile usermem[1024] SECTION("shared_dram");
//int main(void);
inline int SetQuota_(char *funcName,int num);
inline int SetGroup_(char *funcName1, char *funcName2, int num1, int num2);
inline int RetValue(unsigned char  ret);

//inline int RetTrue();
//inline int RetFalse();
inline int TaskInst();
inline int SetCoreNum(unsigned char num);
//remember to do here set queta!