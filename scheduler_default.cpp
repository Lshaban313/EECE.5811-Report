#include "scheduler.h"
#include <random>

class DefaultScheduler : public Scheduler {
public:
    DefaultScheduler(MemoryManager* mgr)
        : Scheduler(mgr),
          rng_(123456),
          dist_(0.0, 1.0) {}

    uint64_t run(int cpu_load_pct) override {
        // simulate an allocation
        void* buf = memMgr->allocate();
        
        // compute latency sample
        double mean   = BASE_LATENCY + cpu_load_pct * PER_PERCENT;
        double sigma  = BASE_JITTER;
        std::normal_distribution<double> nd(mean, sigma);
        double sample = nd(rng_);
        if (sample < 0.0) sample = 0.0;

        // return buffer
        memMgr->deallocate(buf);
        return static_cast<uint64_t>(std::llround(sample));
    }

private:
    std::mt19937_64              rng_;
    std::normal_distribution<>   dist_;
};
