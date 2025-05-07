#include <px4_platform_common/log.h>
#include <px4_platform_common/px4_work_queue/ScheduledWorkItem.hpp>
#include <px4_time.h>
#include <px4_platform_common/tasks.h>

#include "memory_manager.h"
#include "scheduler.h"

// forward-declare both implementations:
extern class DefaultScheduler;
extern class RtScheduler;

/**
 * Entry point for `rtos_framework` PX4 module.
 * Usage: 
 *   rtos_framework default   → default scheduler
 *   rtos_framework rt        → PREEMPT_RT scheduler
 */
int rtos_framework_main(int argc, char *argv[])
{
    PX4_INFO("rtos_framework starting");

    // pick scheduler
    bool use_rt = false;
    if (argc > 1 && strcmp(argv[1], "rt") == 0) {
        use_rt = true;
    }

    MemoryManager memMgr(512, 256);
    Scheduler* sched = nullptr;

    if (use_rt) {
        PX4_INFO("Using PREEMPT_RT scheduler");
        sched = new RtScheduler(&memMgr);
    } else {
        PX4_INFO("Using default Linux scheduler");
        sched = new DefaultScheduler(&memMgr);
    }

    const char* mode = use_rt ? "rt" : "default";
    const std::vector<std::pair<const char*,int>> workloads = {
        {"light",    10},
        {"normal",   30},
        {"moderate", 50},
        {"high",     70},
        {"extreme",  90}
    };

    for (auto &w : workloads) {
        const char* wl_name = w.first;
        int pct = w.second;
        PX4_INFO("=== Workload: %s (%d%%) ===", wl_name, pct);

        // open CSV on SD card (or in tmpfs)
        char filename[64];
        snprintf(filename, sizeof(filename), "/fs/microsd/metrics_%s_%s.csv", mode, wl_name);
        FILE *fp = fopen(filename, "w");
        if (!fp) {
            PX4_ERR("Failed to open %s", filename);
            continue;
        }
        fprintf(fp, "cycle,latency_us\n");

        for (int cycle = 0; cycle < Scheduler::NUM_CYCLES; cycle++) {
            uint64_t start = hrt_absolute_time();
            uint64_t lat   = sched->run(pct);
            uint64_t end   = hrt_absolute_time();

            // write the simulated latency
            fprintf(fp, "%d,%llu\n", cycle, (unsigned long long)lat);
        }

        fclose(fp);
        PX4_INFO(" -> wrote %s", filename);
    }

    delete sched;
    PX4_INFO("rtos_framework done");
    return 0;
}
