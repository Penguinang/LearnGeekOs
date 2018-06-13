#include <geekos/semaphore.h>
#include <geekos/kthread.h>
#include <geekos/user.h>
#include <geekos/malloc.h>
#include <string.h>

/**

    TODO:
    - Create Semaphore Function
    - Destroy Semaphore Function
    - Just like kthread.c and kthread.h

 */

/* -------------------------------------------------------------------------------
 * Private Data
 * ------------------------------------------------------------------------------- */
static struct Semaphore_List s_semaphoreList;
static int sem_count = 0;

static struct Semaphore *get_semaphore_with_name(char *name){
    struct Semaphore *sem = s_semaphoreList.head;

    // Try to find a semaphore in s_semaphoreList with name name param
    while(sem){
        if(!strcmp(sem->name, name)){
            return sem;
        }
        else{
            sem = Get_Next_In_Semaphore_List(sem);
        }
    }

    return NULL;
}

static struct Semaphore *get_semaphore_with_sid(int sid){
    struct Semaphore *sem = s_semaphoreList.head;

    // Try to find a semaphore in s_semaphoreList with name name param
    while(sem){
        if(sem->sid == sid){
            return sem;
        }
        else{
            sem = Get_Next_In_Semaphore_List(sem);
        }
    }

    return NULL;
}



/* -------------------------------------------------------------------------------
 * Public Function
 * ------------------------------------------------------------------------------- */

/*
* Create a semaphore.
* Params:
*   name - address of name of semaphore
*   length - length of semaphore name
*   initial_value - initial semaphore count
* Returns: the global semaphore id
*/
int Create_Semaphore(char *name, int length, int initial_value){
    if(sem_count > MAX_SEMR_COUNT)
        return -1;

    struct Semaphore *sem = get_semaphore_with_name(name);
    if(!sem){
        // No semaphore with name existing so we create a new semaphore
        sem_count ++;
        sem = (struct Semaphore*)Malloc(sizeof(struct Semaphore));
        strcpy(sem->name, name);
        sem->sid = s_semaphoreList.tail ? s_semaphoreList.tail->sid+1 : 0;
        sem->value = initial_value;
        sem->ref_count = 0;
        Add_To_Back_Of_Semaphore_List(&s_semaphoreList, sem);
    }

    struct User_Context *userContext = g_currentThread->userContext;
    userContext->semaphores[userContext->semaphore_count++] = sem->sid;
    sem->ref_count ++;
    return sem->sid;
}

/*
 * Acquire a semaphore.
 * Assume that the process has permission to access the semaphore,
 * the call will block until the semaphore count is >= 0.
 * Params:
 *   state->ebx - the semaphore id
 *
 * Returns: 0 if successful, error code (< 0) if unsuccessful
 */
int P(int sid){
    struct Semaphore *sem = get_semaphore_with_sid(sid);
    if(sem){
        if(--sem->value >= 0)
            ;
        else{
            // TODO
            // Block current thread
            // Remove run queue
            Add_To_Back_Of_All_Thread_List(&sem->wait_queue, g_currentThread);
        }
        return 0;
    }
    else{
        return -1;
    }  
}

/*
 * Release a semaphore.
 * Params:
 *   state->ebx - the semaphore id
 *
 * Returns: 0 if successful, error code (< 0) if unsuccessful
 */
int V(int sid){

}

/*
* Destroy a semaphore.
* Params:
*   sid - the semaphore id
*
* Returns: 0 if successful, error code (< 0) if unsuccessful
*/
int Destroy_Semaphore(int sid){
    struct Semaphore *sem = get_semaphore_with_sid(sid);
    if(!sem){
        return -1;
    }
    else{
        Remove_From_Semaphore_List(&s_semaphoreList, sem);
        Free(sem);
        return 0;
    }
}
