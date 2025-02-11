#include "benchmark_context.hpp"

/* BenchmarkContext Implementation */
BenchmarkContext::BenchmarkContext(unsigned int iterations, unsigned int warmupIterations)
: m_warmupTarget(warmupIterations), 
  m_warmupNum(0), 
  m_iterTarget(iterations), 
  m_iterNum(0), 
  m_currentState(BenchmarkState::Warmup)
{ }

bool BenchmarkContext::next() {
    // Switch based on current state
    switch (m_currentState) {

        // Warmup state
        //  Here we are not timing the iterations
        //  So if we are < than m_warmupTarget then simply return true
        case BenchmarkState::Warmup: {
            if (m_warmupNum++ != m_warmupTarget)
                return true;

            // If we have reached the required warmup iterations
            // swap the state and start the timers
            m_currentState = BenchmarkState::Running;

            m_cpuTimer.start();
            m_realTimer.start();

            return true;
        }

        // Running state
        //  In this state the timers will be running so we simply check the iteration count
        case BenchmarkState::Running: {
            if (m_iterNum++ != m_iterTarget)
                return true;

            // We've reached the target iterations so we can stop the timers
            // as well as update the state
            m_cpuTimer.stop();
            m_realTimer.stop();

            m_currentState = BenchmarkState::Done;

            return false;
        };

        default: {
            return false;
        }
    }
}


time_span<units::nanoseconds> BenchmarkContext::get_cpu_time() {
    return m_cpuTimer.elapsedNanoseconds();
}

time_span<units::nanoseconds> BenchmarkContext::get_real_time() {
    return m_realTimer.elapsedNanoseconds();
}

BenchmarkState BenchmarkContext::state() {
    return m_currentState;
}