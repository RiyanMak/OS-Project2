# Devlog

## 2026-03-24 13:53

### Initial Thoughts

Currently trying to understanding the bigger picture on how threads will interact with each other and
how the synchronization be implemented for threads that will be conflict with each other for resources


### Overall Plan

I'm thinking of using the C and their Pthreads library and will have different files one for the teller, manager and the customers. Will have a main file that will put all of this together probably going to use some forks and pipes for each thread
to communicate with the other.

### Open Questions

I'm not sure of the overall architecture and havent really worked with threads like this. I will definetly need to change
up my approaches here and there.

---

# Devlog

## 2026-04-11 10:35

### Initial Thoughts

I have the threads for the teller.c up and now need to create the threads for the customers. Will have to start the interactions between the customer and the teller after the creation of the customer threads



### Overall Plan

Create the customer threads lets just start with creating 5 customer threads as recommended in the project description
and we will set up the communication between the two and then scale up to 50 customers

### Open Questions

At this point I am confused on how these threads will communicate with each other and how will i set up states where
it can show that a teller is ready to communicate with the customers

---

# Devlog

## 2026-04-13

### Initial Thoughts

Now that the threads are created and running I need to start getting the synchronization working. Started adding semaphores for the teller, customer and bank interactions. Also need to get the bank opening gate working so customers cant enter before all tellers are ready.

### Overall Plan

Started with the bank opening semaphore and the teller free queue so customers can actually pick a teller. Then wired up the customer teller handshake so they can communicate back and forth. Added the transaction exchange so the customer can tell the teller what they want. Got the manager semaphore working for withdrawals and the safe semaphore capped at 2 tellers. Added the completion handshake so the teller waits for the customer to leave before serving the next one. Also added the door semaphore for 2 customers at a time and the travel delay for customers. Refactored the mutexes to binary semaphores to keep everything consistent.

### Open Questions

The output messages are not matching the sample format yet. Some of the context brackets are wrong and some lines are missing entirely. Need to go through the sample output and fix the print statements.

---

# Devlog

## 2026-04-16

### Initial Thoughts

Need to fix all the print statements to match the sample output. The core synchronization logic was correct but almost every message was wrong in some way whether it was the wrong context brackets, missing lines or wrong wording.

### Overall Plan

Went through the sample output line by line and fixed everything. The manager and safe messages were using [Teller X] instead of [Customer X]. Added the missing messages like handling transaction, getting managers permission, got managers permission, enter safe, finishes transaction and wait for customer to leave. Fixed the order so the teller prints wait for customer to leave before posting teller_done so the ordering in the output is always correct. Fixed the customer departure to go leaves teller then goes to door then leaves the bank. Fixed introduces itself to have no colon. Fixed the bank close message to say The bank closes for the day. Also fixed the teller loop so ready to serve and waiting for a customer print at the top of each loop iteration. Ran into an issue where sem_init doesnt work on macOS so switched to named semaphores to test locally. Tested on the school linux server and it compiled and ran correctly with all 50 customers served.

### Open Questions

Nothing major left just need to make sure the submission is clean.

---