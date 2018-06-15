Operating System Project 2
===
- 作業連結:  http://rswiki.csie.org/dokuwiki/courses:106_2:project_2

- 投影片: http://rswiki.csie.org/dokuwiki/_media/courses:102_2:os2014_project_2_si.pdf

- 原始碼參考: 助教提供之 `sample_code`

執行步驟
---
以下是執行此 project 原始碼的步驟，
1. 首先我們必須編譯此原始碼，以及將以下核心模組安裝上去：`ksocket`, `master_device`, `slave_device`.  此步驟我們借用了 `sample_code` 裡的
shell script 來達成：
``` 
$ sudo ./compile.sh
```
2. (Optional) 此步驟只是來確認 `ksocket`, `master_device` 和 `slave_device` 是否成功的被 installed:
```
$ sudo lsmod
```
若成功，我們應能在 console 順利看到  
```
Module                  Size  Used by
slave_device           12905  0 
master_device          13040  0 
ksocket                15266  2 slave_device,master_device
```
等關於此三個核心模組之資訊。
3. 成功 install 這三個模組後，我們切換到 `user_program` 的資料夾,
``` 
$ cd user_program
```
4. 接著遵循課程網頁上 project 2 的範例輸出，我們對於 `master`, `slave` 兩邊的 `user_program` 分別給予以下指令：
    - For slave-side: `L` for <path/received_file>, `M` for method(`fcntl` or `mmap`), `IP` for master's IP
    - For master-side: `L` for <path/target_file>,  `M` for method(`fcntl` or `mmap`)  
例如：
```
$ sudo ./master ../input/f1.in mmap
$ sudo ./slave ../output/f1.out fcntl 127.0.0.1
``` 
如此我們便能在 console 上看到傳輸時間和傳輸的檔案大小。  
另外在 `dmesg` 裡，我們也能看到關於 mapped memory region 的 page descriptors。

檔案內容
---
- `src/master_device/` : 此資料夾包含了 `master_device` 核心模組的 c code 和 `Makefile` 
- `src/slave_device/`  : 此資料夾包含了 `slave_device` 核心模組的 c code 和 `Makefile` 
- `src/ksocket/`       : 此資料夾包含了 `ksocket` 核心模組的 c code 和 `Makefile` 
- `src/user_program/`  : `master`, `slave` side 的 user program 
- `src/input/`         : 要被傳輸之輸入檔案input files to be transmitted
- `src/output/`        : 輸出檔案的目的資料夾（不一定要輸出至此資料夾）
  
