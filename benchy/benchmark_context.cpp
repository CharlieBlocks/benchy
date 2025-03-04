#include "benchmark_context.hpp"


std::string BenchmarkStateToString(BenchmarkState state) {
    switch (state) {
        case BenchmarkState::Warmup: return "Warmup";
        case BenchmarkState::Running: return "Running";
        case BenchmarkState::Done: return "Done";
        case BenchmarkState::Fail: return "Fail";
    }
}


/* BenchmarkContext Implementation */
BenchmarkContext::BenchmarkContext(unsigned int iterations, unsigned int warmupIterations)
: m_warmupTarget(warmupIterations), 
  m_warmupNum(0), 
  m_iterTarget(iterations), 
  m_iterNum(0), 
  m_currentState(BenchmarkState::Warmup)
{ }

BenchmarkContext::BenchmarkContext(const BenchmarkContext &other)
: m_warmupTarget(other.m_warmupTarget),
    m_warmupNum(other.m_warmupNum),
    m_iterTarget(other.m_iterTarget),
    m_iterNum(other.m_iterNum),
    m_currentState(other.m_currentState),
    m_cpuTimer(other.m_cpuTimer),
    m_realTimer(other.m_realTimer)
{ }
BenchmarkContext::BenchmarkContext(const BenchmarkContext &&other)
: m_warmupTarget(other.m_warmupTarget),
    m_warmupNum(other.m_warmupNum),
    m_iterTarget(other.m_iterTarget),
    m_iterNum(other.m_iterNum),
    m_currentState(other.m_currentState),
    m_cpuTimer(other.m_cpuTimer),
    m_realTimer(other.m_realTimer)
{ }

BenchmarkContext &BenchmarkContext::operator =(const BenchmarkContext &other) {
    m_warmupTarget = other.m_warmupTarget;
    m_warmupNum = other.m_warmupNum;
    m_iterTarget = other.m_iterTarget;
    m_iterNum = other.m_iterNum;
    m_currentState = other.m_currentState;
    m_cpuTimer = other.m_cpuTimer;
    m_realTimer = other.m_realTimer;

    return *this;
}
BenchmarkContext &BenchmarkContext::operator =(const BenchmarkContext &&other) {
    *this = other; // Invoke copy constructor
    return *this;
}


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