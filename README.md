# Races and Synchronization Lab

## Overview:

**Purpose**: Engage (at a low level) with a range of synchronization problems

#### Updates to a shared variable:
     
- Write a multithreaded application (using pthreads) that
		performs parallel updates to a shared variable
- Demonstrate the race condition in the provided `add` routine, and address it with different synchronization mechanisms
- Do performance instrumentation and measurement
 
#### Updates to a shared complex data structure:
   
- Implement the four routines described in <a href="src/SortedList.h">SortedList.h</a>:
		`SortedList_insert`, `SortedList_delete`,
		    `SortedList_lookup`, `SortedList_length`
- Write a multi-threaded application, (using pthreads) that performs,
		parallel updates to a sorted doubly linked list data structure
- Recognize and demonstrate the race conditions when performing
		linked list operations, and address them with different
		synchronization mechanisms
- Do performance instrumentation and measurement

## Usage:

- Clone or download this repository
- `cd src`
- `make` to build the executables
- `make add_tests` to get data for synchronization with a shared variable
- `make list_tests` to get data for synchronization with a shared complex data structure
- `make graphs` to graph collected data
- Analyze outputted graphs (*.png) to understand the performance of multithreaded programs

#### The usage for the executables lab2_add and lab2_list is as follows:
```
./lab2_add [--threads=#] [--iterations=#] [--yield] [--sync={m,s,c}]
```
- iterations=# ... specify the number of iterations (default 1)
- threads=# ... specify the number of threads (default 1)
- yield ... yield in critical section of add
- sync=m ... mutex synchronization
- sync=s ... spin-lock synchronization
- sync=c ... compare-and-swap synchronization
```
./lab2_list [--threads=#] [--iterations=#] [--yield={idl}] [--sync={m,s}]
```
- iterations=# ... specify the number of iterations (default 1)
- threads=# ... specify the number of threads (default 1)
- yield=i ... yield in SortedList_insert
- yield=d ... yield in SortedList_delete
- yield=l ... yield in SortedList_lookup and SortedList_length
- sync=m ... mutex synchronization
- sync=s ... spin-lock synchronization
