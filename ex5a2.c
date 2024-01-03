/*
======================================================================
File: ex5a2.c
======================================================================
Writen by : Rotem Kashani, ID = 209073352
Mahmood Jazmawy, ID = 211345277

* This is the "prime number creators" program.
 * it joins the shared memory creates by the array owner.
 * it tries to insert a random prime number in the array for 100 times.
 * there's three creators running at te same time each one has it's turn to try
 * and insert. they use the fifth cell as a locking mechanism to indicate that 
 * some process it trying to insert at the moment wait for your turn.
 * when they finish they print how much numbers they managed to insert
*/

//-------include section---------------
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdbool.h>
#include <signal.h>
#include <math.h>

//--------const section----------------
const int ARR_SIZE = 105;
const int ARGC_SIZE = 2;
const int KEY = 4;

//------prototypes---------------------
void start_job(int shm_id, char* argv[]);

void fill_arr(int* ptr, int process_num);

bool is_prime(int num);

//---------main section----------------
int main(int argc, char* argv[]) {
    if (argc != ARGC_SIZE) {
        perror("Incorrect number of arguments");
        exit(EXIT_FAILURE);
    }

    key_t key;
    int shm_id;

    srand(atoi(argv[1]));

    if ((key = ftok(".", '5')) == -1) {
        perror("ftok() failed");
        exit(EXIT_FAILURE);
    }

    if ((shm_id = shmget(key, 0, 0600)) == -1) {
        perror("shmget() failed");
        exit(EXIT_FAILURE);
    }

    start_job(shm_id, argv);

    return EXIT_SUCCESS;
}

void start_job(int shm_id, char* argv[]) {
    int* ptr;

    if ((ptr = (int*)shmat(shm_id, NULL, 0)) < 0) {
        perror("shmat() failed");
        exit(EXIT_FAILURE);
    }

    int process_num = atoi(argv[1]);

    ptr[process_num] = 1;

    while (ptr[1] < 1 || ptr[2] < 1 || ptr[3] < 1) {}

    fill_arr(ptr, process_num);

    kill(ptr[0], SIGINT);
}

/**
 * try 100 times to insert prime numbers in the array.
 */
void fill_arr(int* ptr, int process_num) {
    int  curr_index;

    for (int i = 5; i < ARR_SIZE; ++i) {
        int num = rand() % ((int)(pow(10, 6)) - 1) + 2;

        if (is_prime(num)) {

            while (ptr[KEY] != 1) {} // loop until it's ur turn to insert

            ptr[KEY] = 0; // "lock" the array

            // the sum of how many numbers each process inserted
            // +5(starting index), -3 is the ones that ere before in the cell
            curr_index = ptr[1] + ptr[2] + ptr[3] + 5 - 3;

            if (curr_index == 5) { // if it's the first time inserting
                ptr[5] = num;
                ptr[process_num]++; //add the counter for this process
            }
            else {
                if (ptr[curr_index - 1] <= num) {
                    ptr[curr_index++] = num;
                    ptr[process_num]++; //add the counter for this process
                }
            }
            ptr[KEY] = 1; // unlock the array
        }
    }
    printf("found: %d\n", ptr[process_num] - 1);
}

/**
 * checks if the number is prime
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
