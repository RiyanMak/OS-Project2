#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define NUM_TELLERS   3
#define NUM_CUSTOMERS 50

sem_t bank_open;
int tellers_ready = 0;
sem_t open_sem;    /* binary semaphore, protects tellers_ready (init 1) */

sem_t teller_free;
int free_queue[NUM_TELLERS];
int free_queue_size = 0;
sem_t queue_sem;   /* binary semaphore, protects free_queue (init 1)    */

sem_t customer_arrived[NUM_TELLERS];   /* customer → teller: "I'm here"       */
sem_t teller_asks_txn[NUM_TELLERS];   /* teller  → customer: "what txn?"     */
sem_t customer_gives_txn[NUM_TELLERS];/* customer → teller: "here it is"     */
sem_t teller_done[NUM_TELLERS];       /* teller  → customer: "all done"      */
sem_t customer_gone[NUM_TELLERS];     /* customer → teller: "I'm leaving"    */
int serving_customer[NUM_TELLERS];    /* which customer id is at teller i    */
int txn_type[NUM_TELLERS];            /* 0 = deposit, 1 = withdraw           */

sem_t manager_sem; /* only 1 teller with manager at a time */
sem_t safe_sem;    /* max 2 tellers in safe at once       */
sem_t door_sem;    /* max 2 customers through door at once */

int bank_closed = 0;

void *teller_thread(void *arg)
{
    int id = *(int *)arg;

    /* coordinate bank opening — last teller to be ready opens the bank */
    sem_wait(&open_sem);
    tellers_ready++;
    if (tellers_ready == NUM_TELLERS) {
        for (int i = 0; i < NUM_CUSTOMERS; i++)
            sem_post(&bank_open);
    }
    sem_post(&open_sem);

    while (1) {
        printf("Teller %d []: ready to serve\n", id);

        /* add self to the free queue */
        sem_wait(&queue_sem);
        free_queue[free_queue_size++] = id;
        sem_post(&queue_sem);

        /* print waiting BEFORE signaling customers so message always precedes selection */
        printf("Teller %d []: waiting for a customer\n", id);
        sem_post(&teller_free);

        /* wait for a customer to arrive */
        sem_wait(&customer_arrived[id]);
        if (bank_closed) break;

        int cid = serving_customer[id];
        printf("Teller %d [Customer %d]: serving a customer\n", id, cid);

        /* ask for the transaction */
        printf("Teller %d [Customer %d]: asks for transaction\n", id, cid);
        sem_post(&teller_asks_txn[id]);

        /* wait for customer to provide it */
        sem_wait(&customer_gives_txn[id]);

        const char *txn_str = txn_type[id] ? "withdrawal" : "deposit";
        printf("Teller %d [Customer %d]: handling %s transaction\n", id, cid, txn_str);

        /* withdrawal requires manager approval */
        if (txn_type[id] == 1) {
            printf("Teller %d [Customer %d]: going to the manager\n", id, cid);
            sem_wait(&manager_sem);
            printf("Teller %d [Customer %d]: getting manager's permission\n", id, cid);
            /* rand()%26 gives 0–25, +5 shifts to 5–30, *1000 converts ms to us */
            usleep((rand() % 26 + 5) * 1000);
            printf("Teller %d [Customer %d]: got manager's permission\n", id, cid);
            sem_post(&manager_sem);
        }

        /* go to the safe to perform the transaction (max 2 tellers inside) */
        printf("Teller %d [Customer %d]: going to safe\n", id, cid);
        sem_wait(&safe_sem);
        printf("Teller %d [Customer %d]: enter safe\n", id, cid);
        /* rand()%41 gives 0–40, +10 shifts to 10–50, *1000 converts ms to us */
        usleep((rand() % 41 + 10) * 1000);
        printf("Teller %d [Customer %d]: leaving safe\n", id, cid);
        sem_post(&safe_sem);

        printf("Teller %d [Customer %d]: finishes %s transaction.\n", id, cid, txn_str);

        /* print "wait" message and THEN signal customer so customer's "leaves teller"
           always appears after this line in the output */
        printf("Teller %d [Customer %d]: wait for customer to leave.\n", id, cid);
        sem_post(&teller_done[id]);

        /* wait for customer to leave before serving next */
        sem_wait(&customer_gone[id]);
    }

    printf("Teller %d []: leaving for the day\n", id);
    return NULL;
}

void *customer_thread(void *arg)
{
    int id = *(int *)arg;
    /* rand()%2 gives 0 or 1 randomly: 0 = deposit, 1 = withdraw */
    int txn = rand() % 2;
    const char *txn_str = txn ? "withdrawal" : "deposit";
    printf("Customer %d []: wants to perform a %s transaction\n", id, txn_str);

    /* travel to the bank: rand()%101 gives 0–100, *1000 converts ms to us */
    usleep((rand() % 101) * 1000);
    printf("Customer %d []: going to bank.\n", id);

    /* wait for bank to open */
    sem_wait(&bank_open);

    /* enter through the door (max 2 customers at a time) */
    sem_wait(&door_sem);
    printf("Customer %d []: entering bank.\n", id);
    sem_post(&door_sem);

    printf("Customer %d []: getting in line.\n", id);

    /* wait for a free teller */
    sem_wait(&teller_free);

    sem_wait(&queue_sem);
    int tid = free_queue[0];
    for (int i = 0; i < free_queue_size - 1; i++)
        free_queue[i] = free_queue[i + 1];
    free_queue_size--;
    sem_post(&queue_sem);

    printf("Customer %d []: selecting a teller.\n", id);
    printf("Customer %d [Teller %d]: selects teller\n", id, tid);

    /* introduce self to the teller */
    serving_customer[tid] = id;
    printf("Customer %d [Teller %d] introduces itself\n", id, tid);
    sem_post(&customer_arrived[tid]);

    /* wait for teller to ask for transaction */
    sem_wait(&teller_asks_txn[tid]);

    /* tell the teller the transaction type */
    txn_type[tid] = txn;
    printf("Customer %d [Teller %d]: asks for %s transaction\n", id, tid, txn_str);
    sem_post(&customer_gives_txn[tid]);

    /* wait for teller to finish the transaction */
    sem_wait(&teller_done[tid]);

    /* leave the teller, then walk out through the door */
    printf("Customer %d [Teller %d]: leaves teller\n", id, tid);
    sem_post(&customer_gone[tid]);
    printf("Customer %d []: goes to door\n", id);
    printf("Customer %d []: leaves the bank\n", id);

    return NULL;
}

int main(void)
{
    sem_init(&bank_open,   0, 0);
    sem_init(&teller_free, 0, 0);
    sem_init(&manager_sem, 0, 1);
    sem_init(&safe_sem,    0, 2);
    sem_init(&open_sem,    0, 1); /* binary semaphore acting as mutex */
    sem_init(&queue_sem,   0, 1); /* binary semaphore acting as mutex */
    sem_init(&door_sem,    0, 2); /* 2 customers through door at once */
    for (int i = 0; i < NUM_TELLERS; i++) {
        sem_init(&customer_arrived[i],   0, 0);
        sem_init(&teller_asks_txn[i],    0, 0);
        sem_init(&customer_gives_txn[i], 0, 0);
        sem_init(&teller_done[i],        0, 0);
        sem_init(&customer_gone[i],      0, 0);
    }

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

    printf("The bank closes for the day.\n");
    return 0;
}
