# School-Project-Operating-System-Kernel
This project is a mandatory part of Operating Systems course. It emulates a simple operating system kernel providing API to work with threads, synchronization and events. 

Thread interface is the same as in Java. The user is expected to extend on the Thread class and override the run() method and call the start() method to start using the thread. 

Semaphores, on the other hand, provide one type of synchronization between usercreated threads. It has a simple interface consisting of wait() and signal() methods that provide sync option for a particular object of the Semaphore class.

Events also provide a synchronization option, but in a way that a thread proceedes when a particular interrupt has been generated in the system. It makes sense to be a keybutton pressed event, for example. The PREPAREENTRY() macro is needed in order to create a paricular interrupt routine before creating an object of type Event. 

User is expected to write a userMain(int, char**) function which has the same properties as the main() function, except that main() is reserved for the execution of th OS kernel emulator. It is created that way because this OS runs on a host 32bit Windows Operating System. 

This porgram needs a 32bit Windows Operating System as it was written in Borland C++ 3.1(1992) designed for x86 architecture, and will not work otherwise. A sample user program is provided. 
