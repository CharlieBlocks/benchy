#ifndef _BENCHY_BENCHMARK_CONTEXT_H
#define _BENCHY_BENCHMARK_CONTEXT_H

#include "time.hpp"
#include "timers.hpp"


/*
An enum to represent the state of the benchmark.
Warmup - Running warmup iterations
Running - Running the benchmark iterations
Done - Finished running all iterations
*/
enum class BenchmarkState : unsigned char {
    Warmup,
    Running,
    Done,
    Fail
};

std::string BenchmarkStateToString(BenchmarkState state);


/*
The Benchmark Context is passed to the benchmarking function. It provides
an iterative interface for running and timing code.
*/
class BenchmarkContext {
public:
    /*
    A constructor that accepts the number of times to run the code
    and a number of additional runs to perform before timing the code.

    Warmup iterations help ensure that the instruction cache is hot
    */
    BenchmarkContext(unsigned int iterations, unsigned int warmupIterations);


    /*
    Copy and Move Constructors. They will copy the targets, states and timers
    */
    BenchmarkContext(const BenchmarkContext &other);
    BenchmarkContext(const BenchmarkContext &&other);


    /* 
    Copy and Move assignment operators. Do the same thing
    */
    BenchmarkContext &operator =(const BenchmarkContext &other); 
    BenchmarkContext &operator =(const BenchmarkContext &&other);


    /*
    Steps to the next iteration. This should be used in a while loop.

    while (context.next()) { ... }
    */
    bool next();


    /*
    Returns the timed duration in nanoseconds. This is calculated using the
    per-thread CPU timer which counts the number of clock cycles and then divides them
    by the frequency of the CPU. 
    */
    time_span<units::nanoseconds> get_cpu_time();

    /*
    Returns the real time duration in nanoseconds.
    */
    time_span<units::nanoseconds> get_real_time();


    /*
    Returns the current state of the benchmark
    */
    BenchmarkState state();


private:
    // The current number of warmup iterations and the target number of warmup iterations
    unsigned int m_warmupNum;
    unsigned int m_warmupTarget;

    // The current number of iterations and the current number of iterations
    unsigned int m_iterNum;
    unsigned int m_iterTarget;

    // The current state of the benchmark
    // This is updated by the next function
    BenchmarkState m_currentState;

    // Timer objects for CPU and world time
    CpuTimer m_cpuTimer;
    RealtimeTimer m_realTimer;
};


#endif // _BENCHY_BENCHMARK_CONTEXT_H