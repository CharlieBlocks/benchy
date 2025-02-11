#ifndef _BENCHY_BENCHMARK_H
#define _BENCHY_BENCHMARK_H

#include "time.hpp"
#include "benchmark_context.hpp"
#include "series.hpp"

#include <chrono>
#include <map>

// TODO: Define in Benchy class
#define BENCHY_MIN_TIME time_span<units::milliseconds>(500)
#define BENCHY_MAX_TIME time_span<units::seconds>(1)
#define BENCHY_MAX_ITERATIONS 50'000'000
#define BENCHY_MIN_CHANGE 100

/*
A data structure that holds the results of the benchmark
*/
struct BenchmarkResults {
    BenchmarkResults() = default;
    BenchmarkResults(int reserveSize)
    : cpuData(reserveSize), realData(reserveSize) { }


    DataSeries<double> cpuData;
    DataSeries<double> realData;

    int iterationCount;
    int runCount;
};


class Benchmark {
public:
    /*
    Constructor for the Benchmark type.
    It takes the number of iterations to perform per run,
    the number of warmup iterations to perform before the actual benchmark runs,
    the total number of times to run the benchmark,
    and whether to adjust the number of iterations ran
    */
    Benchmark(unsigned int iterations, unsigned int warmupIterations, unsigned int runCount, bool adjustIterations);

    virtual ~Benchmark() = default;


    /*
    The actual benchmarking function to be run.
    This is expected to wrap the test code that you want to time
    in the context.
    */
    virtual void bench_fn(BenchmarkContext &context) = 0;


    /*
    Executes the benchmark, including the fit time for the requested number of 
    runs. The results are returned in a BenchmarkResults structure
    */
    BenchmarkResults execute();

public:
    using create_fn = std::unique_ptr<Benchmark>(*)();

    static bool register_benchmark(const std::string name, create_fn createFn);


    // TEMPORARY
    static std::unique_ptr<Benchmark> get_benchmark(const std::string name) {
        return registeredBenchmarks()[name]();
    }

private:
    /*
    Runs the benchmark for one run and checks what the returned time was,
    if the time is insignificant then the number of iterations will be increased
    until the time is expected to provide reliable results.
    */
    void check_time();


    /* 
    A helper function used by the check_time function. This runs the benchmark
    for a single instance and returns the resultant context
    */
    BenchmarkContext run_one();


    /*
    Returns the static variable containing the registered benchmarks. Unfortunatly we can not
    use a static member variable as we cannot guarantee that it will be initialised before the first
    call. Instead we use a function with a static variable inside it, this makes use of Construct On First Use
    to ensure the variable is created before an insertion is made.
    */
    static std::map<const std::string, create_fn> &registeredBenchmarks();

private:    
    unsigned int m_iterations;
    unsigned int m_warmupIterations;
    unsigned int m_runCount;
    bool m_adjustIterations;
};



template<typename T>
class BenchmarkRegisterHelper {
protected:
    static bool s_wasRegistered;
};

#endif // _BENCHY_BENCHMARK_H