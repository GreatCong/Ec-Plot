/*
 * Usage : simple_test [ifname1]
 * ifname is NIC interface, f.e. eth0
 *
 * 2018-5-22 by lcj
 */

#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include "ethercat.h"
#include <stdlib.h> //atoi function

#ifdef _MSC_VER //windows unistd instead
#include <io.h>
#include <process.h>
#else
#include <unistd.h>//sleep 1s
#endif

#include "soem_main.h"

#define EC_TIMEOUTMON 500

char IOmap[4096];
OSAL_THREAD_HANDLE thread1;
int expectedWKC;
boolean needlf;
volatile int wkc;
boolean inOP;
uint8 currentgroup = 0;//default 0, 0 = all groups

int cycle_num = 1000;
OSAL_THREAD_HANDLE my_input_handle;
// int input_char = 0;
//int my_CMD[4] = {0};

/********************** my device setting **********************************/

/* Master 0, Slave 0, "DRE2004"
 * Vendor ID:       0x10533004
 * Product code:    0x00000002
 * Revision number: 0x00000001
 */
typedef struct
{
    int16_t CH1;
    int16_t CH2;
    int16_t CH3;
    int16_t CH4;
}My_device_out;//AD 4 channels

typedef struct
{
  int16_t CMD1;
  int16_t CMD2;
  int16_t CMD3;
  int16_t CMD4;
}My_device_in;//CMD 4 channels

/******************* my device setting end ********************************/


void simpletest(char *ifname)
{
    int i, cnt, oloop, iloop, chk;
    needlf = FALSE;
    inOP = FALSE;

   printf("Starting simple test\n");

   /* initialise SOEM, bind socket to ifname */
   if (ec_init(ifname))
   {
      printf("ec_init on %s succeeded.\n",ifname);
      /* find and auto-config slaves */


       if ( ec_config_init(FALSE) > 0 )
      {
         printf("%d slaves found and configured.\n",ec_slavecount);

         ec_config_map(&IOmap);

         ec_configdc();

         printf("Slaves mapped, state to SAFE_OP.\n");
         /* wait for all slaves to reach SAFE_OP state */
         ec_statecheck(0, EC_STATE_SAFE_OP,  EC_TIMEOUTSTATE * 4);

         oloop = ec_slave[0].Obytes;
         if ((oloop == 0) && (ec_slave[0].Obits > 0)) oloop = 1;
         if (oloop > 8) oloop = 8;
         iloop = ec_slave[0].Ibytes;
         if ((iloop == 0) && (ec_slave[0].Ibits > 0)) iloop = 1;
         if (iloop > 8) iloop = 8;

         printf("oloop=%d,iloop=%d\n",ec_slave[0].Obytes,ec_slave[0].Ibytes );

         /* read indevidual slave state and store in ec_slave[] */
         ec_readstate();
         //ec_slave[0]表示master节点，slaves从ec_slave[1]开始的，slaves[0]的地址与slave[1]的地址相同，IBytes和OBytes为所有slave的总和
         for(cnt = 1; cnt <= ec_slavecount ; cnt++)//read some message
         {
            printf("Slave:%d Name:%s VID:0x%x,PID:0x%x\n", cnt, ec_slave[cnt].name,ec_slave[cnt].eep_man,ec_slave[cnt].eep_id);
            printf("Slave:%d Output size:%3dbits Input size:%3dbits State:%2d delay:%d.%d\n",
                  cnt, ec_slave[cnt].Obits, ec_slave[cnt].Ibits,
                  ec_slave[cnt].state, (int)ec_slave[cnt].pdelay, ec_slave[cnt].hasdc);
            printf("         Out:%8.8x,%4d In:%8.8x,%4d\n",
                  (int)ec_slave[cnt].outputs, ec_slave[cnt].Obytes, (int)ec_slave[cnt].inputs, ec_slave[cnt].Ibytes);
            /* check for EL2004 or EL2008 */
            // if( !digout && ((ec_slave[cnt].eep_id == 0x0af83052) || (ec_slave[cnt].eep_id == 0x07d83052)))
            // {
            //    digout = ec_slave[cnt].outputs;
            // }
         }
         // bind data to the logic addr
         My_device_out *out_values;
         My_device_in *in_values;
         // out_values = (My_device_out *)ec_slave[0].inputs;//ec_slave[0] --> master
         // in_values = (My_device_in *)ec_slave[0].outputs;
         out_values = (My_device_out *)ec_slave[1].inputs;
         in_values = (My_device_in *)ec_slave[1].outputs;
         in_values->CMD1 = 0;
         in_values->CMD2 = 0;
         in_values->CMD3 = 0;
         in_values->CMD4 = 0;

         printf("segments : %d : %d %d %d %d\n",ec_group[0].nsegments ,ec_group[0].IOsegment[0],ec_group[0].IOsegment[1],ec_group[0].IOsegment[2],ec_group[0].IOsegment[3]);

         printf("Request operational state for all slaves\n");
         expectedWKC = (ec_group[0].outputsWKC * 2) + ec_group[0].inputsWKC;
         printf("Calculated workcounter %d\n", expectedWKC);
         ec_slave[0].state = EC_STATE_OPERATIONAL;
         /* send one valid process data to make outputs in slaves happy*/
         ec_send_processdata();
         ec_receive_processdata(EC_TIMEOUTRET);
         /* request OP state for all slaves */
         ec_writestate(0);
         chk = 40;
         /* wait for all slaves to reach OP state */
         do
         {
            ec_send_processdata();
            ec_receive_processdata(EC_TIMEOUTRET);
            ec_statecheck(0, EC_STATE_OPERATIONAL, 50000);
         }while (chk-- && (ec_slave[0].state != EC_STATE_OPERATIONAL));

         if (ec_slave[0].state == EC_STATE_OPERATIONAL )
         {
            printf("Operational state reached for all slaves.\n");
            inOP = TRUE;
                /* cyclic loop */
            // int cycle_num = 1000;

            for(i = 1; i <= cycle_num; i++)
            {
               ec_send_processdata();
               wkc = ec_receive_processdata(EC_TIMEOUTRET);

                    if(wkc >= expectedWKC)
                    {
                        // printf("Processdata cycle %4d, WKC %d , O:", i, wkc);

                        // for(j = 0 ; j < oloop; j++)
                        // {
                        //     printf(" %2.2x", *(ec_slave[0].outputs + j));
                        // }

                        // printf(" I:");
                        // for(j = 0 ; j < iloop; j++)
                        // {
                        //     printf(" %2.2x", *(ec_slave[0].inputs + j));
                        // }

                        in_values->CMD2 = my_CMD[1];
                        printf("Processdata cycle %4d, WKC %d", i, wkc);
                        printf(" CMD2:%d,CH1 %5d,CH2 %5d,CH3 %5d, CH4 %5d",my_CMD[1],out_values->CH1, out_values->CH2,out_values->CH3,out_values->CH4);

                        printf(" T:%"PRId64"\r",ec_DCtime);
                        needlf = TRUE;
                    }
                    osal_usleep(5000);

                }//for
                inOP = FALSE;
            }// if EC_STATE_OPERATIONAL
            else
            {
                printf("Not all slaves reached operational state.\n");
                ec_readstate();
                for(i = 1; i<=ec_slavecount ; i++)
                {
                    if(ec_slave[i].state != EC_STATE_OPERATIONAL)
                    {
                        printf("Slave %d State=0x%2.2x StatusCode=0x%4.4x : %s\n",
                            i, ec_slave[i].state, ec_slave[i].ALstatuscode, ec_ALstatuscode2string(ec_slave[i].ALstatuscode));
                    }
                }
            }
            printf("\nRequest init state for all slaves\n");
            ec_slave[0].state = EC_STATE_INIT;
            /* request INIT state for all slaves */
            ec_writestate(0);
        }//if ec_config_init
        else
        {
            printf("No slaves found!\n");
        }
        printf("End simple test, close socket\n");
        /* stop SOEM, close socket */
        ec_close();
    }
    else
    {
        printf("No socket connection on %s\nExcecute as root\n",ifname);
    }
}

OSAL_THREAD_FUNC ecatcheck( void *ptr )
{
    int slave;
    (void)ptr;                  /* Not used */

    while(1)
    {
        if( inOP && ((wkc < expectedWKC) || ec_group[currentgroup].docheckstate))
        {
            if (needlf)
            {
               needlf = FALSE;
               printf("\n");
            }
            /* one ore more slaves are not responding */
            ec_group[currentgroup].docheckstate = FALSE;
            ec_readstate();
            for (slave = 1; slave <= ec_slavecount; slave++)
            {
               if ((ec_slave[slave].group == currentgroup) && (ec_slave[slave].state != EC_STATE_OPERATIONAL))
               {
                  ec_group[currentgroup].docheckstate = TRUE;
                  if (ec_slave[slave].state == (EC_STATE_SAFE_OP + EC_STATE_ERROR))
                  {
                     printf("ERROR : slave %d is in SAFE_OP + ERROR, attempting ack.\n", slave);
                     ec_slave[slave].state = (EC_STATE_SAFE_OP + EC_STATE_ACK);
                     ec_writestate(slave);
                  }
                  else if(ec_slave[slave].state == EC_STATE_SAFE_OP)
                  {
                     printf("WARNING : slave %d is in SAFE_OP, change to OPERATIONAL.\n", slave);
                     ec_slave[slave].state = EC_STATE_OPERATIONAL;
                     ec_writestate(slave);
                  }
                  else if(ec_slave[slave].state > EC_STATE_NONE)
                  {
                     if (ec_reconfig_slave(slave, EC_TIMEOUTMON))
                     {
                        ec_slave[slave].islost = FALSE;
                        printf("MESSAGE : slave %d reconfigured\n",slave);
                     }
                  }
                  else if(!ec_slave[slave].islost)
                  {
                     /* re-check state */
                     ec_statecheck(slave, EC_STATE_OPERATIONAL, EC_TIMEOUTRET);
                     if (ec_slave[slave].state == EC_STATE_NONE)
                     {
                        ec_slave[slave].islost = TRUE;
                        printf("ERROR : slave %d lost\n",slave);
                     }
                  }
               }
               if (ec_slave[slave].islost)
               {
                  if(ec_slave[slave].state == EC_STATE_NONE)
                  {
                     if (ec_recover_slave(slave, EC_TIMEOUTMON))
                     {
                        ec_slave[slave].islost = FALSE;
                        printf("MESSAGE : slave %d recovered\n",slave);
                     }
                  }
                  else
                  {
                     ec_slave[slave].islost = FALSE;
                     printf("MESSAGE : slave %d found\n",slave);
                  }
               }
            }
            if(!ec_group[currentgroup].docheckstate)
               printf("OK : all slaves resumed OPERATIONAL.\n");
        }
        osal_usleep(10000);
    }
}

OSAL_THREAD_FUNC input_task( void *ptr ){ //for input
  (void)ptr;                  /* Not used */

   while(1){
    if(inOP){

      int input_char = getchar();
      //printf("input = %c(%d)\n", input_char,input_char);
      switch(input_char){
        case '0':
              my_CMD[1] = 0;
        break;
        case '1':
              my_CMD[1] = 1;
        break;
        case '2':
              my_CMD[1] = 2;
        break;
        case '3':
              my_CMD[1] = 3;
        break;
        default:
              //in_values->CMD2 = 0;
        break;

      }
      fflush(stdin);
    }

#ifdef _MSC_VER
    Sleep(1000);
#else
      sleep(1);//int linux,delay is 1s,while windows is 1ms
    #endif
   }
}

#if 0
int main(int argc, char *argv[])
{
  ec_adaptert * adapter = NULL;

   printf("SOEM (Simple Open EtherCAT Master)\nSimple test\n");


   if (argc > 1)
   {
      /* create thread to handle slave error handling in OP */
//      pthread_create( &thread1, NULL, (void *) &ecatcheck, (void*) &ctime);
      osal_thread_create(&thread1, 128000, &ecatcheck, (void*) &ctime);//这个函数在C++中会报错，原因是C++的void指针不能强制赋值

      osal_thread_create(&my_input_handle, 128000, &input_task, NULL);
      /* start cyclic part */
      if(argc == 3){
        cycle_num = atoi(argv[2]);
      }

      simpletest(argv[1]);
   }
   else
   {
      printf("Usage: simple_test ifname1\nifname = eth0 for example\n");

      printf ("Available adapters\n");
      adapter = ec_find_adapters ();
      while (adapter != NULL)
      {
         printf ("Description : %s, Device to use for wpcap: %s\n", adapter->desc,adapter->name);
         adapter = adapter->next;
      }
   }

   printf("End program\n");
   return (0);
}
#endif

int my_soem_main(char * adaptert_name,int cycle_count,bool default_count){
//    printf("SOEM (Simple Open EtherCAT Master)\nSimple test\n");

    osal_thread_create(&thread1, 128000, &ecatcheck, (void*) &ctime);//这个函数在C++中会报错，原因是C++的void指针不能强制赋值

    osal_thread_create(&my_input_handle, 128000, &input_task, NULL);

    if(default_count)  cycle_num = cycle_count;

    simpletest(adaptert_name);

//    printf("End program\n");

    return 0;
}

//bool Is_my_ecRun;
// bind data to the logic addr
My_device_out *out_values;
My_device_in *in_values;

int my_ec_initCheckThread(void){
    osal_thread_create(&thread1, 128000, &ecatcheck, (void*) &ctime);//这个函数在C++中会报错，原因是C++的void指针不能强制赋值

    return 0;
}

int my_ec_process_hander(void){
    in_values->CMD1 = my_CMD[0];
    in_values->CMD2 = my_CMD[1];
    in_values->CMD3 = my_CMD[2];
    in_values->CMD4 = my_CMD[3];
    if(Is_ecPrintToConsole){
      printf(" CMD2:%d,CH1 %5d,CH2 %5d,CH3 %5d, CH4 %5d",my_CMD[1],out_values->CH1, out_values->CH2,out_values->CH3,out_values->CH4);

      printf("\r");
//    printf(" T:%"PRId64"\r",ec_DCtime);
    }

    enQueue(&my_ecQueue.my_ecQueue_ch1, out_values->CH1);
    enQueue(&my_ecQueue.my_ecQueue_ch2, out_values->CH2);
    enQueue(&my_ecQueue.my_ecQueue_ch3, out_values->CH3);
    enQueue(&my_ecQueue.my_ecQueue_ch4, out_values->CH4);
    return 0;
}

void my_ec_test(char *ifname)
{
    int i, cnt, oloop, iloop, chk;
    needlf = FALSE;
    inOP = FALSE;

   printf("Starting simple test\n");

   /* initialise SOEM, bind socket to ifname */
   if (ec_init(ifname))
   {
      printf("ec_init on %s succeeded.\n",ifname);
      /* find and auto-config slaves */


       if ( ec_config_init(FALSE) > 0 )
      {
         printf("%d slaves found and configured.\n",ec_slavecount);

         ec_config_map(&IOmap);

         ec_configdc();

         printf("Slaves mapped, state to SAFE_OP.\n");
         /* wait for all slaves to reach SAFE_OP state */
         ec_statecheck(0, EC_STATE_SAFE_OP,  EC_TIMEOUTSTATE * 4);

         oloop = ec_slave[0].Obytes;
         if ((oloop == 0) && (ec_slave[0].Obits > 0)) oloop = 1;
         if (oloop > 8) oloop = 8;
         iloop = ec_slave[0].Ibytes;
         if ((iloop == 0) && (ec_slave[0].Ibits > 0)) iloop = 1;
         if (iloop > 8) iloop = 8;

         printf("oloop=%d,iloop=%d\n",ec_slave[0].Obytes,ec_slave[0].Ibytes );

         /* read indevidual slave state and store in ec_slave[] */
         ec_readstate();
         //ec_slave[0]表示master节点，slaves从ec_slave[1]开始的，slaves[0]的地址与slave[1]的地址相同，IBytes和OBytes为所有slave的总和
         for(cnt = 1; cnt <= ec_slavecount ; cnt++)//read some message
         {
            printf("Slave:%d Name:%s VID:0x%x,PID:0x%x\n", cnt, ec_slave[cnt].name,ec_slave[cnt].eep_man,ec_slave[cnt].eep_id);
            printf("Slave:%d Output size:%3dbits Input size:%3dbits State:%2d delay:%d.%d\n",
                  cnt, ec_slave[cnt].Obits, ec_slave[cnt].Ibits,
                  ec_slave[cnt].state, (int)ec_slave[cnt].pdelay, ec_slave[cnt].hasdc);
            printf("         Out:%8.8x,%4d In:%8.8x,%4d\n",
                  (int)ec_slave[cnt].outputs, ec_slave[cnt].Obytes, (int)ec_slave[cnt].inputs, ec_slave[cnt].Ibytes);
            /* check for EL2004 or EL2008 */
            // if( !digout && ((ec_slave[cnt].eep_id == 0x0af83052) || (ec_slave[cnt].eep_id == 0x07d83052)))
            // {
            //    digout = ec_slave[cnt].outputs;
            // }
         }
//         // bind data to the logic addr
//         My_device_out *out_values;
//         My_device_in *in_values;
         // out_values = (My_device_out *)ec_slave[0].inputs;//ec_slave[0] --> master
         // in_values = (My_device_in *)ec_slave[0].outputs;
         out_values = (My_device_out *)ec_slave[1].inputs;
         in_values = (My_device_in *)ec_slave[1].outputs;
         in_values->CMD1 = 0;
         in_values->CMD2 = 0;
         in_values->CMD3 = 0;
         in_values->CMD4 = 0;

         printf("segments : %d : %d %d %d %d\n",ec_group[0].nsegments ,ec_group[0].IOsegment[0],ec_group[0].IOsegment[1],ec_group[0].IOsegment[2],ec_group[0].IOsegment[3]);

         printf("Request operational state for all slaves\n");
         expectedWKC = (ec_group[0].outputsWKC * 2) + ec_group[0].inputsWKC;
         printf("Calculated workcounter %d\n", expectedWKC);
         ec_slave[0].state = EC_STATE_OPERATIONAL;
         /* send one valid process data to make outputs in slaves happy*/
         ec_send_processdata();
         ec_receive_processdata(EC_TIMEOUTRET);
         /* request OP state for all slaves */
         ec_writestate(0);
         chk = 40;
         /* wait for all slaves to reach OP state */
         do
         {
            ec_send_processdata();
            ec_receive_processdata(EC_TIMEOUTRET);
            ec_statecheck(0, EC_STATE_OPERATIONAL, 50000);
         }while (chk-- && (ec_slave[0].state != EC_STATE_OPERATIONAL));

         if (ec_slave[0].state == EC_STATE_OPERATIONAL )
         {
            printf("Operational state reached for all slaves.\n");
            inOP = TRUE;
                /* cyclic loop */
            // int cycle_num = 1000;
            while(Is_ecRun){
                ec_send_processdata();
                wkc = ec_receive_processdata(EC_TIMEOUTRET);

                 if(wkc >= expectedWKC) //send and receive OK
                 {

//                     in_values->CMD2 = my_CMD[1];
//                     printf("Processdata cycle %4d, WKC %d", i, wkc);
//                     printf(" CMD2:%d,CH1 %5d,CH2 %5d,CH3 %5d, CH4 %5d",my_CMD[1],out_values->CH1, out_values->CH2,out_values->CH3,out_values->CH4);

//                     printf(" T:%"PRId64"\r",ec_DCtime);
//                     needlf = TRUE;
                     my_ec_process_hander();
                 }
//                 osal_usleep(5000);//5000us=5ms
                 osal_usleep(1000);//5000us=5ms
            }//while run
            inOP = FALSE;
           }// if EC_STATE_OPERATIONAL
            else
            {
                printf("Not all slaves reached operational state.\n");
                ec_readstate();
                for(i = 1; i<=ec_slavecount ; i++)
                {
                    if(ec_slave[i].state != EC_STATE_OPERATIONAL)
                    {
                        printf("Slave %d State=0x%2.2x StatusCode=0x%4.4x : %s\n",
                            i, ec_slave[i].state, ec_slave[i].ALstatuscode, ec_ALstatuscode2string(ec_slave[i].ALstatuscode));
                    }
                }
            }
            printf("\nRequest init state for all slaves\n");
            ec_slave[0].state = EC_STATE_INIT;
            /* request INIT state for all slaves */
            ec_writestate(0);
        }//if ec_config_init
        else
        {
            printf("No slaves found!\n");
        }
        printf("End simple test, close socket\n");
        /* stop SOEM, close socket */
        ec_close();
    }
    else
    {
        printf("No socket connection on %s\nExcecute as root\n",ifname);
    }
}

ec_adaptert_win* print_ec_adaptert(void){ //打印网络适配器

   ec_adaptert * adapter = NULL;

//   printf("Usage: slaveinfo ifname [options]\nifname = eth0 for example\nOptions :\n -sdo : print SDO info\n -map : print mapping\n");

//   printf ("Available adapters\n");
   adapter = ec_find_adapters ();
//   while (adapter != NULL)
//   {
//    printf ("Description : %s, Device to use for wpcap: %s\n", adapter->desc,adapter->name);
//      adapter = adapter->next;
//   }

   return (ec_adaptert_win*)adapter;
}

int print_ec_adaptert_free(ec_adaptert_win * adapter)
{
   ec_free_adapters ((ec_adaptert*)adapter);
   return 0;
}
