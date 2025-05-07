px4_add_module(
    MODULE        rtos_framework
    MAIN          rtos_framework_main
    STACK_MAIN    2000

    SRCS
        rtos_framework_main.cpp
        memory_manager.cpp
        scheduler_default.cpp
        scheduler_rt.cpp

    HEADERS
        memory_manager.h
        scheduler.h

    DEPENDS
        px4_platform
        px4_runtime
)
