/*
======================================================================
File: ex5a1.c
======================================================================
Writen by : Rotem Kashani, ID = 209073352
Mahmood Jazmawy, ID = 211345277

* This is the Array Owner Process. initializes an array with 105 cells.
 * cell #0: has thes owner's pid
 * cell #1,2,3: is a cell for each process, we use it at first to indicate the process are running
 *  then we use it as a counter for how many values this process has inserted in the array.
 * cell #4: this cell is used a sa lock to give each process it's turn to insert values
 *
 * The rest of the cells are used as normal cells to get values.
 *
 * after initializing the array this process goes to sleep and waits for a signal
 * to wake up and print the required info about the array we have now.
 * --> size, min value, max value
*/

//-------include section---------------
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>


//--------const section----------------
const int ARR_SIZE = 105;

//------prototypes---------------------
void catch_signal(int sig_num);

void start_job();

void init_primes_arr(int *primes_arr);

void delete_shm(int shm_id);

int shm_id;

//---------main section----------------
int main() {
    signal(SIGINT, catch_signal);

    key_t key;

    // create shared memory
    if ((key = ftok(".", '5')) == -1) {
        perror("ftok() failed");
        exit(EXIT_FAILURE);
    }

    if ((shm_id = shmget(key, ARR_SIZE, IPC_CREAT
                                        | IPC_EXCL | 0600)) == -1) {
        perror("shmget() failed");
        exit(EXIT_FAILURE);
    }

    start_job();

    delete_shm(shm_id);

    return EXIT_SUCCESS;
}

void catch_signal(int sig_num) {}


/**
 * start the filling of the array
 */
void start_job() {
    int *ptr;

    //attach
    if ((ptr = (int *) shmat(shm_id, NULL, 0)) < 0) {
        perror("shmat() failed");
        exit(EXIT_FAILURE);
    }

    init_primes_arr(ptr); // init to zero

    pause();


    // the size of the array is each process cell minus 3 which is the one
    // that was before
    int size = ptr[1] + ptr[2] + ptr[3] - 3;
    printf("size %d.\nmin: %d.\nmax: %d.\n", size, ptr[5], ptr[size + 4]);
}

/**
 * init the array to zeros and put the pid and lock cell
 */
void init_primes_arr(int *primes_arr) {
    for (int i = 0; i < ARR_SIZE; i++)
        primes_arr[i] = 0;

    primes_arr[0] = getpid();
    primes_arr[4] = 1;
}


/**
 * delete shared memory
 */
void delete_shm(int shm_id) {
    if (shmctl(shm_id, IPC_RMID, NULL) == -1) {
        perror("shmctl() failed");
        exit(EXIT_FAILURE);
    }
}

