/*
 * ======================================================================
File: ex5b3.c
======================================================================
Writen by : Rotem Kashani, ID = 209073352
    Mahmood Jazmawy, ID = 211345277

 * this is the front end side of our program.
 * it prompts the user to enter a command 'q' or 'p'.
 * 'p': it waits for the user to enter a series of numbers that end with a 0.
 * it puts the numbers in the shared memory array sends a signal to SIGUSR1 
 * and goes to sleep after the server finishes checking the array and sends a 
 * signal back the front end wakes up and prints the prime numbers in the array.
 * 'q': it waits for the user to enter a series of number and a 0 at the end 
 * of it. it puts the numbers in the shared array and sends a signal to SIGUSR2
 * and goes to sleep. after the server finishes and send a signal back the front
 * end wakes up and prints whether the series is a palindrome or not.
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
#define MAX_LEN 1000
const int P_ARR_SIZE = 102;
const int Q_ARR_SIZE = 22;


//=================-STRUCTS-======================
//=======================-PROTOTYPES-=========
void handle_palindrome(int *palindrome_ptr);

void handle_prime(int *prime_ptr);

void print_primes(int sig_num);

void print_palindrome(int sig_num);

//---------------------------------------------

int front_id;
//=====================-MAIN-====================================
int main() {
    int q_id, p_id;
    key_t q_key, p_key;
    int *prime_ptr, *palindrome_ptr;


    //------------ prime shared memory ------------------
    if ((p_key = ftok(".", 'p')) == -1) {
        perror("ftok() failed");
        exit(EXIT_FAILURE);
    }
    if ((p_id = shmget(p_key, 0, 0600)) == -1) {
        perror("shmget() failed");
        exit(EXIT_FAILURE);
    }
    if ((prime_ptr = (int *) shmat(p_id, NULL, 0)) < 0) {
        perror("shmat() failed");
        exit(EXIT_FAILURE);
    }

    //------------ palindrome shared memory ------------------
    if ((q_key = ftok(".", 'q')) == -1) {
        perror("ftok() failed");
        exit(EXIT_FAILURE);
    }
    if ((q_id = shmget(q_key, 0, 0600)) == -1) {
        perror("shmget() failed");
        exit(EXIT_FAILURE);
    }
    if ((palindrome_ptr = (int *) shmat(q_id, NULL, 0)) < 0) {
        perror("shmat() failed");
        exit(EXIT_FAILURE);
    }

    prime_ptr[1] = getpid();
    palindrome_ptr[1] = getpid();

    signal(SIGUSR1, print_primes);
    signal(SIGUSR2, print_palindrome);

    char command;
    while (1) {
        puts("enter type and command\n");
        scanf("%c", &command);

        if (command == 'q') { // if the user entered q
            handle_palindrome(palindrome_ptr);
        } else if (command == 'p') { // if the user entered p
            handle_prime(prime_ptr);
        } else { // if an invalid command was entered
            puts("invalid command!");
        }
    }
}

/**
 * this function is called when the user entered p as a command
 * it receives the numbers from the user and sends them to the prime checker server.
 */
void handle_prime(int *prime_ptr) {
    int tmp;

    // reset strings just in case of garbage memory
    for (int i = 2; i < P_ARR_SIZE; ++i) {
        prime_ptr[i] = 0;
    }
    // get numbers until 0 is met
    for (int i = 2; i < P_ARR_SIZE; ++i) {
        scanf("%d", &tmp);
        if (tmp == 0) {
            getchar(); // get rid of whitespace
            break;
        } else {
            prime_ptr[i] = tmp;
        }
    }

    kill(prime_ptr[0], SIGUSR1);
    pause();

    for (int i = 2; prime_ptr[i] != 0; ++i) {
        if(prime_ptr[i] != -1){
            printf("%d ", prime_ptr[i]);
        }
    }
    puts("");
}


/**
 * this function is called when the user entered q as a command
 * it receives a string from the user and sends it to the server
 * that checks for palindromes
 */
void handle_palindrome(int *palindrome_ptr) {
    int tmp;

    // reset strings just in case of garbage memory
    for (int i = 2; i < Q_ARR_SIZE; ++i) {
        palindrome_ptr[i] = 0;
    }

    // get numbers until 0 is met
    for (int i = 2; i < Q_ARR_SIZE; ++i) {
        scanf("%d", &tmp);
        if (tmp == 0) {
            getchar(); // get rid of whitespace
            break;
        } else {
            palindrome_ptr[i] = tmp;
        }
    }

    kill(palindrome_ptr[0], SIGUSR2);
    pause();

    if(palindrome_ptr[21] == 1){
        puts("is palindrome!");
    } else {
        puts("is NOT palindrome!");
    }
}

void print_primes(int sig_num){

}

void print_palindrome(int sig_num){

}
