#include <stdio.h>
#include <pthread.h>
#include <unistd.h>


void* teller(void* arg){

    int value = *(int*)arg;

    printf("Created a new teller with teller thread with id: %d\n", value);

    return NULL;
}

int main() {
    pthread_t teller1, teller2, teller3;
    int teller1Id = 1;
    int teller2Id = 2;
    int teller3Id = 3;


    //Create the threads here
    pthread_create(&teller1, NULL, teller, &teller1Id);
    pthread_create(&teller2, NULL, teller, &teller2Id);
    pthread_create(&teller3, NULL, teller, &teller3Id);

    //use join to wait for threads to complete
    pthread_join(teller1, NULL);
    pthread_join(teller2, NULL);
    pthread_join(teller3, NULL);


    return 0;
}