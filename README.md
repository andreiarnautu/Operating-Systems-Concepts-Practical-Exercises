# Operating-Systems-Concepts-Practical-Exercises

This repository contains my solutions in C to the practical exercises in the 1oth version of the "Operating System Concepts" book by Abraham Silberschatz, Greg Gagne and Peter B. Galvin.

I have been passioned by operating systems for some time and decided to go through this book on my own and solve most of the coding challenges along the way. Each solved exercise has its own subfolder.

Please note that I do not guarantee that my solutions are necessarily optimal. I am still learning.

You can find a brief description for each solved exercise/project down below. Most of the exercises have their own README in their own subfolder, where more specific details are provided.

## Brief exercise description

### Chapter 2 - OS structures
* **2.24**
    * A simple program which copies the contents of a file to another file.

* **2.4**
    * A simple linux kernel module that creates a new entry in the `/proc` file-system that reports the number of seconds since the kernel module was loaded.

### Chapter 3 - Processes
* **3.19**
    * A program that measures the amount of time necessary to run a command from the command line. The given command is being run in a child process.
    * `time_pipe.c` - implements the communication between the two processes using a pipe.
    * `time_shared_memory.c` - implements the communication between the two processes using shared memory.

* **3.20**
    * Implements a basic API to allocate and release a process id using a bitmap.

* **3.21**
    * A program that generates the Collatz sequence of a given number in a child process.

* **3.22**
    * Same as **3.21**, but the child process saves the sequence in a shared-memory object.

* **3.26**
    * A program containing a short message exchange between two processes using ordinary pipes. 

* **3.27**
    * A file-copying program using ordinary pipes.

* **UNIX Shell**
    * A program to serve as a shell interface that accepts user commands and then executes each command in a separate process.
    * It supports input and output redirections, as well as pipes as a form of inter-process communication between commands. However, a command may contain only one redirection or pipe - I plan on extending this functionality in the future.

* **Linux Kernel Module for Task Information**
    * A kernel module that uses the `/proc` file-system for displaying a task's information based on its process id.

* **Linux Kernel Module for Listing Tasks**
    * A kernel module that lists all tasks in a Linux system. This is implemented both linearly and depth first.

* **Kernel Data Structures**
    * A project which consists in using the kernel linked list structure.

### Chapter 4 - Threads & Concurrency
* **4.22**
    * A multithreaded program which computes statistical values for an array of numbers

* **4.24**
    * A multithreaded program which uses the Monte Carlo technique to approximate the value of pi.

* **Sudoku Solution Validator**
    * A program which checks whether a given sudoku solution is valid or not.
    * The validation for each separate row, column and 3x3 square is done in a separate thread.

* **Multithreaded sorting application**
    * A version of merge-sort which makes use of multithreading.
    * **Results**: you can literally see the speedup between sorting the array using 1 thread and using multiple threads. On my PC, the multithreaded version ran ~4 times faster.


