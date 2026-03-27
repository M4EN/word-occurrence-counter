# Word Frequency Analyzer in C

A C program that counts word occurrences in a text file and prints the **top 10 most frequent words** using three approaches:

- **Naive**
- **Multiprocessing**
- **Multithreading**

It was built to compare performance between a single-process solution and parallel implementations using processes and threads.

---

## What it shows

- Word counting on a large text file
- Dynamic memory allocation
- Shared memory with `mmap()`
- Process creation with `fork()`
- Threading with `pthread`
- Sorting with `qsort()`
- Execution-time comparison between approaches

---

## Build and Run

The program expects the input file to be named `text.txt`.

Compile:

```bash
gcc -o word_counter word_counter.c -pthread
```

Run:

```bash
./word_counter
```

Then choose one of the following:

- `1` = Naive approach
- `2` = Multiprocessing approach
- `3` = Multithreading approach

For the parallel approaches, choose the number of children/threads when prompted.

---

## Results

### Test Environment
- **CPU:** Intel Core i5-7400, 4 cores / 4 threads, 3.00 GHz base frequency
- **Memory:** 16 GB
- **OS:** buntu 24.04 LTS (64-bit), Kernel 6.8.0-49generic
- **IDE:** CLion
- **Dataset** enwik8 (large real-world text corpus)
  
### Performance

| Approach | Children / Threads | Time (Seconds) | Time (Min:Sec) |
|---|---:|---:|---:|
| Naive | 0 | 876.443 | 14 min 36 sec |
| Multiprocessing | 2 | 431.387 | 7 min 11 sec |
| Multiprocessing | 4 | 279.567 | 4 min 39 sec |
| Multiprocessing | 6 | 241.651 | 4 min 01 sec |
| Multiprocessing | 8 | 233.594 | 3 min 53 sec |
| Multithreading | 2 | 405.509 | 6 min 45 sec |
| Multithreading | 4 | 277.425 | 4 min 37 sec |
| Multithreading | 6 | 234.536 | 3 min 54 sec |
| Multithreading | 8 | 224.126 | 3 min 44 sec |

### Takeaways
- Parallel versions were much faster than the naive version.
- Multithreading was slightly faster than multiprocessing on the tested system.
- The best measured result was **8 threads: 224.126 seconds**.

---
