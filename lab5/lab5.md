# Assignment V: CUDA Programming
###### tags: `Parallel Programming`

#### Q1 What are the pros and cons of the three methods? Give an assumption about their performances.
* method1和2是以pixel為運算的單位，比起method3來說，一個function只要算完那個pixel的結果就結束，而method3則是讓每個function都執行複數個pixel，所以method1和2會較3來的快
* 在method2和3，我們用了`cudaMallocPitch`取代原本的`cudaMalloc`，雖然用法比較麻煩，但是他allocate memory的方式會來得比較快

#### Q2 How are the performances of the three methods? Plot a chart to show the differences among the three methods
for VIEW 1 and VIEW 2, and for different maxIteration (1000, 10000, and 100000)
- View 1 之比較(秒數後為與ref的speedup比較)

| maxIteration | 1000            | 10000           | 100000           |
| ------------ | --------------- | --------------- | ---------------- |
| kernel1      | 14.943ms(1.06x) | 41.222ms(1.99x) | 323.301ms(1.05x) |
| kernel2      | 11.292ms(1.4x)  | 36.582ms(1.82x) | 323.125ms(1.05x) |
| kernel3      | 17.866ms(0.91x) | 67.41ms(1.22x)  | 591.613ms(0.57x) |
![](https://i.imgur.com/ayVWsDg.png)

- View 2 之比較(秒數後為與ref的speedup比較)

| maxIteration | 1000           | 10000           | 100000          |
| ------------ | -------------- | --------------- | --------------- |
| kernel1      | 7.453ms(0.9x)  | 15.079ms(1.10x) | 41.674ms(1.84x) |
| kernel2      | 7.609ms(0.88x) | 14.507ms(1.14x) | 41.066ms(1.89x) |
| kernel3      | 12.48ms(0.55x) | 28.43ms(0.59x)  | 76.209ms(1.09x) |
![](https://i.imgur.com/dmr52VZ.png)


#### Q3 Explain the performance differences thoroughly based on your experimental results. Does the results match your assumption? Why or why not.
不管是view1或者view2，三種的performance都是kernel2>kernel1>kernel3。
kernel1與kernel2的差距較小，kernel2與kernel3的差距較大，這也符合上一題的預測。
kernel1與kernel2只差在allocate的方式不同，每個thread都是算一個pixel，所以時間上只會有微小的差距；
而kernel2與kernel3差距較大，則是因為他們算法上的不同，kernel3是一個thread算一個group，即使我加上了一些條件判斷，讓運算過的pixel盡量不要再運算，但難免還是會有重複計算的部分，所以重複計算造成了運算上的浪費，導致performance變差。
#### Q4 Can we do even better? Think a better approach and explain it. Implement your method in kernel4.