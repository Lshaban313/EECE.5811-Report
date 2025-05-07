#include "scheduler.h"
#include <random>

class RtScheduler : public Scheduler {
public:
    RtScheduler(MemoryManager* mgr)
        : Scheduler(mgr),
          rng_(654321),
          dist_(0.0, 1.0) {}

    uint64_t run(int cpu_load_pct) override {
        void* buf = memMgr->allocate();

        double mean   = BASE_LATENCY + cpu_load_pct * PER_PERCENT;
        double sigma  = BASE_JITTER * 0.70;  // 30% less jitter on RT :contentReference[oaicite:2]{index=2}
        std::normal_distribution<double> nd(mean, sigma);
        double sample = nd(rng_);
        if (sample < 0.0) sample = 0.0;

        memMgr->deallocate(buf);
        return static_cast<uint64_t>(std::llround(sample));
    }

private:
    std::mt19937_64            rng_;
    std::normal_distribution<> dist_;
};
