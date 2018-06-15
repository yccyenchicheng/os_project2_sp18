Operating System Project 2 Report Spring 2018 NTU
===

程式的設計
---
- `src/master_device/` : 裡面包含
    - `Makefile`: 
    - `master_device.c`:
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
- `./input/f1.in`

| method/method | master | slave |
| -------- | --------                   | --------                |
| f/f      | 0.065 ms, 50 bytes| 5574.759 ms, 50 bytes |
| f/f      | 2790.568 ms, 50 bytes|0.801 ms, 50 bytes |
| -------- | -------- | -------- |
| f/m      |     0.065 ms,   50 bytes |     4295.752 ms,   50 bytes|
| f/m      |     8602.359000 ms,   50 bytes |     0.817000 ms,   50 bytes|
| -------- | -------- | -------- |
| m/f      |     1.935 ms,   50 bytes |     4717.523 ms,   50 bytes|
| m/f      |     3157.283 ms,   50 bytes |     0.991 ms,   50 bytes |
| -------- | -------- | -------- |
| m/m      |     1.843 ms,   50 bytes |     3876.096 ms,   50 bytes |
| m/m      |     4055.385 ms,   50 bytes|     0.895 ms,   50 bytes |


- `./input/f2.in`

| method/method | master | slave |
| -------- | --------                   | --------                |
| f/f      | 2.648    ms, 500 bytes| 4206.161 ms, 500 bytes |
| f/f      | 6352.568 ms, 500 bytes| 0.968 ms, 500 bytes |
| -------- | -------- | -------- |
| f/m      |     1.869 ms,   500 bytes |     4795.623 ms,   500 bytes|
| f/m      |     9259.24 ms,   500 bytes |     1.305 ms,   500 bytes|
| -------- | -------- | -------- |
| m/f      |     1.83 ms,   500 bytes |     2386.041 ms,   500 bytes|
| m/f      |     6024.737 ms,   500 bytes |     0.908 ms,   500 bytes |
| -------- | -------- | -------- |
| m/m      |     0.069 ms,   500 bytes |     4187.648 ms,   500 bytes |
| m/m      |     4760.073 ms,   500 bytes|     0.849 ms,   500 bytes |

- `./input/f3.in`

| method/method | master | slave |
| -------- | --------                   | --------                      |
| f/f      | 8.103000 ms, 260416 bytes| 821.297000 ms, 260416 bytes |
| f/f      | 9960.744000 ms, 260416 bytes|7.472000 ms, 260416 bytes |
| -------- | -------- | -------- |
| f/m      |     6.049000 ms,   260416 bytes |     1282.259000 ms,   260416 bytes|
| f/m      |     5692.584000 ms,   260416 bytes |     4.848000 ms,   260416 bytes|
| -------- | -------- | -------- |
| m/f      |     4.349000 ms,   260416 bytes |     1366.666000 ms,   260416 bytes|
| m/f      |     1182.905000 ms,   260416 bytes |     3.477000 ms,   260416 bytes |
| -------- | -------- | -------- |
| m/m      |     2.163000 ms,   260416 bytes |     704.473000 ms,   260416 bytes |
| m/m      |     1065.158000 ms,   260416 bytes|     1.739000 ms,   260416 bytes |

- `./input/f4.in`

| method/method | master | slave |
| -------- | --------                   | --------                       |
| f/f      |     2.964000 ms,   70920 bytes|     2694.853000 ms,   70920 bytes |
| f/f      |     863.115000 ms,   70920 bytes| : 2.040000 ms,   70920 bytes |
| -------- | -------- | -------- |
| f/m      |     2.420000 ms,   70920 bytes |     921.281000 ms,   70920 bytes|
| f/m      |     1184.849000 ms,   70920 bytes |     3.145000 ms,   70920 bytes|
| -------- | -------- | -------- |
| m/f      |     2.054000 ms,   70920 bytes |     1243.163000 ms,   70920 bytes|
| m/f      |     1215.914000 ms,   70920 bytes |     1.863000 ms,   70920 bytes |
| -------- | -------- | -------- |
| m/m      |     1.815000 ms,   70920 bytes |     2102.905000 ms,   70920 bytes |
| m/m      |     1220.666000 ms,   70920 bytes |     1.709000 ms,   70920 bytes |


file I/O v.s. memory-mapped I/O & 差異的原因
---
    當我們使用file I/O的時候，資料傳輸的路徑為：
        disk -> kernel space -> user space -> socket space (master)
             ->socket space -> user space -> kernel space -> disk (slave)
    而使用mmap的話可以略過kernel space，即
        disk -> user space -> socket space (master)
             ->socket space -> user space -> disk (slave)
    故使用mmap可以改善transmission rate。
示意圖：
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
