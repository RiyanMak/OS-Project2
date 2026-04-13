#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

sem_t tellerSem;

void* teller(void* arg){

    int value = *(int*)arg;

    //let this thread enter the critical section
    sem_wait(&tellerSem);



    //let this thread leave the critical section
    sem_post(&tellerSem);


    return NULL;
}

int main() {

    sem_init(&tellerSem, 0, 3);
    pthread_t tellerThread1, tellerThread2, tellerThread3;
    int teller1 = 1;
    int teller2 = 2;
    int teller3 = 3;


    //Create the threads here
    pthread_create(&tellerThread1, NULL, teller, &teller1);
    pthread_create(&tellerThread2, NULL, teller, &teller2);
    pthread_create(&tellerThread3, NULL, teller, &teller3);

    //use join to wait for threads to complete
    // pthread_join(teller1, NULL);
    // pthread_join(teller2, NULL);
    // pthread_join(teller3, NULL);


    return 0;
}