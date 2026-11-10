# Master Context & Architectural Plan: Custom High-Performance C++ Logging Engine

## Project Purpose & Duration
- **Goal:** Deeply master C++ systems programming, kernel internals, storage I/O, memory architectures, concurrency, and Linux performance tuning by building a custom high-performance, disk-bound logging engine from scratch over 1–2 months.
- **Scope:** Progress iteratively from basic file streams (V1) through kernel-bypass I/O, asynchronous ring buffers (`io_uring`), lock-free atomics, memory-mapped files (`mmap`), NUMA optimizations, and eBPF kernel tracing (V10).

---

## Pedagogical Protocol & Mentorship Rules
1. **No Code Copy-Pasting:** The AI assistant MUST NEVER provide complete or directly copy-pasteable C++ code implementations. The user writes 100% of the project code independently.
2. **Role:** The AI acts strictly as a senior systems architect, engineering mentor, and technical reviewer—providing conceptual deep-dives, architectural guidance, diagnostic challenges, and debugging advice.
3. **Hardware & OS Observability Mandatory:** Every single milestone MUST be paired with live system-level metrics and telemetry verification (RAM page caches, CPU states, syscall latencies, disk block queues).
4. **Seamless Session Handoff:** This `.md` file serves as the canonical session state. Feeding this file into any AI model must fully restore all rules, architectural plans, and current progress.

---

## Complete Systems Engineering Roadmap (V1 – V10)

- **V1: Basic Buffered File Logging**
  - *Mechanism:* Standard `std::ofstream` and `std::ios::app`.
  - *Characteristics:* High user-space and kernel page-cache absorption. Applications report instant completion while data sits dirty in RAM.
- **V2: POSIX Synchronous Disk Writes (`O_SYNC` / `fsync`)**
  - *Mechanism:* Low-level POSIX file descriptors (`open()`, `write()`, `close()`) with `O_SYNC` or `fsync()`.
  - *Characteristics:* Bypasses deferred writeback; forces `write()` to block until physical storage hardware confirms persistence.
- **V3: Thread-Safe Multi-Producer Logging (Mutex Queues)**
  - *Mechanism:* Concurrent producer threads submitting to a locked queue with a dedicated background consumer writer thread.
  - *Characteristics:* Prevents log interleaving/corruption; introduces context switching and lock contention.
- **V4: Vectorized Scatter-Gather I/O (`writev`)**
  - *Mechanism:* Non-contiguous buffer aggregation using `struct iovec` arrays.
  - *Characteristics:* Reduces system call transition frequency and eliminates unnecessary user-space string concatenations.
- **V5: Kernel Page Cache Bypass via Direct I/O (`O_DIRECT`)**
  - *Mechanism:* Direct Memory Access (DMA) from aligned user-space memory directly to storage media.
  - *Characteristics:* Requires strict sector-aligned memory buffers (`posix_memalign`); zero kernel cache overhead.
- **V6: Zero-Copy Memory-Mapped Files (`mmap`)**
  - *Mechanism:* Virtual address mapping (`mmap()`, `MAP_SHARED`) with `ftruncate()`.
  - *Characteristics:* Offloads file I/O to the Virtual Memory Manager (VMM), replacing `write()` syscalls with pointer operations.
- **V7: High-Throughput Asynchronous I/O via `io_uring`**
  - *Mechanism:* Shared memory ring buffers (Submission Queue & Completion Queue) interacting directly with the Linux kernel block layer.
  - *Characteristics:* Completely decouples application submit threads from I/O completion; ultra-high IOPS.
- **V8: Lock-Free Multi-Producer Single-Consumer (MPSC) Ring Buffer**
  - *Mechanism:* Circular atomic buffer utilizing memory barriers (`std::memory_order_release`/`acquire`) and cache-line alignment (`alignas(64)`).
  - *Characteristics:* Eliminates CPU thread sleep states, OS mutex locks, and false sharing on hardware CPU cores.
- **V9: NUMA-Aware Memory Locality & Thread Binding**
  - *Mechanism:* Socket-specific memory allocations (`libnuma`) and core affinity pinning (`pthread_setaffinity_np`).
  - *Characteristics:* Avoids cross-socket interconnect bottlenecks on modern multi-socket server architectures.
- **V10: Deep Kernel Observability & Tracing via eBPF**
  - *Mechanism:* Sandboxed eBPF C programs attached to kernel tracepoints (`sys_enter_write`, `block_rq_issue`) compiled via `libbpf`.
  - *Characteristics:* Microsecond-level telemetry of block device queues, page cache hits/misses, and kernel latency histograms.

---

## Core Systems & OS Internals Knowledge Base

### 1. Kernel Page Cache & Deferred Writeback
- Standard `write()` system calls copy data from application RAM to kernel RAM (the page cache).
- Pages are marked `Dirty` in `/proc/meminfo`.
- Background kernel threads (`flush-*` / `pdflush` / `kswapd`) write dirty pages to hardware when thresholds like `vm.dirty_background_ratio` or `vm.dirty_bytes` are exceeded.
- **Large Writes Illusion:** Writing large chunks (e.g., 10MB) instantly trips the kernel dirty background threshold, waking up flush threads immediately and giving the illusion of direct hardware writing, even though the call returns before bytes hit disk sectors.

### 2. POSIX Low-Level I/O & `O_SYNC` Mechanics
- Standard C++ streams wrap buffer management. Dropping down to raw POSIX calls requires handling raw integer file descriptors.
- `O_SYNC` forces the kernel to wait for physical storage controller confirmation before returning from `write()`.
- **Short Writes & Interrupts:** POSIX `write()` may write fewer bytes than requested (due to signal interruption `EINTR` or disk space constraints). Systems-level code MUST implement a robust retry loop tracking byte offsets until the entire buffer is written.

### 3. Hardware Storage Write-Back Caches
- Consumer storage drives (USB flash, SSDs) often report successful writes to the OS when data hits their internal volatile RAM cache.
- Full transactional safety requires hardware flush commands (`fsync` combined with storage barriers) or disabling device write-back caches via storage management tools.

---

## Live System Observability Matrix

At each milestone, software execution must be verified using live Linux observability metrics across four distinct layers:

| Layer | System Tool | Specific Metric / Metric Path | Purpose / Target Behavior |
| :--- | :--- | :--- | :--- |
| **Memory Subsystem** | `/proc/meminfo`, `vmstat`, `free` | `Dirty:`, `Writeback:`, `Cached:` | Prove page cache bypass (`Dirty` stays flat at `0 kB` under `O_SYNC`/`O_DIRECT`). |
| **Block Device I/O** | `iostat -xz 1`, `blktrace` | `kB_wrtn/s`, `w_await`, `aqu-sz` | Observe physical write throughput and hardware queue depth in real time. |
| **System Call Layer** | `strace -T -c`, `perf` | Syscall duration, count, `EINTR` errors | Measure execution latency in user-space vs. kernel-space transitions. |
| **Process Resource** | `pidstat -d -u 1` | `kB_wr/s`, `%CPU`, `%system` | Isolate thread CPU consumption and per-process disk write performance. |

---
