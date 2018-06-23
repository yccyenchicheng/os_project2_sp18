Operating System Project 2 Report Spring 2018 NTU
===

程式的設計
---
- `src/master_device/` : 裡面包含
    - `Makefile`: `master_device` 的 Makefile，我們用其編譯核心模組，使得我們能透過 `sudo insmod` 來安裝模組，接著便能供 `master` 的 `user_program`: `master.c` 使用。
    - `master_device.c`: 實作 master_device 的 file operation, 包含負責建立 socket 連結的 ioctl, 接收 user program 資料的 write 與 mmap。
- `src/slave_device/` : 此資料夾主要由助教提供的 `sample_code` 來改進，進行了 `fcntl` 和 `mmap` 的實作以及傳輸時間的計算。另外 *page descriptors of the mapped memory region* 也能透過 `dmesg` 來查看。
    - `Makefile`: `slave_device` 的 Makefile，我們用其編譯核心模組，使得我們能透過 `sudo insmod` 來安裝模組，接著便能供 `slave` 的 `user_program`: `slave.c` 使用。
    - `slave_device.c`: 我們同樣在參考了 `sample_code` 裡 `slave_device` 的範例來實作 `slave_device.c`，另外在核心模組的觀念上，我們也參考了下列連結的 c 程式碼：  
1. http://derekmolloy.ie/writing-a-linux-kernel-module-part-1-introduction/  
2. http://derekmolloy.ie/writing-a-linux-kernel-module-part-2-a-character-device/  

- `src/ksocket/`: 此資料夾主要由助教提供的 `sample_code` 而來，包含了用 C 實作的 `kernel_module` 以及 `Makefile` 來編譯此 `kernel_module`。正如同助教所說，此資料夾主要把 `kernel socket` 的方程式用一些 wrapper 包起來，如此能讓 `master`, `slave` 在使用上能比較方便。
- `src/user_program/master.c` : 透過 `sudo insmod *.ko` 來安裝 `master_device` 後，我們便能透過 `master` 的 `user_program` 來將其如檔案般打開(會回傳 `file_descriptor` 供我們操作)。在 `master` 已持續監聽著 `slave_device` 時並準備好傳送檔案時，我們便能根據使用者給定的方法(`fcntl` or `mmap`)來做檔案的讀寫。最後計算檔案傳輸的速度（根據 `master_device`, `slave_device` 打開、關閉的時間）及檔案大小（根據 `stat(char* filename, struct stat* st)`) 來紀錄結果。
- `src/user_program/slave.c` : 透過 `sudo insmod *.ko` 來安裝 `slave_device` 後，我們便能透過 `slave` 的 `user_program` 來將其如檔案般打開(會回傳 `file_descriptor` 供我們操作)。接著在`slave_device` 已連接到 `master` 的 ip 並準備好接收檔案時，我們便能根據使用者給定的方法(`fcntl` or `mmap`)來做檔案的讀寫。最後計算檔案傳輸的速度（根據 `master_device`, `slave_device` 打開、關閉的時間）及檔案大小（根據 `stat(char* filename, struct stat* st)`) 來紀錄結果。

執行的結果
---
- `f` 表示使用方法為 `fcntl`; `m` 表示使用方法為 `mmap`  
----- Master method `fcntl`, Slave method `fcntl` -----  

- File: `file1_in`
- Command:
```
src/user_program/master data/file1_in fcntl
src/user_program/slave data/file1_in.out fcntl 127.0.0.1
```
- Output:  
Master:  
`Transmission time: 1.463000 ms, File size: 32 bytes`  
Slave:  
`Transmission time: 2.419000 ms, File size: 32 bytes`  

- File: `file2_in`
- Command:
```
src/user_program/master data/file2_in fcntl
src/user_program/slave data/file2_in.out fcntl 127.0.0.1
```
- Output:  
Master:  
`Transmission time: 0.732000 ms, File size: 4619 bytes`  
Slave:  
`Transmission time: 4.355000 ms, File size: 4619 bytes`  

- File: `file3_in`
- Command:  
```
src/user_program/master data/file3_in fcntl
src/user_program/slave data/file3_in.out fcntl 127.0.0.1
```
- Output:  
Master:  
`Transmission time: 2.185000 ms, File size: 77566 bytes`  
Slave:  
`Transmission time: 3.036000 ms, File size: 77566 bytes`  

----- Master method `fcntl`, Slave method `mmap` -----  

- File: `file1_in`
- Command:  
```
src/user_program/master data/file1_in fcntl
src/user_program/slave data/file1_in.out mmap 127.0.0.1
```
- Output:  
Master:  
`Transmission time: 0.052000 ms, File size: 32 bytes`  
Slave:
`Transmission time: 0.965000 ms, File size: 32 bytes`  

- File: `file2_in`
- Command:
```
src/user_program/master data/file2_in fcntl
src/user_program/slave data/file2_in.out mmap 127.0.0.1
```
- Output:  
Master:  
`Transmission time: 0.537000 ms, File size: 4619 bytes`  
Slave:  
`Transmission time: 7.026000 ms, File size: 4619 bytes`  

- File: `file3_in`
- Command:  
```
src/user_program/master data/file3_in fcntl
src/user_program/slave data/file3_in.out mmap 127.0.0.1
```
- Output:  
Master:  
`Transmission time: 0.960000 ms, File size: 77566 bytes`  
Slave:  
`Transmission time: 10.642000 ms, File size: 77566 bytes`  

----- Master method (`mmap`), Slave method (`fcntl`) -----  

- File: `file1_in`  
- Command:  
```
src/user_program/master data/file1_in mmap
src/user_program/slave data/file1_in.out fcntl 127.0.0.1
```
- Output:  
Master:  
`Transmission time: 0.939000 ms, File size: 32 bytes`  
Slave:  
`Transmission time: 0.829000 ms, File size: 32 bytes`  

- File: `file2_in`  
- Command:  
```
src/user_program/master data/file2_in mmap
src/user_program/slave data/file2_in.out fcntl 127.0.0.1
```
- Output:  
Master:  
`Transmission time: 0.188000 ms, File size: 4619 bytes`  
Slave:  
`Transmission time: 30.082000 ms, File size: 4619 bytes`  

- File: `file3_in`
- Command:
```
src/user_program/master data/file3_in mmap
src/user_program/slave data/file3_in.out fcntl 127.0.0.1
```
- Output:  
Master:  
`Transmission time: 0.214000 ms, File size: 77566 bytes`  
Slave:  
`Transmission time: 2.441000 ms, File size: 77566 bytes`  

----- Master method (mmap), Slave method (mmap) -----

- File: `file1_in`
- Command:  
```
src/user_program/master data/file1_in mmap
src/user_program/slave data/file1_in.out mmap 127.0.0.1
```
- Output:  
Master:  
`Transmission time: 0.059000 ms, File size: 32 bytes`  
Slave:  
`Transmission time: 1.138000 ms, File size: 32 bytes`  

- File: `file2_in`
- Command:  
```
src/user_program/master data/file2_in mmap
src/user_program/slave data/file2_in.out mmap 127.0.0.1
```
- Output:  
Master:  
`Transmission time: 3.625000 ms, File size: 4619 bytes`  
Slave:  
`Transmission time: 5.180000 ms, File size: 4619 bytes`  

- File: `file3_in`
- Command:  
```
src/user_program/master data/file3_in mmap
src/user_program/slave data/file3_in.out mmap 127.0.0.1
```
- Output:  
Master:  
`Transmission time: 0.182000 ms, File size: 77566 bytes`  
Slave:  
`Transmission time: 1.336000 ms, File size: 77566 bytes`

file I/O v.s. memory-mapped I/O & 差異的原因
---

- 當我們使用file I/O的時候，資料傳輸的路徑為：
```
        disk -> kernel space -> user space -> socket space (master)
             ->socket space -> user space -> kernel space -> disk (slave)
```  
而使用mmap的話可以略過kernel space，則是
```
        disk -> user space -> socket space (master)
             ->socket space -> user space -> disk (slave)
```
故使用mmap可以改善transmission rate。  
- 示意圖：
![](https://i.imgur.com/mDA90kj.png)
![](https://i.imgur.com/bcdfghY.png)

1. 就f3.in和f4.in的實驗結果來說，m/m不論在master端或是slave端，不論read或write的時間皆快過f/f。
2. 而f1.in和f2.in的實驗結果中，當執行時間小於1ms的話，m/m並不一定會快過f/f。猜是因為執行mmap所需的準備時間較read/write長，所以file過小的時候使用mmap並沒有顯著的優勢。
3. 另外，f4中的m/m較f/m及m/f慢，我們認為是使用socket時的變異比較大，而非m/f或f/m優於m/m的緣故。比如f1只有50byte，但其f/m的執行時間高達8602ms，遠比f3、f4的同格數據來的大。

結論：socket配上mmap可以很好地改善transmission rate。
(參考資料：https://www.ibm.com/developerworks/cn/linux/l-cn-zerocopy1/index.html)



各組員的貢獻
---
R05246009 應數所碩二 吳其豪

1. 負責 `src/master_device.c` 及 `src/master.c` 的實作
2. 負責實測並記錄四個 `input` 檔案的結果

R06723041 財金所碩一 鄭晏奇
1. 負責 `src/slave_device.c` 及 `src/slave.c` 的實作
2. 負責實測並記錄四個 `input` 檔案的結果

B03201003 數學系大四 林永濬
1. 負責 `src/master_device.c` 及 `src/master.c` 的實作
2. 負責研究 `fcntl` 和 `mmap` 兩 `I/O` 方法之差異

B03201013 數學系大四 郭子翔
1. 負責 `src/slave_device.c` 及 `src/slave.c` 的實作
2. 負責研究 `fcntl` 和 `mmap` 兩 `I/O` 方法之差異
