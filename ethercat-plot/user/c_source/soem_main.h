#ifndef __SOEM_MAIN_H__
#define __SOEM_MAIN_H__

#include <stdbool.h>
#include "my_queue.h"

#ifdef __cplusplus
extern "C"
{
#endif

/***variate***/
typedef struct{
    struct queueLK my_ecQueue_ch1;
    struct queueLK my_ecQueue_ch2;
    struct queueLK my_ecQueue_ch3;
    struct queueLK my_ecQueue_ch4;
}my_ec_data_Q_t;

//由于windows网络定义有冲突，仿照ec_adapter定义一个不同名字的结构体
#define EC_MAXLEN_ADAPTERNAME_win 128
typedef struct ec_adapter_win ec_adaptert_win;
struct ec_adapter_win
{
   char   name[EC_MAXLEN_ADAPTERNAME_win];
   char   desc[EC_MAXLEN_ADAPTERNAME_win];
   ec_adaptert_win *next;
};

bool Is_ecRun;
bool Is_ecPrintToConsole;
my_ec_data_Q_t my_ecQueue;

int my_CMD[4];

/***variate end***/

/***functions***/
//int soem_main(int argc, char *argv[]);
int my_soem_main(char * adaptert_name,int cycle_count,bool default_count);
void my_ec_test(char *ifname);
int my_ec_initCheckThread(void);

ec_adaptert_win* print_ec_adaptert(void);
int print_ec_adaptert_free(ec_adaptert_win * adapter);

int my_ec_slaveinfo(char *ifname,bool is_printSDO,bool is_printMAP);
/***functions end***/

#ifdef __cplusplus
}
#endif

#endif
