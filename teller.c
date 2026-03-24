#include <stdio.h>
#include <pthread.h>


void* teller(void* arg){


    printf("Created a new teller with teller thread with id: %p\n", pthread_self());
    return NULL;
}

int main() {
    pthread_t teller1, teller2, teller3;


    //Create the threads here
    pthread_create(&teller1, NULL, teller, NULL);
    pthread_create(&teller2, NULL, teller, NULL);
    pthread_create(&teller3, NULL, teller, NULL);

    //use join to wait for threads to complete
    pthread_join(teller1, NULL);
    pthread_join(teller2, NULL);
    pthread_join(teller3, NULL);


    return 0;
}