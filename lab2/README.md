# Program II: Multi-thread

###### tags: `Parallel Programming`

## Q1: 

**Is speedup linear in the number of threads used? In your writeup hypothesize why this is (or is not) the case?(You may also wish to produce a graph for VIEW 2 to help you come up with a good answer.**
> In your write-up, produce a graph of speedup compared to the reference sequential implementation as a function of the number of threads used FOR VIEW 1.
> Hint: take a careful look at the three-thread data-point.

- Time graph
可以發現在view 1 中，3個thread的效率非常的差，甚至比2個thread還慢，推測可能是thread的loading分配不均，導致某個thread拖慢了整體的速度。
![](https://i.imgur.com/USrVisR.png)


## Q2
**How do your measurements explain the speedup graph you previously created?**
> To confirm (or disprove) your hypothesis, measure the amount of time each thread requires to complete its work by inserting timing code at the beginning and end of workerThreadStart(). 

- View 1 之比較

| Number of threads | thread    0 | thread    1 | thread    2 | thread    3 |
| ----------------- | ----------- | ----------- | ----------- | ----------- |
| 2                 | 0.236       | 0.23        | X           | X           |
| 3                 | 0.093       | **0.282**   | 0.094       | X           |
| 4                 | 0.046       | **0.193**   | **0.192**   | 0.0.46      |

- View 2 之比較

| Number of threads | thread    0 | thread    1 | thread    2 | thread    3 |
| ----------------- | ----------- | ----------- | ----------- | ----------- |
| 2                 | 0.114       | 0.162       | X           | X           |
| 3                 | **0.124**       | 0.074       | 0.079       | X           |
| 4                 | **0.106**       | 0.059       | 0.060       | 0.057       |

- 結論 : 在view 1 中，可以發現中間的thread特別慢，在view 2中則是前面的thread比較慢，推測是這些thread的loading比較重(運算比較多次等原因)才造成整體速度減緩，而打開這些圖看，根據算法，越白色的地方就是運算較多的地方，因此符合以上的猜測。
(view 1 - 中間白色的部分剛好是比較慢的thread負責的地方)
![](https://i.imgur.com/CnL3MR7.png)
(view 2 - 上面白色的部分剛好是比較慢的thread負責的地方)
![](https://i.imgur.com/SGCDRQk.png)

## Q3
**Q3: In your write-up, describe your approach to parallelization and report the final 4-thread speedup obtained.**
> Modify the mapping of work to threads to achieve to improve speedup to at about 3-4x on both views of the Mandelbrot set (if you’re above 3.5x that’s fine, don’t sweat it). You may not use any synchronization between threads in your solution. We are expecting you to come up with a single work decomposition policy that will work well for all thread counts—hard coding a solution specific to each configuration is not allowed! (Hint: There is a very simple static assignment that will achieve this goal, and no communication/synchronization among threads is necessary.).

- 原本的算法
原先是根據thread將要算的部分分區規劃好，這樣的做法加上這個函式的特性就會史的中間的thread跑得特別慢
 ![](https://i.imgur.com/GQZ9sJv.png)
- 調整的算法
改成每個row輪流用不同的thread去算，使得運算時間平均，沒有loading過重的thread，整體的速度也可以提升
(藍色-thread 0 ; 紅色-thread 1 ; 綠色 - thread 2)
![](https://i.imgur.com/UWYJXWj.png)

- view 1 (調整後)，可以看到速度有很大的提升，每個thread的時間也平均許多，而在thread 4時，speed up為3.78X
![](https://i.imgur.com/jjvkz9m.png)
```cpp=
 ./mandelbrot --threads 3 --view 1
thread 1 use 0.0928255 seconds
thread 0 use 0.0929809 seconds
thread 2 use 0.0930766 seconds
(2.89x speedup from 3 threads)
```

- view 2 (調整後)，可以看到速度有很大的提升，每個thread的時間也平均許多，而在thread 4時，speed up為3.77X
![](https://i.imgur.com/cy5sJhl.png)
```cpp=
 ./mandelbrot --threads 3 --view 2
thread 0 use 0.159109 seconds
thread 2 use 0.158294 seconds
thread 1 use 0.158303 seconds
(2.90x speedup from 3 threads)
```

:::info
Good! 
> [name=劉安齊]
:::

## Q4
**Now run your improved code with eight threads. Is performance noticeably greater than when running with four threads? Why or why not? (Notice that the workstation server provides 4 cores 4 threads.)**

把每個thread的運算時間output出來後可以發現，照理來說越快結束的thread output出來的順序也會越早(以not improved的來看會更明顯)，所以推斷使用8 thread的時候，並不會同時產生8個thread，而是等前面的thread結束後才產生，因此速度上也與4 thread時不會差多少

```cpp=
(improved)
 ./mandelbrot --threads 8 --view 2
thread 0 use 0.0357081 seconds
thread 3 use 0.0381864 seconds
thread 5 use 0.0442574 seconds
thread 1 use 0.0679597 seconds
thread 6 use 0.0530151 seconds
thread 2 use 0.0680387 seconds
thread 4 use 0.0634997 seconds
thread 7 use 0.0560205 seconds
(3.75x speedup from 8 threads)
```

```cpp=
(not improved)
 ./mandelbrot --threads 4 --view 2
thread 1 use 0.0591407 seconds
thread 2 use 0.0597448 seconds
thread 3 use 0.0571655 seconds
thread 0 use 0.105694 seconds
```
