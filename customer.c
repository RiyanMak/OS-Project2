#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

void* customer(void *arg){

    int value = *(int*)arg;
    printf("Created a new customer thread with id: %d\n", value);

    return NULL;
}

int main () {

    pthread_t customerId;
    
    for(int i = 1; i <= 50; i++){
        int customerArg = i;
        pthread_create(&customerId, NULL, customer, &customerArg);
        pthread_join(customerId, NULL);
    }

    return 0;
}


