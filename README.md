#Races and Synchronization Lab

###Overview:

Purpose: Engage (at a low level) with a range of synchronization problems

   ####Updates to a shared variable:
     
	   - Write a multithreaded application (using pthreads) that
		performs parallel updates to a shared variable
	   - Demonstrate the race condition in the provided <tt>add</tt> routine,
		and address it with different synchronization mechanisms
	   - Do performance instrumentation and measurement
 
   ####Updates to a shared complex data structure:
   
	   - Implement the four routines described in <a href="src/SortedList.h">SortedList.h</a>:
		<tt>SortedList_insert, SortedList_delete,
		    SortedList_lookup, SortedList_length</tt>
	   - Write a multi-threaded application, using pthread that performs,
		parallel updates to a sorted doubly linked list data structure
	   - Recognize and demonstrate the race conditions when performing
		linked list operations, and address them with different
		synchronization mechanisms
	   - Do performance instrumentation and measurement

###Usage:

	- Clone or download this repository
	- <tt>make</tt> to build the executables
	- <tt>make add_tests</tt> to get data for synchronization with a shared variable
	- <tt>make list_tests</tt> to get data for synchronization with a shared complex data structure
	- <tt>make graphs</tt> to graph collected data
	- Analyze outputted graphs (*.png) to understand the performance of multithreaded programs

#####The usage for the executables lab2_add and lab2_list are as follows:

./lab2_add [--threads=#] [--iterations=#] [--yield] [--sync={m,s,c}]

add-none ... no yield, no synchronization
add-m ... no yield, mutex synchronization
add-s ... no yield, spin-lock synchronization
add-c ... no yield, compare-and-swap synchronization
add-yield-none ... yield, no synchronization
add-yield-m ... yield, mutex synchronization
add-yield-s ... yield, spin-lock synchronization
add-yield-c ... yield, compare-and-swap synchronization

./lab2_list [--threads=#] [--iterations=#] [--yield={idl}] [--sync={m,s}]

yield=i ... yield in SortedList_insert
yield=d ... yield in SortedList_delete
yield=l ... yield in SortedList_lookup and SortedList_length
sync=m ... mutex synchronization
sync=s ... spin-lock synchronization
