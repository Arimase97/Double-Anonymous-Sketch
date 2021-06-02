#ifndef UTIL_H
#define UTIL_H

#include <chrono>

#define INF 1000000000
#define REP_TIME 1
#define TOTAL_FLOW 20000000
//#define DEBUG_MODE

typedef std::chrono::high_resolution_clock::time_point TP;

inline TP now(){
    return std::chrono::high_resolution_clock::now();
}

#endif