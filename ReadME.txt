Description: 

The project was to implement a virtual memory manager to simulate TLB and Page Tables and mapping of logical addresses to physical addresses. The simulation code was written in C Language. An Analysis also made to measure the TLB Hit Rate and Page Fault Rate based on the number of Frames and TLB entries available.

How to RUN:

WE can Change the No of Frames Needed in the Program by Changing the value of NO_OF_FRAMES macro

To Compile We execute the following command

gcc -Wall Virtual_Manager.c -o Virtual_Manager

To run the program we execute the command

./Virtual_Manager

The Program will output the number of Page Faults and The TLB Miss Rate

Before running the program make sure that Addressess.txt is present in the folder we are running the program.

The detailed design of the virtual Memory Manager can be seen in Virtual manager design report pdf.
