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