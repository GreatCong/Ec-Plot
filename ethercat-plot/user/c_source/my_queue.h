#ifndef __MY_QUEUE_H_
#define __MY_QUEUE_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef int16_t elemType;

struct sNode{
    elemType data;            /* 值域 */
    struct sNode *next;        /* 链接指针 */
};

struct queueLK{
    struct sNode *front;    /* 队首指针 */
    struct sNode *rear;        /* 队尾指针 */
};

void initQueue(struct queueLK *hq);
void enQueue(struct queueLK *hq, elemType x);
elemType outQueue(struct queueLK *hq);
elemType peekQueue(struct queueLK *hq);
int emptyQueue(struct queueLK *hq);
void clearQueue(struct queueLK *hq);

#ifdef __cplusplus
}
#endif

#endif // MY_QUEUE_H

