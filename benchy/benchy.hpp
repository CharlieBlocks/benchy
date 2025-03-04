#include <vector>

#include "benchmark.hpp"
#include "benchmark_context.hpp"
#include "defines.hpp"
#include "dispatcher.hpp"
#include "exporter.hpp"


class Benchy {
public:
    // TODO: Accept cmd arguments
    Benchy(int maxThreadCount);


    // Executor Functions
    void ExecuteMicroBenchmarks();
    void ExportTo(std::unique_ptr<Exporter> &exporter);
    


    inline JobDispatcher *Pool() { return &_threadPool; }


    static inline Benchy *Instance() { return s_instance; }
private:
    JobDispatcher _threadPool;

    std::vector<BenchmarkResults> _benchmarkResults;

    static Benchy *s_instance;
};