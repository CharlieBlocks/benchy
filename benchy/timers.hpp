#ifndef _BENCHY_TIMERS_H
#define _BENCHY_TIMERS_H

#include <chrono>
#include <time.h>

#include "time.hpp"



class RealtimeTimer {
public:

    inline void start() { m_startPoint  = std::chrono::high_resolution_clock::now(); };
    inline void stop()  { m_endPoint    = std::chrono::high_resolution_clock::now(); };

    inline time_span<units::nanoseconds> elapsedNanoseconds()  { return time_span<units::nanoseconds>(std::chrono::duration_cast<std::chrono::nanoseconds>(m_endPoint - m_startPoint).count()); }

private:
    std::chrono::high_resolution_clock::time_point m_startPoint;
    std::chrono::high_resolution_clock::time_point m_endPoint;

};


class CpuTimer {
public:

    inline void start() { clock_gettime(CLOCK_THREAD_CPUTIME_ID, &m_startPoint); }
    inline void stop()  { clock_gettime(CLOCK_THREAD_CPUTIME_ID, &m_endPoint); }

    inline time_span<units::nanoseconds> elapsedNanoseconds() { return time_span<units::nanoseconds>((m_endPoint.tv_sec - m_startPoint.tv_sec) * 1.0e+9 + (m_endPoint.tv_nsec - m_startPoint.tv_nsec)); }
    
private:
    timespec m_startPoint;
    timespec m_endPoint;
};

#endif // _BENCHY_TIMERS_H