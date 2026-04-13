#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

void* customer(void *arg){

    int value = *(int*)arg;
    printf("Created a new customer thread with id: %d\n", value);

    return NULL;
}

int main () {

    pthread_t customerThread;

    int customerNumber = 1;
    
    // for(int i = 1; i <= 50; i++){
    //     int customerArg = i;
    //     pthread_create(&customerId, NULL, customer, &customerArg);

    // }

    pthread_create(&customerThread, NULL, customer, &customerNumber);


    // pthread_join(customerId, NULL);

    return 0;
}


