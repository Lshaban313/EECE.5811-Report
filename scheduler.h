#pragma once

#include "memory_manager.h"
#include <cstdint>

/**
 * Abstract interface for our synthetic scheduler.
 * run(pct) returns one simulated latency measurement in µs.
 */
class Scheduler {
public:
    virtual ~Scheduler() = default;
    virtual uint64_t run(int cpu_load_pct) = 0;

protected:
    Scheduler(MemoryManager* mgr) : memMgr(mgr) {}

    // synthetic workload parameters:
    static constexpr int    NUM_CYCLES   = 500;
    static constexpr double BASE_LATENCY = 15000.0;  // µs
    static constexpr double PER_PERCENT  = 20.0;     // µs per % load
    static constexpr double BASE_JITTER  = 100.0;    // σ in µs

    MemoryManager* memMgr;
};
