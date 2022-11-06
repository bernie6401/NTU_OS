# NTU Operating System Project2

## Implement System Call

### Objective

We'd like to add sleep() function in system call

### Observation

1. First of all, we need to define a new token, SC_Sleep,  that compiler(scanner) can recognize in `code/userprog/syscall.h`.

2. So, we have to assign a number to SC_Sleep that it can return the value.

   ```c++
   ...
   #define SC_ThreadFork	9
   #define SC_ThreadYield	10
   #define SC_PrintInt	11
   
   /* ********************************************* 
     Self-defined
    ********************************************* */
   #define SC_Sleep    12.
   ...
   ```

3. Then, observe the other define words like SC_PrintInt in the same file. You can see there're lots of declaration function for each system call such as `void PrintInt(int number);` or `void ThreadFork(void (*func)());`,etc.. So, we must declare a function for sleep system call.

   ```c++
   ...
   /* ********************************************* 
     Self-defined
    ********************************************* */
   void Sleep(int number);
   #endif /* IN_ASM */
   ...
   ```

4. According to the assignment description file, another file should be checked is start.s in test/start.s.

   According to the comment above this file, it's a file to assist system calls to **NachOS kernel** by assembly language.

   > Start.s 
   > *	Assembly language **assist for user programs running on top of Nachos**. Since we don't want to pull in the entire C library, we define what we need for a user program here, namely Start and the system calls.
   >
   > System call stubs:
   > *  Assembly language assist to make system calls to the Nachos kernel. There is one stub per system call, that places the code for the system call into register r2, and leaves the arguments to the system call alone (in other words, arg1 is in r4, arg2 is  in r5, arg3 is in r6, arg4 is in r7)
   > The return value is in r2. This follows the standard C calling convention on the MIPS.

   ```assembly
   /* ********************************************* 
     Self-defined
    ********************************************* */
   	.globl  Sleep	/* Set Sleep to a global symbol that can be found in real c++ code properly */
   	.ent    Sleep	/* Set the entry point for Sleep */
   Sleep:
   	addiu   $2,$0,SC_Sleep
   	syscall
   	j       $31
   	.end    Sleep
   ```

   

5. From assignment description file, we should catch the exception situation by userprog/exception.cc. So, we need to define a new case in this file.

   According to the comment above in exception.cc

   > Entry point into the Nachos kernel from user programs. There are two kinds of things that can cause control to transfer back to here from user code:
   >
   > * **syscall** -- The user code explicitly requests to call a procedure in the Nachos kernel.  Right now, the only function we support is "Halt".
   > * **exceptions** -- The user code does something that the CPU can't handle. For instance, accessing memory that doesn't exist, arithmetic errors, etc.
   >
   > ...
   >
   > **ExceptionHandler**
   > *	Entry point into the Nachos kernel.  Called when a user program is executing, and either does a syscall, or generates an addressing or arithmetic exception.
   > *	For system calls, the following is the calling convention:
   > 	system call code -- r2
   > 				arg1 -- r4
   > 				arg2 -- r5
   > 				arg3 -- r6
   > 				arg4 -- r7
   > 	The result of the system call, if any, must be put back into r2. 
   > *	And don't forget to increment the pc before returning. **(Or else you'll loop making the same system call forever!**
   > *	"which" is the kind of exception.  The list of possible exceptions are in machine.h.

   So, observe the previous case, SC_PrintInt, we can guess the value stored in `kernel->machine->Readgister(4)`. The code is like this...

   ```c++
   /* ********************************************* 
   Self-defined
   ********************************************* */
   case SC_Sleep:
       val=kernel->machine->ReadRegister(4);
       cout << "Sleep a while:" <<val << "(ms)" << endl;
       return;
   ```

   But, this code **can not really stop the executing** instead of printing out a normal message. So, refer to the assignment note, we should check **`kernel->alarm->WaitUntil()`** function that'll really implement sleep function in **threads/alarm.h**. The result after adding the code is just like...

   ```c++
   /* ********************************************* 
   Self-defined
   ********************************************* */
   case SC_Sleep:
       val=kernel->machine->ReadRegister(4);
       cout << "Sleep a while:" <<val << "(ms)" << endl;
       kernel->alarm->WaitUntil(val);
       return;
   ```
   *More info about WainUntil() function and interrupt, you can check out [here](http://blog.terrynini.tw/tw/OS-NachOS-HW1/#%E4%B8%AD%E6%96%B7%E5%B8%B8%E5%BC%8F)

6. **The most important part**

   Define a class named **sleepList** in alarm.h and implement the methods in alarm.cc. All the other description can check out [OS-NachOS-HW1](http://blog.terrynini.tw/tw/OS-NachOS-HW1/#中斷常式) and use VSode can be more clearly. Lazy to write it down.

7. Refer to [OS-NachOS-HW1](http://blog.terrynini.tw/tw/OS-NachOS-HW1/#測試-1), you can write your own testing case or just use the ready-made test case on the web. Note that you must revise `Makefile` in `code/test/Makefile` like this...

   ```makefile
   
   all: halt shell matmult sort test1 test2 sleep1 sleep2
   ...
   sleep1: sleep1.o start.o
   	$(LD) $(LDFLAGS) start.o sleep1.o -o sleep1.coff
   	../bin/coff2noff sleep1.coff sleep1
   
   sleep2: sleep2.o start.o
   	$(LD) $(LDFLAGS) start.o sleep2.o -o sleep2.coff
   	../bin/coff2noff sleep2.coff sleep2
   ```

   *Note that you must use **tab** in `Makefile`.

   *The purpose of these manipulation is when you use `make` command in `code/Makefile`. It'll execute all `Makefile` that exist in each folder. It can be observed in `code/Makefile`

   ```makefile
   MAKE = make
   LPR = lpr
   
   all: 
   	cd threads; $(MAKE) depend
   	cd threads; $(MAKE) nachos
   	cd userprog; $(MAKE) depend 
   	cd userprog; $(MAKE) nachos 
   	cd filesys; $(MAKE) depend
   	cd filesys; $(MAKE) nachos 
   	cd network; $(MAKE) depend
   	cd network; $(MAKE) nachos 
   	cd bin; $(MAKE) all
   	cd test; make all
   ```

   *Error message I encountered:

   ```
   ...
   ../bin/coff2noff halt.coff halt
   make[1]: execvp: ../bin/coff2noff: Permission denied
   make[1]: *** [halt] Error 127
   make[1]: Leaving directory `/home/sbk/NTU/Operating System/Project2/nachos-4.0/code/test'
   make: *** [all] Error 2
   ```

   According to [Why do I get permission denied when I try use "make" to install something?](https://stackoverflow.com/questions/9106536/why-do-i-get-permission-denied-when-i-try-use-make-to-install-something) page on StackOverflow, just use `chmod 777 ./bin/coff2noff` in `./code` folder.

8. Testing result

   I create another test case named Sleep3.c and aim to test the sleep time **10 times longer** than Sleep1.c and Sleep2.c is aim to test the time that **100 times shorter** than Sleep1.c.

   * sleep1

     ```c++
     #include "syscall.h"
     main() {
         int i;
         for(i = 0; i < 5; i++) {
             Sleep(1000000);
             PrintInt(2222);
         }
         return 0;
     }
     ```

     ![testing result for sleep1](https://imgur.com/wk19ont.png)

   * Sleep2

     ```c++
     #include "syscall.h"
     main() {
         int i;
         for(i = 0; i < 5; i++) {
             Sleep(10000);
             PrintInt(99999);
         }
         return 0;
     }
     ```

     ![result for sleep2](https://imgur.com/5r37MJg.png)

   * Sleep3

     ```c++
     #include "syscall.h"
     main() {
         int i;
         for(i = 0; i < 5; i++) {
             Sleep(10000000);
             PrintInt(666);
         }
         return 0;
     }
     ```

     ![result for sleep3](https://imgur.com/rrOmAJm.png)

   In Sleep1.c, you can feel the sleep function working clearly that compare with a normal code without sleep function or compare with a shorter sleep time such as Sleep2.c
   And in Sleep3.c, you can feel the sleeping time much more longer that what we expected but just execute 3 times PrintInt function.

9. 

## Implement CPU Scheduling

## Reference

* [OS-NachOS-HW1](http://blog.terrynini.tw/tw/OS-NachOS-HW1/)