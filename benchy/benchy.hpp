#include <memory>
#include <thread>
#include <mutex>
#include <atomic>

#include "timers.hpp"
#include "dispatcher.hpp"

#include "benchmark.hpp"

/* Defines */
// #define BENCHY_MAX_ITERATIONS 50'000'000
// #define BENCHY_MIN_TIME_MS 500 // Minimum of 500 milliseconds
// #define BENCHY_MAX_TIME_MS 2000 // Maximum time of 1000 milliseconds. If the benchmark exceeds this Benchy will not attempt to perform any more iterations



// /* Enums */
// enum class BenchmarkState {
//     Idle,
//     Warmup,
//     Running,
//     Done
// };

// enum class JobState {
//     Waiting,
//     Running,
//     Done
// };


// /* Classes */

// class BenchmarkContext {
// public:
//     BenchmarkContext(int iterations);
//     BenchmarkContext(int iterations, int warmupIterations);

//     /*
//     Steps the context to the next iteration
//     */
//     bool next();

//     unsigned long get_cpu_time()    { return cpuTime.elapsedNanoseconds(); }
//     unsigned long get_real_time()   { return realTime.elapsedNanoseconds(); }

//     BenchmarkState state() { return currState; }

// private:
//     // Number of warmup iterations and the current count of warmup iterations
//     int warmupNum;
//     int warmupMax;

//     // The current and max iteration count
//     int iterNum;
//     int iterMax;

//     BenchmarkState currState;

//     CpuTimer cpuTime;
//     RealtimeTimer realTime;
// };


// struct BenchmarkResult {

//     BenchmarkResult();
//     BenchmarkResult(unsigned long cpu, unsigned long real);


//     double cpu_seconds() { return ((double)cpuNs) * 1e-9; }
//     double real_seconds() { return ((double)realNs) * 1e-9; }


//     unsigned long cpuNs;
//     unsigned long realNs;

// };



// class Benchmark {
// public:
//     Benchmark();

//     // Setup methods
//     inline void iterations(int iter) { this->iterationCount = iter; }
//     inline int iterations() { return this->iterationCount; }

//     inline void warmup_iterations(int iter) { this->warmupCount = iter; }
//     inline int warmup_iterations() { return this->warmupCount; }

//     inline void run_count(int count) { this->runCount = count; }
//     inline int run_count() { return this->runCount; }

//     virtual void BenchFn(BenchmarkContext &context) = 0;

//     void Execute();

// private:
//     /*
//     Runs a single run of the benchmark and checks if the time is significant
//     enough to be measured, if not then the number of iterations is increased
//     until a maximum is hit or a suitible value is hit
//     */
//     void FitTime();

//     /* 
//     Runs the benchmark for the requested number of runs
//     */
//     void RunAll();

//     /*
//     Runs the benchmark once and returns the results
//     */
//     BenchmarkResult RunOne();

// private:
//     int warmupCount;        // Number of warmup iterations
//     int iterationCount;     // Number of real iterations
//     int runCount;           // Number of runs to execute

// };


// class BenchmarkJob : public Job {
// public:
//     BenchmarkJob(Benchmark *instance, int iterations, int warmupIterations);


//     BenchmarkResult result() { 
//         std::unique_lock<std::mutex> lock = data_lock();
//         return m_result; 
//     }

//     void proc() override final;

// private:
//     Benchmark              *m_benchmarkInstance;

//     BenchmarkContext        m_context;
//     BenchmarkResult         m_result;
// };



// class ThreadQueue {
// public:
//     ThreadQueue(int poolSize);
//     ~ThreadQueue();

//     void queue_job(BenchmarkJob *job);

// private:
//     void thread_proc();

// private:
//     int m_poolSize;
//     std::thread *m_threadPool;
//     std::atomic<bool> m_isRunning;

//     std::queue<BenchmarkJob *> m_jobQueue;
//     std::condition_variable m_cv;
//     std::mutex m_queueLock;
// };



class Benchy {
public:
    Benchy();

    inline JobDispatcher *pool() { return &m_threadPool; }

    static inline Benchy *instance() { return s_instance; }
private:
    // List of benchmarks
    // Thread queue

    JobDispatcher m_threadPool;
    // Exporter

    static Benchy *s_instance;
};



/* Defines */
#define BENCHMARK(name, content) \
class Bench_##name : public Benchmark { \
public: \
    void BenchFn(BenchmarkContext &context) override {\
        content \
    }\
};