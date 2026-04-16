# CS4348 Project 2 - Bank Simulation

## Requirements

This program must be compiled and run on a **Linux machine**. It will not work correctly on macOS due to macOS not supporting unnamed POSIX semaphores (`sem_init`).

## How to Compile

```bash
gcc -std=c99 -o bank bank.c -lpthread
```

## How to Run

```bash
./bank
```

## How to Save Output to a File

```bash
./bank > output.txt
```

## What It Does

Simulates a bank with 3 teller threads and 50 customer threads. The bank opens once all 3 tellers are ready. Customers travel to the bank, get in line, select a free teller, and perform either a deposit or withdrawal. Withdrawals require manager approval. Tellers access the safe to complete transactions with a max of 2 tellers inside at once. The bank closes once all 50 customers have been served.
