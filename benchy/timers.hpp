#ifndef _BENCHY_TIMERS_H
#define _BENCHY_TIMERS_H

#include <chrono>
#include <time.h>

#include <iostream>



class RealtimeTimer {
public:

    inline void start() { m_startPoint  = std::chrono::high_resolution_clock::now(); };
    inline void stop()  { m_endPoint    = std::chrono::high_resolution_clock::now(); };

    inline unsigned long elapsedNanoseconds()  { return std::chrono::duration_cast<std::chrono::nanoseconds>(m_endPoint - m_startPoint).count(); }
    inline unsigned long elapsedMicroseconds() { return std::chrono::duration_cast<std::chrono::microseconds>(m_endPoint - m_startPoint).count(); }
    inline unsigned long elapsedMilliseconds() { return std::chrono::duration_cast<std::chrono::milliseconds>(m_endPoint - m_startPoint).count(); }
    inline unsigned long elapsedSeconds()      { return std::chrono::duration_cast<std::chrono::seconds>(m_endPoint - m_startPoint).count(); }


private:
    std::chrono::high_resolution_clock::time_point m_startPoint;
    std::chrono::high_resolution_clock::time_point m_endPoint;

};


class CpuTimer {
public:

    inline void start() { clock_gettime(CLOCK_THREAD_CPUTIME_ID, &m_startPoint); }
    inline void stop()  { clock_gettime(CLOCK_THREAD_CPUTIME_ID, &m_endPoint); }

    inline unsigned long elapsedNanoseconds() { return (m_endPoint.tv_sec - m_startPoint.tv_sec) * 1.0e+9 + (m_endPoint.tv_nsec - m_startPoint.tv_nsec); }
    
    // TODO: Remove
    int getDigitCount() {
        auto ns = this->elapsedNanoseconds();

        if (ns < 1e+5) { // < 5 digits

            if (ns < 1e+3) { // < 3 digits

                if (ns < 1e+2) // < 2 digits
                    return 1;
                return 2; // == 2 digits

            }
            else { // >= 3 digits

                if (ns < 1e+4)
                    return 3; // == 3 digits
                return 4; // == 4 digits

            }

        }
        else { // >= 5 digits

            if (ns < 1e+7) { // < 7 digits

                if (ns < 1e+6) // < 6 digits
                    return 5; // == 5 digits
                return 6; // == 6 digits

            }
            else { // >= 7 digits

                if (ns < 1e+8) // < 8 digits
                    return 7; // == 7 digits
                if (ns < 1e+9) // < 9 digits
                    return 8; // == 8 digits
                return 9; // >= 9 digits
            }

        }
        
        return 0;
    }
private:
    timespec m_startPoint;
    timespec m_endPoint;
};

#endif // _BENCHY_TIMERS_H