# Capability Based Authorization of open System Call in Linux
Preventing unauthorized invocation of open system call in Linux by running processes in sand-boxed environments using capability based authorization implemented in user space.

The code base consistes of three C files - daemon.c, preload.c & open.c

preload.c is a dynamic dispatch mechanism implemented at the user level which redirects the system call to Daemon. The dynamic dispatch mechanism is done by wrapper functions that override the existing LibC APIs. A shared library is created that contains  wrapper functions. We have to preload the shared library in order to execute the wrapper function instead of the original
API. To preload the shared library, we use a shell environment variable called LD_PRELOAD. To call back the original function in the event of a system call override, we use dlsym() function.

daemon.c is responsible for the issue of capability tokens and the system call authorization. It has a unique userid and groupid. When daemon receives an open request from the wrapper function, it retrieves the process id, effective user id (EUID) and effective group id (EGID) of the initiated process from the UDS and is compared against the file status. If the file permissions are matching with the process credentials, Daemon authorizes the system call and returns the file handle to wrapper function via Unix Domain Socket. 

open.c is used to open a file normally.


Method to run the code base
---------------------------

 Run the following commands in multiple termnal window :
 
      make 
      LD_PRELOAD=./preload.so ./open vivek.txt
      
Dependencies Required
---------------------
      libcap-dev
    
Installation method
-------------------
   
    sudo apt-get install libcap-dev
