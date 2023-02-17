## Class in NCTU 109 
## Programming Assignment I: SIMD Programming
- Part 1: Vectorizing Code Using Fake SIMD Intrinsics
- Part 2: Vectorizing Code with Automatic Vectorization Optimizations
- The purpose of this assignment is to familiarize yourself with SIMD (single instruction, multiple data) programming. Most modern processors include some form of vector operations (i.e., SIMD instructions), and some applications may take advantage of SIMD instructions to improve performance through vectorization. Although modern compilers support automatic vectorization optimizations, the capabilities of compilers to fully auto-vectorize a given piece of code are often limited. Fortunately, almost all compilers (targeted to processors with SIMD extensions) provide SIMD intrinsics to allow programmers to vectorize their code explicitly.

## Programming Assignment II: Multi-thread Programming
- Part 1: Parallel Counting PI Using Pthreads
- Part 2: Parallel Fractal Generation Using std::thread
- The purpose of this assignment is to familiarize yourself with Pthread and std::thread programming in C and C++, respectively. You will also gain experience measuring and reasoning about the performance of parallel programs (a challenging, but important, skill you will use throughout this class). This assignment involves only a small amount of programming, but a lot of analysis!

## Programming Assignment III: OpenMP Programming
- Part 1: Parallelizing Conjugate Gradient Method with OpenMP
- Part 2: Parallelizing PageRank Algorithm with OpenMP
- The purpose of this assignment is to familiarize yourself with OpenMP programming. In Part 1, conjugate gradient method is an algorithm for the numerical solution of particular systems of linear equations. It is often used to solve partial differential equations, or applied on some optimization problems. In second part, you will implement two graph processing algorithms: breadth-first search (BFS) and a simple implementation of page rank. A good implementation of this assignment will be able to run these algorithms on graphs containing hundreds of millions of edges on a multi-core machine in only seconds.

## Assignment IV: MPI Programming
- Part 1: Getting Familiar with MPI Programming
    - Getting Familiar with the MPI Environment 
    - Calculating PI with MPI
    - Masuring Bandwidth and Latency on NCTU-PP workstations with Ping-Pong
- Part 2: Matrix Multiplication with MPI
The purpose of this assignment is to familiarize yourself with MPI programming.

## Assignment V: CUDA Programming
- Parallelize the computation of the images using CUDA
- The purpose of this assignment is to familiarize yourself with CUDA programming.

## Assignment VI: OpenCL Programming
- Image convolution using OpenCL
- The purpose of this assignment is to familiarize yourself with OpenCL programming.

## Final project: Implement Parallelization on Image Stitching
在手機普及的年代，利用手機拍一張相片很容易，但如果要融合多張相片的話，就需要image stitching的方法，而在過程中，會使用到名為RANSAC (RANdom SAmple Consensus) 的演算法，是一個歷史悠久且能夠廣泛運用在各個領域上的演算法，也是本次報告的首要目標。
