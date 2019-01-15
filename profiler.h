//
// Created by obyoxar on 15/01/19.
//

#ifndef POCEEXP_PROFILER_H
#define POCEEXP_PROFILER_H

#include <chrono>
#include <iostream>

#define GLOBAL_VAR __POCEEXP_PROFILER_H_GLOBAL_COUNT

extern int GLOBAL_VAR;

#define LOCAL_TIMER_INSTANCE __POCEEXP_PROFILER_H_LOCAL_INSTANCE

class TimerInstance{
public:
    TimerInstance(){
        GLOBAL_VAR ++;
    }
    ~TimerInstance(){
        GLOBAL_VAR --;
    }
    std::chrono::system_clock::time_point time = std::chrono::system_clock::now();
};

#define TIMER_INIT TimerInstance LOCAL_TIMER_INSTANCE;
#define TIMER_RESET LOCAL_TIMER_INSTANCE.time = std::chrono::system_clock::now();
#define TIMER_OUTPUT(name) {\
    long cnt = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now()- LOCAL_TIMER_INSTANCE.time).count(); \
    if(cnt != 0){ \
        for(int i = 0; i < GLOBAL_VAR; i++) std::cout << " "; \
        std::cout << " - [T] " << name << ": " << cnt << "ms" << std::endl; \
    }  \
    TIMER_RESET; \
}

#endif //POCEEXP_PROFILER_H
