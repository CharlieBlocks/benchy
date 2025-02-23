#include "benchy.hpp"
#include "series.hpp"
#include "time_fitter.hpp"

#include <iostream>
#include <memory>

Benchy *Benchy::s_instance = nullptr;


class BenchmarkExecutorJob : public Job {
public:
    BenchmarkExecutorJob(std::shared_ptr<__Benchmark<void>> benchmark, int varIdx) 
    : _varIdx(varIdx), _benchmark(benchmark), _context(0, 0)
    { }

    BenchmarkContext &&get_context() {
        std::unique_lock<std::mutex> lock = data_lock();
        return std::move(_context);
    }

    void proc() override final {
        std::unique_lock<std::mutex> lock = this->data_lock();

        _context = _benchmark->Execute(nullptr, _varIdx);

        // Check to ensure the context was run
        // If the benchmark has not been completed then the benchmark is
        // likely malformed
        if (_context.state() != BenchmarkState::Done)
            std::cout << "Warning: Benchmark function concluded without completing all iterations. Perhaps the function is malformed?\n" << std::flush;
    }

private:
    int _varIdx;
    std::shared_ptr<__Benchmark<void>> _benchmark;
    BenchmarkContext _context;
};


/* Benchy Implementation */
Benchy::Benchy(int maxThreadCount) 
: _threadPool(maxThreadCount) { 
    s_instance = this;
}


void Benchy::ExecuteMicroBenchmarks() {

    // 1. Loop over Microbenchmarks
    // 2. Fit time if required
    // 3. Execute multithreaded
    // 4. Collect and save results into a BenchmarkResults struct

    for (auto b : MicroBenchmarks::get_benchmark_list()) {
        // Loop over variations        
        for (int v = 0; v < b->GetVariationCount(); ++v) {
            TimeFitter<void> fitter(b, nullptr, v);
            fitter.Fit();

            // Then run jobs
            std::vector<std::shared_ptr<BenchmarkExecutorJob>> jobs;
            jobs.reserve(b->_runCount);

            // Create Jobs
            for (int i = 0; i < b->_runCount; ++i)
                jobs.push_back(std::make_shared<BenchmarkExecutorJob>(b, v));

            // Run Jobs
            for (int i = 0; i < b->_runCount; ++i)
                Pool()->queue_job(std::dynamic_pointer_cast<Job>(jobs[i]));

            // Wait for completion
            for (int i = 0; i < b->_runCount; ++i)
                jobs[i]->wait();
            
            // Create and push back result
            BenchmarkResults result(b->_runCount);
            result.iterationCount = b->_iterationCount;
            result.runCount       = b->_runCount;
            for (auto job : jobs) {
                BenchmarkContext context = job->get_context();
                result.cpuData.add_point((double)context.get_cpu_time().get() / b->_iterationCount);
                result.realData.add_point((double)context.get_real_time().get() / b->_iterationCount);
            }

            _benchmarkResults.push_back(std::move(result));
        }

    }


    // 1. Loop over ComponentBenchmarks
    // 2. Loop over individual benchmarks
    // 3. Fit time and Execute
    // 4. Save results to a ComponentResults

}