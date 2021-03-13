# Program I: SIMD Programming

###### tags: `Parallel Programming`
##### website: `https://nctu-sslab.github.io/PP-f20/`
## Q1-1
**Q : Does the vector utilization increase, decrease or stay the same as VECTOR_WIDTH changes? Why?**
>Run ./myexp -s 10000 and sweep the vector width from 2, 4, 8, to 16. Record the resulting vector utilization. You can do this by changing the #define VECTOR_WIDTH value in def.h.

A : 當VECTOR_WIDTH增加，utilization減少。因為utilization是用mask計算，而一個lane中如果某些部分已經做完，但還要等其他的部分跑完，此時已經做完的部分就不會被算進utilization，而VECTOR_WIDTH越大，等待的機會就會越大，utilization就會越少。
![](https://i.imgur.com/sHXhWBU.png)

:::info
Use code block next time
> [name=劉安齊]
:::

## Q2-1
**Q : Fix the code to make sure it uses aligned moves for the best performance.**

>Hint: we want to see vmovaps rather than vmovups.
>
original code
```cpp=
  // code without fix
  a = (float *)__builtin_assume_aligned(a, 16);
  b = (float *)__builtin_assume_aligned(b, 16);
  c = (float *)__builtin_assume_aligned(c, 16);
```
my ans
```cpp=
  // code after fixing
  a = (float *)__builtin_assume_aligned(a, 32);
  b = (float *)__builtin_assume_aligned(b, 32);
  c = (float *)__builtin_assume_aligned(c, 32);
```
comparing two files
```shell=
$ diff assembly/test1.vec.restr.align.s assembly/test1.vec.restr.align.avx2.s
```

![](https://i.imgur.com/kon2nkU.png)

A : 因為SSE要求數據16bytes 對齊，而AVX則是要求32bytes。
參考資料 : https://www.cnblogs.com/bonelee/p/9920610.html
## Q2-2
**Q : What speedup does the vectorized code achieve over the unvectorized code? What additional speedup does using -mavx2 give (AVX2=1 in the Makefile)? You may wish to run this experiment several times and take median elapsed times; you can report answers to the nearest 100% (e.g., 2×, 3×, etc). What can you infer about the bit width of the default vector registers on the PP machines? What about the bit width of the AVX2 vector registers.**

>Hint: Aside from speedup and the vectorization report, the most relevant information is that the data type for each array is float.

- Test1.cpp

|Case |command|Time(s)|Speedup|
|-----|--------|---|--|
|1|make && ./test_auto_vectorize -t 1|8.16|1X|
|2|make VECTORIZE=1 && ./test_auto_vectorize -t 1|2.60|3X|
|3|make VECTORIZE=1 AVX2=1 && ./test_auto_vectorize -t 1|1.35|6X|

- Test2.cpp

|Case |command|Time(s)|Speedup|
|-----|--------|---|--|
|1|make && ./test_auto_vectorize -t 2|10.94|1X|
|2|make VECTORIZE=1 && ./test_auto_vectorize -t 2|2.61|4X|
|3|make VECTORIZE=1 AVX2=1 && ./test_auto_vectorize -t 2|1.35|8X|

- Test3.cpp

|Case |command|Time(s)|Speedup|
|-----|--------|---|--|
|1|make && ./test_auto_vectorize -t 3|21.67|1X|
|2|make VECTORIZE=1 && ./test_auto_vectorize -t 3|5.47|4X|
|3|make VECTORIZE=1 AVX2=1 FASTMATH=1 && ./test_auto_vectorize -t 3|1.49|16X|

A : 
(1) Case 2(vectorized) 是Case 1(unvectorized) 的4倍快
(2) Case 3(AVX)是Case 2的2倍快
(3) 推斷default vector register是128 bit width
(4) 推斷AVX2是256 bit width
## Q2-3
**Q : Provide a theory for why the compiler is generating dramatically different assembly.**
A : 執行了before patch的版本，發現這段code無法vectorized，而compiler給出了這樣的資訊" loop not vectorized: unsafe dependent memory operations in loop"，推測是因為在before patching，不管怎樣都會執行c[j] = a[j]，而如果進到if裡面，c[j]又會被assign一次，所以一個迴圈下來可能會做2次assign，但after patching則是先做if else判斷，所以不會有上述的問題發生

```cpp=
 //before patching
 for (int j = 0; j < N; j++){
   /* max() */
  c[j] = a[j];
  if (b[j] > a[j])
    c[j] = b[j];
}

```
```cpp=
 //after patching
 for (int j = 0; j < N; j++){
   /* max() */
    if (b[j] > a[j]) 
        c[j] = b[j];
    else 
        c[j] = a[j];
}

```
The difference
![](https://i.imgur.com/5nytzoW.png)

