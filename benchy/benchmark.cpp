#include "benchmark.hpp"
#include "dispatcher.hpp"

#include "benchy.hpp"

#include <memory>
#include <iostream>



std::vector<std::shared_ptr<__Benchmark<void>>> &MicroBenchmarks::get_benchmark_list() {
    static std::vector<std::shared_ptr<__Benchmark<void>>> _microBenchmarkList;
    return _microBenchmarkList;
}



// Initialise Benchmark::s_registeredBenchmarks
// Because this is a "zero-initialisation" variable it will be
// initialised before any other classes attempt to register
// std::map<const std::string, Benchmark::create_fn> Benchmark::s_registeredBenchmarks;

/*
An implementation of a Job to run the benchmark
It holds a BenchmarkContext which is used to run the benchmark and is then
indirectly accessed using data_lock();
*/
// class BenchmarkJob : public Job {
// public:
//     BenchmarkJob(Benchmark *instance, int iterations, int warmupIterations) 
//     : m_benchmarkInstance(instance), m_context(iterations, warmupIterations)
//     { }

//     time_span<units::nanoseconds> get_cpu_nanoseconds() {
//         // Get a lock on the class data
//         std::unique_lock<std::mutex> lock = data_lock();
//         return m_context.get_cpu_time();
//     }

//     time_span<units::nanoseconds> get_real_nanoseconds() {
//         std::unique_lock<std::mutex> lock = data_lock();
//         return m_context.get_real_time();
//     }


//     void proc() override final {
//         // Lock internal data while we are using it;
//         std::unique_lock<std::mutex> lock = this->data_lock();

//         // Sanity check
//         // May happen if race conditions cause the job to be picked up by two threads
//         if (m_context.state() != BenchmarkState::Warmup)
//             std::cout << "Warning: Running benchmark job more than once. Check your pointers!\n" << std::flush;

//         // Actually execute the benchmark function
//         m_benchmarkInstance->bench_fn(m_context);

//         // Check to ensure the context was run
//         // If the benchmark has not been completed then the benchmark is
//         // likely malformed
//         if (m_context.state() != BenchmarkState::Done)
//             std::cout << "Warning: Benchmark function concluded without completing all iterations. Perhaps the function is malformed?\n" << std::flush;
//     }

// private:
//     Benchmark       *m_benchmarkInstance;
//     BenchmarkContext m_context;
// };



// /* Benchmark Implementation */
// Benchmark::Benchmark(
//     unsigned int iterations, 
//     unsigned int warmupIterations, 
//     unsigned int runCount, 
//     bool adjustIterations
// ) 
// : m_iterations(iterations), 
//   m_warmupIterations(warmupIterations), 
//   m_runCount(runCount), 
//   m_adjustIterations(adjustIterations) 
// { }


// BenchmarkResults Benchmark::execute() {
//     if (m_adjustIterations)
//         this->check_time();

//     // Create a list of jobs to run
//     std::vector<std::shared_ptr<BenchmarkJob>> jobs;
//     jobs.reserve(m_runCount);
//     for (int i = 0; i < m_runCount; i++)
//         jobs.push_back(std::make_shared<BenchmarkJob>(this, this->m_iterations, this->m_warmupIterations));

//     // Submit jobs to be ran
//     for (int i = 0; i < m_runCount; i++)
//         Benchy::instance()->pool()->queue_job(std::dynamic_pointer_cast<Job>(jobs[i]));
//         // continue;
    
//     // Wait for all jobs to complete
//     for (int i = 0; i < m_runCount; i++)
//         jobs[i]->wait();

    
//     // Create the result and collect all the data into the data series
//     BenchmarkResults results(m_runCount);
//     results.iterationCount = this->m_iterations;
//     results.runCount = this->m_runCount;
//     for (auto job : jobs) {
//         results.cpuData.add_point((double)job->get_cpu_nanoseconds().get() / m_iterations);
//         results.realData.add_point((double)job->get_real_nanoseconds().get() / m_iterations);
//     }

//     return results;
// }


// void Benchmark::check_time() {

//     while (true) {

//         // Run one iteration
//         BenchmarkContext result = this->run_one();

//         if (result.get_cpu_time() > BENCHY_MIN_TIME ||
//             result.get_real_time() > BENCHY_MAX_TIME)
//             break;

        
//         // Calculate average time per iter
//         double timePerIter = (double)result.get_cpu_time().get() / m_iterations;

//         // Calculate required time
//         unsigned long requiredIter = BENCHY_MIN_TIME.as<units::nanoseconds>().get() / timePerIter;

//         if (requiredIter - m_iterations < BENCHY_MIN_CHANGE) {
//             m_iterations = requiredIter;
//             break;
//         }


//         if (requiredIter > BENCHY_MAX_ITERATIONS) {
//             m_iterations = BENCHY_MAX_ITERATIONS;
//             break;
//         }

//         m_iterations = requiredIter;
//     }

// }


// BenchmarkContext Benchmark::run_one() {

//     BenchmarkContext context(m_iterations, m_warmupIterations);

//     try {
//         this->bench_fn(context);
//     } catch (...) {
//         std::cout << "Exception occured in benchmark fn" << std::endl;
//         // TODO: Handle exception gracefully 
//         exit(1);
//     }

//     if (context.state() != BenchmarkState::Done)
//         std::cout << "WARNING: Benchmark function concluded without completing all iterations. Perhaps the function is malformed?" << std::endl;

//     return context;
// }

// std::map<const std::string, Benchmark::create_fn> &Benchmark::registeredBenchmarks() {
//     static std::map<const std::string, create_fn> map_;
//     return map_;
// }

// bool Benchmark::register_benchmark(const std::string name, create_fn createFn) {
//     if (auto it = registeredBenchmarks().find(name); it == registeredBenchmarks().end()) {
//         registeredBenchmarks().insert({ name, createFn });
//         return true;
//     }

//     return false;
// }