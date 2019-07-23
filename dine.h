#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#ifndef NUM_PHILOSOPHERS
#define NUM_PHILOSOPHERS 5
#endif


#define CHANGING 0
#define EATING 1
#define THINKING 2
#define FINISHED 3

#define NONE 0
#define HOLDING 1

#define LEFT 0
#define RIGHT 1

#define DOWN(s) (sem_wait(s))
#define UP(s) (sem_post(s))

#define ERR_UP_PRINT "Error with UP() on print lock\n"
#define ERR_DWN_PRINT "Error with DOWN() on print lock\n"
#define ERR_UP_FORK "Error with UP() on fork %d\n"
#define ERR_DWN_FORK "Error with DOWN() on fork %d\n"

#define DECIMAL 10

int initialize(void); 
void *philosopher(void *id); 
void print_header(void); 
int get_fork_desired(int id, int side);  
void dawdle(void); 
void print_state(void); 

typedef struct phil phil;
struct phil {
    int id;
    int l_fork;
    int r_fork;
    int status;
};
