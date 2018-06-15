Operating System Project 2
===
- Link: http://rswiki.csie.org/dokuwiki/courses:106_2:project_2

- Slide: http://rswiki.csie.org/dokuwiki/_media/courses:102_2:os2014_project_2_si.pdf

Quick start
---
The following are the steps to execute the code of this project,
1. First we need to compile the codes and install the modules: `ksocket`, `master_device`, `slave_device`:
``` 
$ sudo ./compile.sh
```
2. (Optional) Check if `ksocket`, `master_device` and `slave_device` have been installed:
``` sudo lsmod ```
3. Change to `user_program` directory,
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
And in `dmesg`, we should see the `page descriptors` for method `mmap`.

File contents
---
- `src/master_device/` : the directory of the c code and `Makefile` for the `master_device` kernel module
- `src/slave_device/`  : the directory of the c code and `Makefile` for the `slave_device` kernel module
- `src/ksocket/`       : the directory of the c code and `Makefile` for the `ksocket` kernel module to use the kernel socket 
- `src/user_program/`  : the user program for `master` side and `slave` side
- `src/input/`         : input files to be transmitted
- `src/output/`        : output files created by us
  
Thanks for reading.
