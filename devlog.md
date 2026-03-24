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