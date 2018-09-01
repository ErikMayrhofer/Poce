//
// Created by obyoxar on 01.09.18.
//

#ifndef DLIBTEST_MONITORING_H
#define DLIBTEST_MONITORING_H

#define MONITOR_VAR_NAME(var) __monitor__##var

#define START_MONITOR(var) auto MONITOR_VAR_NAME(var) = std::chrono::system_clock::now();
#define RESET_MONITOR(var) MONITOR_VAR_NAME(var) = std::chrono::system_clock::now();

#define MONITOR_ns(msg, var) \
            {auto diff = std::chrono::system_clock::now() - MONITOR_VAR_NAME(var); \
            std::cout << (msg) << ": " << diff.count() << "ns" << std::endl;} \
            MONITOR_VAR_NAME(var) = std::chrono::system_clock::now();
#define MONITOR_mis(msg, var) \
            {auto diff = std::chrono::system_clock::now() - MONITOR_VAR_NAME(var); \
            std::cout << (msg) << ": " << diff.count()/1000 << "mis" << std::endl;} \
            MONITOR_VAR_NAME(var) = std::chrono::system_clock::now();
#define MONITOR_ms(msg, var) \
            {auto diff = std::chrono::system_clock::now() - MONITOR_VAR_NAME(var); \
            std::cout << (msg) << ": " << diff.count()/1000000 << "ms" << std::endl;} \
            MONITOR_VAR_NAME(var) = std::chrono::system_clock::now();
#define MONITOR_(msg, var) MONITOR_mis(msg, var)
#define MONITOR(msg, var, ...) MONITOR_##__VA_ARGS__(msg, var)

#endif //DLIBTEST_MONITORING_H
