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
    struct queueLK my_ecQueue_x;
    struct queueLK my_ecQueue_y;
    struct queueLK my_ecQueue_z;
}my_ec_data_Q_t;

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

int my_ec_slaveinfo(char *ifname,bool is_printSDO,bool is_printMAP);
/***functions end***/

#ifdef __cplusplus
}
#endif

#endif
