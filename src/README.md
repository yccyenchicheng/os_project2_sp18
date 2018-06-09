Source code of project2
===

Quick start
---
The following are the steps to execute the code of this project,
1. First we need to compile the codes and install the modules: `ksocket`, `master_device`, `slave_device`:
``` 
$ sudo ./compile.sh
```
2. (Optional) Check if `ksocket`, `master_device` and `slave_device` have been installed:
``` sudo lsmod ```
3. Change to `user_program` dir
``` 
$ cd user_program
```
4. Follow the sample input from the course website, we need to specify:
    - For slave-side: `L` for <path/received_file>, `M` for method(`fcntl` or `mmap`), `IP` for master's IP
    - For master-side: `L` for <path/target_file>,  `M` for method(`fcntl` or `mmap`)  
For example,
```
$ sudo ./master ../input/f1.in mmap
$ sudo ./slave ../output/f1.out fcntl 127.0.0.1
```  
Then we should see the `transimission time` and `transmitted file size`.  
And in `mseg`, we should see the `page descriptors` for method `mmap`.

File contents
---
- `master_device/` : the directory of the c code and `Makefile` for the `master_device` kernel module
- `slave_device/`  : the directory of the c code and `Makefile` for the `slave_device` kernel module
- `ksocket/`       : the directory of the c code and `Makefile` for the `ksocket` kernel module to use the kernel socket 
- `user_program/`  : the user program for `master` side and `slave` side
- `input/`         : input files to be transmitted
- `output/`        : output files created by us
  
Thanks for reading.
