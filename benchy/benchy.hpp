#include <vector>

#include "benchmark.hpp"
#include "benchmark_context.hpp"
#include "defines.hpp"
#include "dispatcher.hpp"


class Benchy {
public:
    // TODO: Accept cmd arguments
    Benchy(int maxThreadCount);


    // Executor Functions
    void ExecuteMicroBenchmarks();
    


    inline JobDispatcher *Pool() { return &_threadPool; }


    static inline Benchy *Instance() { return s_instance; }
private:
    JobDispatcher _threadPool;

    std::vector<BenchmarkResults> _benchmarkResults;

    static Benchy *s_instance;
};