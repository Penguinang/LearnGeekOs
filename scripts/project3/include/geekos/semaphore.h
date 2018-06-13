

#ifndef  SEMAPHORE_H
#define SEMAPHORE_H

#include <geekos/ktypes.h>
#include <geekos/list.h>
#include <geekos/kthread.h>

struct Semaphore;

DEFINE_LIST(Semaphore_List, Semaphore);


#define MAX_NAME_LENGTH 25
#define MAX_SEMR_COUNT  20

struct Semaphore{
    char name[MAX_NAME_LENGTH];
    int sid;
    int value;
    int ref_count;
    struct Thread_Queue wait_queue;
    DEFINE_LINK(Semaphore_List, Semaphore);
};

IMPLEMENT_LIST(Semaphore_List, Semaphore);



int Create_Semaphore(char *name, int length, int initial_value);
int P(int sid);
int V(int sid);
int Destroy_Semaphore(int sid);

#endif // ! SEMAPHORE_H