/* Aaron VanderGraaff 
   CPE453 
   Assignment 3: Dining with Philosophers
*/

#include "dine.h"

sem_t fork_locks[NUM_PHILOSOPHERS];
phil phils[NUM_PHILOSOPHERS]; 
sem_t print_lock; 
int total = -1;

int main(int argc, char *argv[]) {

    int i;
    int id[NUM_PHILOSOPHERS];
    pthread_t childid[NUM_PHILOSOPHERS];

    if (argc == 1) {
        total = 1;
    }
    else if (argc == 2) {
        total = (int) strtol(argv[1], NULL, DECIMAL);

        if (total < 0) {
            fprintf(stderr, "You can't run negative times!\n");
            exit(EXIT_FAILURE);
        }
    }
    else {
        fprintf(stderr, "Incorrect Usage\n");
        exit(EXIT_FAILURE);
    }

    if (NUM_PHILOSOPHERS < 2) {
	    fprintf(stderr, 
		"You can't run with less than 2 philosophers!\n");
	    exit(EXIT_FAILURE);
    }

    /* Initialize my globals */
    initialize();
    print_header();

    for (i=0; i<NUM_PHILOSOPHERS; i++) {
        id[i] = i;
    }


    for (i=0; i<NUM_PHILOSOPHERS; i++) {
        int res;
        res = pthread_create(&childid[i], NULL, philosopher, 
                (void *) (&id[i]));

        if (res == -1) {
            fprintf(stderr, "Unable to create philosopher %d\n", i);
            perror("pthread_create:");
            exit(EXIT_FAILURE);
        }
    }

    /* Wait on all my threads and destroy semaphores */
    for (i=0; i<NUM_PHILOSOPHERS; i++) {
        if (pthread_join(childid[i], NULL)) {
            perror("pthread_join");
            exit(EXIT_FAILURE);
        }
        sem_destroy(&fork_locks[i]);
    }

    return 0;


}

int initialize(void) {
    int i;

    for (i=0; i<NUM_PHILOSOPHERS; i++) {
        sem_t cur;
        phil cur_phil;

        cur_phil.id = i;
        cur_phil.l_fork = NONE;
        cur_phil.r_fork = NONE;
        cur_phil.status = CHANGING;

        phils[i] = cur_phil;


        if (sem_init(&cur, 0, 1) == -1) {
            fprintf(stderr, "Could not initialize %d semaphore\n", i);
	    perror("sem_init:");
	    exit(EXIT_FAILURE);
        } 
        fork_locks[i] = cur;
    }

    if (sem_init(&print_lock, 0, 1) == -1) {
        fprintf(stderr, "Could not initialize print semaphore\n");
        perror("sem_init:");
    	exit(EXIT_FAILURE);
    }

    return 0;
}

void *philosopher(void *id) {
    int real_id;
    int r_fork_w;
    int l_fork_w;
    phil *cur_phil;
    int counter;

    counter = 0;

    real_id = *(int *) id;
    cur_phil = &phils[real_id];

    
    l_fork_w = get_fork_desired(real_id, LEFT);
    r_fork_w = get_fork_desired(real_id, RIGHT);


    while (counter < total) {

        /* If changing, try to pick up forks */
        if (cur_phil->status == CHANGING && 
                (cur_phil->l_fork == NONE && cur_phil->r_fork == NONE)) {

            if (real_id%2) {
                /* Odd philosopher */

                /* Lock the left fork*/
                if (DOWN(&fork_locks[l_fork_w]) == -1) {
                    fprintf(stderr, ERR_DWN_FORK, l_fork_w);
		    perror("DOWN:");
    	            exit(EXIT_FAILURE);
                }

                /* Lock the print lock (locks philosopher list) */
                if (DOWN(&print_lock) == -1) {
                    fprintf(stderr, ERR_DWN_PRINT);
		    perror("DOWN:");
    		    exit(EXIT_FAILURE);
                }

                /* Pick up the left fork */
                cur_phil->l_fork = HOLDING; 

                /* Print state */
                print_state();

                /* Unlock the print lock */
                if (UP(&print_lock) == -1) {
                    fprintf(stderr, ERR_UP_PRINT);
		    perror("UP:");
    		    exit(EXIT_FAILURE);
                }

                /* Lock the right fork */
                if (DOWN(&fork_locks[r_fork_w]) == -1) {
                    fprintf(stderr, ERR_DWN_FORK, r_fork_w);
		    perror("DOWN:");
    		    exit(EXIT_FAILURE);
                }
                
                /* Lock the print lock */
                if (DOWN(&print_lock) == -1) {
                    fprintf(stderr, ERR_DWN_PRINT);
		    perror("DOWN:");
    		    exit(EXIT_FAILURE);
                }

                /* Pick up the right fork */
                cur_phil->r_fork = HOLDING;

                /* Print state*/
                print_state();

                /* Unlock the print lock */
                if (UP(&print_lock) == -1) {
                    fprintf(stderr, ERR_UP_PRINT);
		    perror("UP:");
    		    exit(EXIT_FAILURE);
                }
            }

            else {
                /* Even philosopher */
                /* Same process as odd, but right fork first */
                
                /* Lock the right fork */
                if (DOWN(&fork_locks[r_fork_w]) == -1) {
                    fprintf(stderr, ERR_DWN_FORK, r_fork_w);
		    perror("DOWN:");
    		    exit(EXIT_FAILURE);
                }
                
                /* Lock the print lock */
                if (DOWN(&print_lock) == -1) {
                    fprintf(stderr, ERR_DWN_PRINT);
		    perror("DOWN:");
    		    exit(EXIT_FAILURE);
                }

                /* Pick up the right fork */
                cur_phil->r_fork = HOLDING;

                /* Print state*/
                print_state();

                /* Unlock the print lock */
                if (UP(&print_lock) == -1) {
                    fprintf(stderr, ERR_UP_PRINT);
		    perror("UP:");
    		    exit(EXIT_FAILURE);
                }

                /* Lock the left fork*/
                if (DOWN(&fork_locks[l_fork_w]) == -1) {
                    fprintf(stderr, ERR_DWN_FORK, l_fork_w);
		    perror("DOWN:");
    		    exit(EXIT_FAILURE);
                }

                /* Lock the print lock (locks philosopher list) */
                if (DOWN(&print_lock) == -1) {
                    fprintf(stderr, ERR_DWN_PRINT);
		    perror("DOWN:");
    		    exit(EXIT_FAILURE);
                }

                /* Pick up the left fork */
                cur_phil->l_fork = HOLDING; 

                /* Print state */
                print_state();

                /* Unlock the print lock */
                if (UP(&print_lock) == -1) {
                    fprintf(stderr, ERR_UP_PRINT);
		    perror("UP:");
    		    exit(EXIT_FAILURE);
                }

            }

            /* Now I have 2 forks, time to eat */

            /* Lock the printing */
            if (DOWN(&print_lock) == -1) {
                fprintf(stderr, ERR_DWN_PRINT);
		perror("DOWN:");
    		exit(EXIT_FAILURE);
            }

            /* Change status to eating */
            cur_phil->status = EATING;
            
            /* Print the state */
            print_state();

            /* Unlock the print lock */
            if (UP(&print_lock) == -1) {
                fprintf(stderr, ERR_UP_PRINT);
		perror("UP:");
    		exit(EXIT_FAILURE);
            }
            
            /* Take my time eating */
            dawdle();

            /* Done eating, change back ... I'm done commenting the UPS and
               DOWNS ...
            */
            if (DOWN(&print_lock) == -1) {
                fprintf(stderr, ERR_DWN_PRINT);
		perror("DOWN:");
    		exit(EXIT_FAILURE);
            }

            /* I want to put my forks down, change status */
            cur_phil->status = CHANGING;
            print_state();

            if (UP(&print_lock) == -1) {
                fprintf(stderr, ERR_UP_PRINT);
		perror("UP:");
    		exit(EXIT_FAILURE);
            }
        }

        else if (cur_phil->status == CHANGING && 
                (cur_phil->l_fork != NONE && cur_phil->r_fork != NONE)) {
            
            /* I want to put my forks down please */


            if (DOWN(&print_lock) == -1) {
                fprintf(stderr, ERR_DWN_PRINT);
		perror("DOWN:");
    		exit(EXIT_FAILURE);
            }

            /* Put my right fork down */
            cur_phil->r_fork = NONE;
            print_state();

            if (UP(&print_lock) == -1) {
                fprintf(stderr, ERR_UP_PRINT);
		perror("UP:");
    		exit(EXIT_FAILURE);
            }
	    
	    /* Unock the right fork */
            if (UP(&fork_locks[r_fork_w]) == -1) {
                fprintf(stderr, ERR_UP_FORK, r_fork_w);
		perror("UP:");
    		exit(EXIT_FAILURE);
            }

            
            if (DOWN(&print_lock) == -1) {
                fprintf(stderr, ERR_DWN_PRINT);
		perror("DOWN:");
    		exit(EXIT_FAILURE);
            }

            /* Put my left fork down */
            cur_phil->l_fork = NONE;
            print_state();

            if (UP(&print_lock) == -1) {
                fprintf(stderr, ERR_UP_PRINT);
		perror("UP:");
    		exit(EXIT_FAILURE);
            }

	    /* Unlock the left fork */
            if (UP(&fork_locks[l_fork_w]) == -1) {
                fprintf(stderr, ERR_UP_FORK, l_fork_w);
		perror("UP:");
    		exit(EXIT_FAILURE);
            }


            /* Both forks are down, time to think! */
            if (DOWN(&print_lock) == -1) {
                fprintf(stderr, ERR_DWN_PRINT);
		perror("DOWN:");
    		exit(EXIT_FAILURE);
            }
           
            cur_phil->status = THINKING;
            print_state();

            if (UP(&print_lock) == -1) {
                fprintf(stderr, ERR_UP_PRINT);
		perror("UP:");
    		exit(EXIT_FAILURE);
            }

            /* Think unti I decide to be hungry again */
            dawdle();

            /* Counter represents how many times to go through the eat think
               cycle
            */
            counter++;
        }

        else if (cur_phil->status == THINKING) {
            /* I think I'm hungry again */

            if (DOWN(&print_lock) == -1) {
                fprintf(stderr, ERR_DWN_PRINT);
		perror("DOWN:");
    		exit(EXIT_FAILURE);
            }
            
            /* Change status to changing, so start hunting for forks */
            cur_phil->status = CHANGING;
            print_state();

            if (UP(&print_lock) == -1) {
                fprintf(stderr, ERR_UP_PRINT);
		perror("UP:");
	        exit(EXIT_FAILURE);
            }

        }
    }

    if (cur_phil->status == THINKING) {
        /* This is in case we make it out of the loop
           without finishing our thinking 
        */
        
        if (DOWN(&print_lock) == -1) {
            fprintf(stderr, ERR_DWN_PRINT);
	    perror("DOWN:");
	    exit(EXIT_FAILURE);
        }
        
        /* Changing pretty much means print nothing at this point, 
           I'm done with all my tasks
        */ 

        cur_phil->status = CHANGING;
        print_state();

        if (UP(&print_lock) == -1) {
            fprintf(stderr, ERR_UP_PRINT);
	    perror("UP:");
	    exit(EXIT_FAILURE);
        }
    }

    return NULL; 
}

void print_header(void) {
    int i;
    int j;
    int size_header;

    size_header = NUM_PHILOSOPHERS + 8;

    for (i=0; i<NUM_PHILOSOPHERS; i++) {

        putchar('|');
        for (j=0; j<size_header; j++) {
            putchar('=');
        }
    }
    putchar('|');
    putchar('\n');

    for (i=0; i<NUM_PHILOSOPHERS; i++) {
	putchar('|');
	for (j=0; j<size_header/2; j++) {
	    putchar(' ');
	}
	putchar((char) i+'A');
	for (j=size_header/2+1; j<size_header; j++) {
	    putchar(' ');
	}
    }
    putchar('|');
    putchar('\n');

    for (i=0; i<NUM_PHILOSOPHERS; i++) {

        putchar('|');
        for (j=0; j<size_header; j++) {
            putchar('=');
        }
    }
    putchar('|');
    putchar('\n');
    print_state();
}


int get_fork_desired(int id, int side)  {
    /* Assign indices of the forks a philosopher needs */
    if (id == 0) {
        if (side == LEFT) {
            return id;
        }
        else if (side == RIGHT) {
            return id+1;
        }
    }
    else if (id == NUM_PHILOSOPHERS -1) {
	if (side == LEFT) {
	    return id;
	}
	else {
	    return 0;
	}
    }
    else {
        if (side == LEFT) {
            return id; 
        }
        else if (side == RIGHT) {
            return id+1;
        }
    }

    return -1;
}
void dawdle() {
    /*
    * sleep for a random amount of time between 0 and 999
    * milliseconds. This routine is somewhat unreliable, since it
    * doesn’t take into account the possiblity that the nanosleep
    * could be interrupted for some legitimate reason.
    *
    * nanosleep() is part of the realtime library and must be linked
    * with –lrt
    */
    struct timespec tv;
    int msec = (int)(((double)random() / RAND_MAX) * 1000);
    
    tv.tv_sec = 0;
    tv.tv_nsec = 1000000 * msec;
    if (-1 == nanosleep(&tv,NULL) ) {
        perror("nanosleep");
    }
}


void print_state(void) {
    int i;
    int j;
    phil cur_phil;
    int lfork;
    int rfork;
    char forkstring[NUM_PHILOSOPHERS+1];

    for (i=0; i<NUM_PHILOSOPHERS; i++) {


        for (j=0; j<NUM_PHILOSOPHERS; j++) {
            forkstring[j] = '-';
        }
            
        forkstring[NUM_PHILOSOPHERS] = '\0';


        cur_phil = phils[i];
        lfork = get_fork_desired(i, LEFT);
        rfork = get_fork_desired(i, RIGHT);

        if (cur_phil.l_fork != NONE) {
            forkstring[lfork] = (char) (lfork + '0');
        }
        
        if (cur_phil.r_fork != NONE) {
            forkstring[rfork] = (char) (rfork + '0');;
        }

        printf("| ");
        printf("%s", forkstring);

        if (cur_phil.status == EATING) {
            printf(" Eat   ");
        }
        else if (cur_phil.status == THINKING) {
            printf(" Think ");
        }
        else {
            printf("       ");
        }
        
    }
    printf("|\n");
    
}

