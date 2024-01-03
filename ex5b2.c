/*
 * ======================================================================
File: ex5b2.c
======================================================================
Writen by : Rotem Kashani, ID = 209073352
    Mahmood Jazmawy, ID = 211345277

 * This program is our server that checks for palindrome series.
 * it creates a shared memory of an array of 22 cells.
 * it "goes to sleep" until it receives a signal to wake up.
 * when it does wake up it goes to our shared memory array and goes over 
 * the numbers in it. if the series of number is a palindrome is puts 1 in 
 * the 21th cell otherwise it puts 0. after it finishes it "goes to sleep" and
 * send a signal to the front end to wake up.
 
 */

//====================-INCLUDES-=================
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>

//==============-DEFINE-=======================
int const NOT_PRIME = -1;
int const ARR_SIZE = 22;

//=================-STRUCTS-======================


//=======================-PROTOTYPES-===========================
void catch_sigint(int sig_num);

int is_palindrome(const int *ptr);

void wake_up(int sig_num);

//=======-GLOBALS-==================
int shm_id;

//===========-MAIN-==================
int main() {
    int *ptr;
    key_t key;
    signal(SIGINT, catch_sigint);

    signal(SIGUSR2, wake_up);

    if ((key = ftok(".", 'q')) == -1) {
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
        ptr[21] = is_palindrome(ptr);
        kill(ptr[1], SIGUSR2);
    }
}

int is_palindrome(const int *ptr) {
    int i, j;

    for (i = 2; ptr[i] != 0; ++i) {}
    j = i - 1;

    for (i =2;  i< j; ++i, --j) {
        if(ptr[i] != ptr[j]){
            return 0;
        }
    }
    return 1;
}

/**
 * catch SIGINT, frees queue when signal is raised
 */
void catch_sigint(int sig_num) {
    if (shmctl(shm_id, IPC_RMID, NULL) == -1) {
        perror("shmctl() failed");
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}

void wake_up(int sig_num){}

