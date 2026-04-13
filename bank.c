#include <stdio.h>
#include <pthread.h>

#define NUM_TELLERS   3
#define NUM_CUSTOMERS 50

void *teller_thread(void *arg)
{
    int id = *(int *)arg;
    printf("Teller %d [Teller %d]: ready to serve customers\n", id, id);
    return NULL;
}

void *customer_thread(void *arg)
{
    int id = *(int *)arg;
    printf("Customer %d [Customer %d]: created\n", id, id);
    return NULL;
}

int main(void)
{
    pthread_t tellers[NUM_TELLERS];
    int teller_ids[NUM_TELLERS];
    for (int i = 0; i < NUM_TELLERS; i++) {
        teller_ids[i] = i;
        pthread_create(&tellers[i], NULL, teller_thread, &teller_ids[i]);
    }

    pthread_t customers[NUM_CUSTOMERS];
    int customer_ids[NUM_CUSTOMERS];
    for (int i = 0; i < NUM_CUSTOMERS; i++) {
        customer_ids[i] = i;
        pthread_create(&customers[i], NULL, customer_thread, &customer_ids[i]);
    }

    for (int i = 0; i < NUM_TELLERS; i++)
        pthread_join(tellers[i], NULL);

    for (int i = 0; i < NUM_CUSTOMERS; i++)
        pthread_join(customers[i], NULL);

    return 0;
}
