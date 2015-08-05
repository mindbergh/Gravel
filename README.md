# Gravel
  Real-Time Operating System Kernel for ARM processor, which contains the following features:
  
- basic system calls (read, write, exit, sleep, time, mutex_lock, mutex_unlock, etc).
- context switching and rate monotonic task scheduling.
- timer and interrupt controller drivers.
- mutexes for concurrency control.
- a predicate for the UB admission control algorithm to verify the schedulability of the given task set.
- priority inheritance using Highest Locker Priority Protocol.
- a Snake game based on system calls of the implemented RTOS.
