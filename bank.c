#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

#define NUM_TELLERS   3
#define NUM_CUSTOMERS 50

sem_t bank_open;
int tellers_ready = 0;
pthread_mutex_t open_mutex = PTHREAD_MUTEX_INITIALIZER;

sem_t teller_free;
int free_queue[NUM_TELLERS];
int free_queue_size = 0;
pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;

sem_t customer_arrived[NUM_TELLERS]; /* customer → teller: "I'm here" */
int serving_customer[NUM_TELLERS];   /* which customer id is at teller i */

int bank_closed = 0;

void *teller_thread(void *arg)
{
    int id = *(int *)arg;
    printf("Teller %d [Teller %d]: ready to serve customers\n", id, id);

    pthread_mutex_lock(&open_mutex);
    tellers_ready++;
    if (tellers_ready == NUM_TELLERS) {
        printf("Teller %d [Teller %d]: all tellers ready, opening bank\n", id, id);
        for (int i = 0; i < NUM_CUSTOMERS; i++)
            sem_post(&bank_open);
    }
    pthread_mutex_unlock(&open_mutex);

    while (1) {
        /* add self to the free queue and signal customers */
        pthread_mutex_lock(&queue_mutex);
        free_queue[free_queue_size++] = id;
        pthread_mutex_unlock(&queue_mutex);
        sem_post(&teller_free);

        /* wait for a customer to arrive */
        sem_wait(&customer_arrived[id]);
        if (bank_closed) break;

        int cid = serving_customer[id];
        printf("Teller %d [Customer %d]: serving customer\n", id, cid);
    }

    printf("Teller %d [Teller %d]: done for the day\n", id, id);
    return NULL;
}

void *customer_thread(void *arg)
{
    int id = *(int *)arg;

    sem_wait(&bank_open);
    printf("Customer %d [Customer %d]: enters bank\n", id, id);

    /* wait for a free teller */
    sem_wait(&teller_free);

    pthread_mutex_lock(&queue_mutex);
    int tid = free_queue[0];
    for (int i = 0; i < free_queue_size - 1; i++)
        free_queue[i] = free_queue[i + 1];
    free_queue_size--;
    pthread_mutex_unlock(&queue_mutex);

    printf("Customer %d [Teller %d]: selects teller\n", id, tid);

    /* introduce self to the teller */
    serving_customer[tid] = id;
    printf("Customer %d [Teller %d]: introduces self to teller\n", id, tid);
    sem_post(&customer_arrived[tid]);

    return NULL;
}

int main(void)
{
    sem_init(&bank_open, 0, 0);
    sem_init(&teller_free, 0, 0);
    for (int i = 0; i < NUM_TELLERS; i++)
        sem_init(&customer_arrived[i], 0, 0);
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

    /* wait for all customers to finish */
    for (int i = 0; i < NUM_CUSTOMERS; i++)
        pthread_join(customers[i], NULL);

    /* shut down tellers */
    bank_closed = 1;
    for (int i = 0; i < NUM_TELLERS; i++)
        sem_post(&customer_arrived[i]);

    for (int i = 0; i < NUM_TELLERS; i++)
        pthread_join(tellers[i], NULL);

    printf("Bank is now closed.\n");
    return 0;
}
