
/*
======================================================================
File: ex5b1.c
======================================================================
Writen by : Rotem Kashani, ID = 209073352
    Mahmood Jazmawy, ID = 211345277

 * This program is our server that checks for prime numbers.
 * it creates a shared memory of an array of 102 cells.
 * it "goes to sleep" until it receives a signal to wake up.
 * when it does wake up it goes to our shared memory array and 
 * goes over the numbers in it. if the number is not a prime it replaces it 
 * with -1. after it finishes it "goes to sleep" and send a signal to 
 * the front end to wake up.
 */

//==============-INCLUDES-===============
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdbool.h>
#include <signal.h>

//================-DEFINE-================
int const NOT_PRIME = -1;
int const ARR_SIZE = 102;

//==============-STRUCTS-================

//===============-PROTOTYPES-=============
void catch_sigint(int sig_num);

bool is_prime(int num);

void wake_up(int sig_num);


//============-GLOBLAS-================
int shm_id;

//==============-MAIN-=====================
int main() {
    int *ptr;
    key_t key;
    signal(SIGINT, catch_sigint);

    signal(SIGUSR1, wake_up);

    if ((key = ftok(".", 'p')) == -1) {
        perror("ftok() failed");
        exit(EXIT_FAILURE);
    }

    if ((shm_id = shmget(key, ARR_SIZE, IPC_CREAT
                                        | IPC_EXCL | 0600)) == -1) {
        perror("shmget() failed");
        exit(EXIT_FAILURE);
    }


    if ((ptr = (int *) shmat(shm_id, NULL, 0)) < 0) {
        perror("shmat() failed");
        exit(EXIT_FAILURE);
    }

    ptr[0] = getpid();

    while(1){
        pause();

        for (int i = 2; i < ARR_SIZE; ++i) {
            if(ptr[i] > 0){
                if(!is_prime(ptr[i])){
                    ptr[i] = NOT_PRIME;
                }
            }
        }

        kill(ptr[1], SIGUSR1);
    }
}


/**
 * This function checks if a number is prime or not
 */
bool is_prime(int num) {
    int mid = num / 2;
    if (num < 2) {
        return false;
    }
    for (int i = 2; i <= mid; i++) {
        if (num % i == 0) {
            return false;
        }
    }
    return true;
}


void wake_up(int sig_num){

}
/**
 * catch SIGINT and free message queue
 */
void catch_sigint(int sig_num) {
    if (shmctl(shm_id, IPC_RMID, NULL) == -1) {
        perror("shmctl() failed");
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}

