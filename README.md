Operating System Project 2
===
- Link: http://rswiki.csie.org/dokuwiki/courses:106_2:project_2

- Slide: http://rswiki.csie.org/dokuwiki/_media/courses:102_2:os2014_project_2_si.pdf

Quick start
---
以下是執行此 project 原始碼的步驟，
1. 首先我們必須編譯此原始碼，以及將以下核心模組安裝上去：`ksocket`, `master_device`, `slave_device`.  此步驟我們借用了 `sample_code` 裡的
shell script 來達成：
``` 
$ sudo ./compile.sh
```
2. (Optional) 此步驟只是來確認 `ksocket`, `master_device` 和 `slave_device` 是否成功的被 installed:
``` sudo lsmod ```
若成功，我們應能在 console 順利看到  
```
Module                  Size  Used by
slave_device           12905  0 
master_device          13040  0 
ksocket                15266  2 slave_device,master_device
```
等關於此三個核心模組之資訊。
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
