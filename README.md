# High-Performance C++ Systems Data Saver: Architecture & Roadmap

This document outlines an expanded, progressive engineering roadmap to transform a basic C++ logging implementation into an ultra-low-latency, asynchronous, kernel-bypass systems application. Each milestone introduces deeper operating system internals, memory management constraints, and hardware-level optimizations.

Only Blocker is the usb drive that I am working on.. it's literally an 8$ USB drive from Amazon in the times when drive prices are at all time high.

## Roadmap Overview

- [x] V1: Buffered Streams
- [x] V2: Synchronous O_SYNC
- [ ] V3: Mutex Queues
- [ ] V4: Vectorized writev
- [ ] V5: O_DIRECT Bypass
- [ ] V6: Memory-Mapped mmap
- [ ] V7: io_uring Async
- [ ] V8: Lock-Free Atomics
- [ ] V9: NUMA-Aware Buffering
- [ ] V10: eBPF Kernel Tracing

---

## Milestone Details

### V1: Basic Buffered File Logging
* **Objective:** Establish a baseline logger using high-level C++ standard streams (`std::ofstream`).
* **Implementation:** Open files with `std::ios::app` and use stream insertion operators (`logFile_ << data`).
* **System Characteristics:** Relies entirely on user-space and kernel page-cache buffering. Write returns instantly to application space while data sits safely in RAM.
* **Verification:** Monitor high `Cached` and `Dirty` metrics in `/proc/meminfo` and large quiet intervals in `iostat`.

### V2: Synchronous Physical Writes (`O_SYNC` / `fsync`)
* **Objective:** Eliminate page-cache masking by enforcing that write operations block until data is physically committed to storage media.
* **Implementation:** Transition from streams to POSIX file descriptors (`open()`) using the `O_SYNC` flag or issuing manual `fsync(fd)` calls.
* **System Characteristics:** Application execution is throttled by actual physical storage latency (e.g., USB flash or disk controller speed).
* **Verification:** Verify `/proc/meminfo` `Dirty` metric would rise a lot and also the Writeback (as we are throttled by USB flash slowness ) and `iostat` shows continuous write throughput.

### V3: Thread-Safe Multi-Producer Logging (Mutex Queues)
* **Objective:** Allow multiple concurrent threads to submit logs safely without file corruption or race conditions.
* **Implementation:** Implement a producer-consumer pattern using `std::mutex`, `std::condition_variable`, and a shared message queue, drained by a single background worker thread.
* **System Characteristics:** Solves race conditions but introduces OS-level thread blocking, context switching overhead, and lock contention under high core counts.
* **Verification:** Stress test with 16+ concurrent threads and verify chronological ordering of timestamps in output logs.

### V4: Vectorized Scatter-Gather I/O (`writev`)
* **Objective:** Reduce system call overhead and user-space memory allocations by writing non-contiguous memory buffers (e.g., timestamp header + log payload + newline) in a single atomic syscall.
* **Implementation:** Populate an array of `struct iovec` descriptors pointing to separate memory locations and pass them to the `writev()` system call.
* **System Characteristics:** Minimizes unnecessary string concatenation buffers in RAM and reduces user-to-kernel transition frequency.
* **Verification:** Use `strace` to confirm multiple log components are dispatched via a single `writev` syscall rather than multiple individual `write` calls.

### V5: Kernel Bypass via Direct I/O (`O_DIRECT`)
* **Objective:** Bypass the kernel page cache entirely during writes, streaming data directly from application RAM to physical storage via Direct Memory Access (DMA).
* **Implementation:** Open files with `O_DIRECT` and allocate memory buffers aligned to sector boundaries (typically 512 or 4096 bytes) using `posix_memalign()`.
* **System Characteristics:** Eliminates kernel memory copying overhead; however, unaligned buffer sizes or memory addresses will cause `write()` to fail instantly with `EINVAL`.
* **Verification:** Observe zero fluctuation in system `Cached` memory during massive logging workloads.

### V6: Zero-Copy Memory-Mapped Files (`mmap`)
* **Objective:** Eliminate explicit `write()` system calls by mapping the target log file directly into the process's virtual address space.
* **Implementation:** Allocate file size with `ftruncate()`, map it via `mmap()` with `MAP_SHARED`, and write log entries using direct pointer arithmetic (`memcpy`).
* **System Characteristics:** Offloads data transfer management to the operating system's Virtual Memory Manager (VMM), reducing context switches.
* **Verification:** Inspect memory mappings via `pmap` or `/proc/<pid>/maps` and profile reduced CPU time spent in system calls.

### V7: Modern Asynchronous I/O via `io_uring`
* **Objective:** Achieve high-throughput, non-blocking asynchronous file operations using the modern Linux kernel ring-buffer interface.
* **Implementation:** Initialize an `io_uring` ring with submission queue (SQ) and completion queue (CQ) buffers, pushing write requests directly without synchronous blocking.
* **System Characteristics:** Eliminates traditional syscall bottlenecks, allowing the kernel and application to exchange I/O operations through shared memory rings.
* **Verification:** Measure high IOPS and low p99 latencies compared to standard synchronous file access.

### V8: Lock-Free Multi-Producer Single-Consumer (MPSC) Ring Buffer
* **Objective:** Eliminate CPU lock contention, mutex overhead, and thread sleep/wake cycles under heavy multi-core logging workloads.
* **Implementation:** Build a bounded circular buffer using `std::atomic` operations, memory ordering (`std::memory_order_release` / `acquire`), and 64-byte cache-line padding (`alignas(64)`) to prevent false sharing.
* **System Characteristics:** Ensures predictable, ultra-low latency log collection across competing threads without OS scheduler intervention.
* **Verification:** Run hardware performance counter diagnostics (`perf c2c`) to verify zero cache-line bouncing between CPU cores.

### V9: NUMA-Aware Memory Allocation & Batching
* **Objective:** Optimize memory locality in multi-socket server architectures to prevent cross-socket bus latency bottlenecks.
* **Implementation:** Bind logger memory allocations and background worker threads to specific Non-Uniform Memory Access (NUMA) nodes using `libnuma` (`numa_alloc_onnode`).
* **System Characteristics:** Keeps memory traffic local to the CPU core executing the logging logic, drastically reducing inter-socket communication overhead.
* **Verification:** Profile memory access latencies using hardware tools (`numastat`) under heavy multi-socket workloads.

### V10: Kernel Observability & Tracing via eBPF
* **Objective:** Gain real-time, zero-overhead telemetry of internal kernel block layer performance, page cache states, and actual hardware request latencies.
* **Implementation:** Write lightweight C programs compiled into eBPF bytecode, attaching them to kernel tracepoints (`sys_enter_write`, `block_rq_issue`) using `libbpf`.
* **System Characteristics:** Provides safe, sandboxed, microsecond-level visibility directly inside the kernel execution path without modifying application binaries.
* **Verification:** Generate real-time histograms of end-to-end disk request latencies and correlate application timestamps with block layer hardware completions.
 
