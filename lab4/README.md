# Assignment IV: MPI Programming

###### tags: `Parallel Programming`

## Q1: MPI Hello World, Getting Familiar with the MPI Environment
1. How do you control the number of MPI processes on each node? (1 points)
A:可以在host file上加slots參數，例如有兩個node可以用，要設定一個node有兩個process的話就是
```cpp=
# host file
pp1 slots=2
pp2 slots=2
```
2. Which functions do you use for retrieving the rank of an MPI process and the total number of processes? (1 points)
A:MPI_Comm_rank and MPI_Comm_size ( MPI_Comm comm, int *size )
3. We use Open MPI for this assignment. What else MPI implementation is commonly used? What is the difference between them? (1 points)
A:MPICH
MPICH targets special purpose needs. Open-MPI targets the common case, both in terms of usage and network conduits.

## Q2 : MPI Blocking Communication & Linear Reduction Algorithm

1.Why MPI_Send and MPI_Recv are called “blocking” communication? (1 points)
A:因為他們會在確認send/receive完，data是safe的情況下才會return(non-blocking會直接return)
2. Measure the performance (execution time) of the code for 2, 4, 8, 12, 16 MPI processes and plot it. (1 points)

| np      | 2      | 4      | 8      | 12     | 16     |
| ------- | ------ | ------ | ------ | ------ | ------ |
| time(s) | 8.0947 | 4.0568 | 2.0163 | 1.2058 | 1.0311 |



## Q3 : MPI Blocking Communication & Binary Tree Reduction Communication Algorithm

1. Measure the performance (execution time) of the code for 2, 4, 8, 16 MPI processes and plot it. (1 points)
 
| np      | 2      | 4      | 8      | 16     |
| ------- | ------ | ------ | ------ | ------ |
| time(s) | 8.0206 | 4.0015 | 2.2631 | 1.6932 |

2. How does the performance of binary tree reduction compare to the performance of linear reduction? (2 points)
A:在np等於2的時候兩者的時間差不多，但提高np後，binary tree的效率就會比linear來的差。

3. Increasing the number of processes, which approach (linear/tree) is going to perform better? Why? Think about the number of messages and their costs. (3 points)
A:以實驗的結果看，當process的數量增加，linear的效率會比較好。但照理來說linear都是rank 0要接收所有的send，在接收的message一樣的情況下，binary tree應該要比較快才對。

## Q4 : MPI Non-Blocking Communication & Linear Reduction Algorithm
1. Measure the performance (execution time) of the code for 2, 4, 8, 12, 16 MPI processes and plot it. (1 points)

| np      | 2      | 4      | 8      | 12     | 16     |
| ------- | ------ | ------ | ------ | ------ | ------ |
| time(s) | 8.0459 | 3.7890 | 1.8540 | 1.1543 | 1.0930 |

2. What are the MPI functions for non-blocking communication? (1 points)
A:MPI_IRecv/MPI_Wait/MPI_Waitall
3. How the performance of non-blocking communication compares to the performance of blocking communication? (3 points)
A:Non-blocking的performance會比blocking的好，我想主要就是差在MPI_Irecv的應用
## Q5 : MPI Collective: MPI_Gather
1. Measure the performance (execution time) of the code for 2, 4, 8, 12, 16 MPI processes and plot it. (1 points)
 
| np      | 2      | 4      | 8      | 12     | 16     |
| ------- | ------ | ------ | ------ | ------ | ------ |
| time(s) | 8.5523 | 4.0319 | 2.1818 | 1.4653 | 1.1158 |


## Q6 : MPI Collective: MPI_Reduce
1. Measure the performance (execution time) of the code for 2, 4, 8, 12, 16 MPI processes and plot it. (1 points)

| np      | 2      | 4      | 8      | 12     | 16     |
| ------- | ------ | ------ | ------ | ------ | ------ |
| time(s) | 8.3920 | 4.0921 | 2.2682 | 1.5168 | 1.1135 |

## Q7 : MPI Windows and One-Sided Communication & Linear Reduction Algorithm
1. Measure the performance (execution time) of the code for 2, 4, 8, 12, 16 MPI processes and plot it. (1 points)

| np      | 2      | 4      | 8      | 12     | 16     |
| ------- | ------ | ------ | ------ | ------ | ------ |
| time(s) | 8.3356 | 4.0757 | 2.1575 | 1.6531 | 1.2204 |


2. Which approach gives the best performance among the 1.2.1-1.2.6 cases? What is the reason for that? (3 points)
A:Non-blocking linear的performance是最好的，以演算法來說，linear應該會劣於binary tree(Q3的第三題有說明)，而因為這是個測資小的小實驗，可能看不太出明顯的差距，不過如果是比較複雜的程式，可能結果就不一定了
3. Which algorithm or algorithms do MPI implementations use for reduction operations? You can research this on the WEB focusing on one MPI implementation. (1 points)

## Q8 (9 points)

1. Plot ping-pong time in function of the message size for cases 1 and 2, respectively. (2 points)!
x軸-message size(byte)
y軸-time(s)
**case 1**
![](https://i.imgur.com/6coSec4.png)
**case 2**
![](https://i.imgur.com/XSckZSA.png)


2. Calculate the bandwidth and latency for cases 1 and 2, respectively. (3 points)


| Case | bandwidth          | latency(ms)         |
| ---- | ------------------ | --------------- |
| 1    | 2,959,289,627.5  | 0.00110196033  |
| 2    | 73,082,230.9 | 0.014651926132004672 |

4. For case 2, how do the obtained values of bandwidth and latency compare to the nominal network bandwidth and latency of the NCTU-PP workstations. What are the differences and what could be the explanation of the differences if any? (4 points)
以交大來說，理想的bandwidth是1Gb/s, lantency是0 ms，不過實驗的結果為bandwidth較小，latency較大，會影響的因素可能有
* propagation delay：封包在網路線上傳輸所花費的時間
* transmission delay：網路卡將資料傳送到網路線上所花的時間，與網路設備的傳送速度有關
* nodal processing delay：路由器處理封包表頭（packet header）、檢查位元資料錯誤與尋找配送路徑等所花費的時間。
* 路由器因為某些因素無法立刻將封包傳送到網路上，造成封包暫存在佇列（queue）中等待的時間。
參考資料:
[Linux ping指令 - iT 邦幫忙 - iThome](https://ithelp.ithome.com.tw/articles/10210473)
[speed test](https://www.speedtest.net/result/10563298977)
