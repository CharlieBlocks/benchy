#include "benchy.hpp"
#include "series.hpp"

#include <iostream>
#include <functional>

Benchy *Benchy::s_instance = nullptr;

/* 
BenchmarkContext Implemenation
*/
// BenchmarkContext::BenchmarkContext(int iterations) 
// : warmupMax(0), warmupNum(0), iterMax(iterations), iterNum(0), currState(BenchmarkState::Warmup)
// { }

// BenchmarkContext::BenchmarkContext(int iterations, int warmupIterations)
// : warmupMax(warmupIterations), warmupNum(0), iterMax(iterations), iterNum(0), currState(BenchmarkState::Warmup)
// { }

// bool BenchmarkContext::next() {
//     switch (currState) {
//         case BenchmarkState::Warmup: {
//             if (warmupNum++ != warmupMax)
//                 return true;
//             currState = BenchmarkState::Running;

//             cpuTime.start();
//             realTime.start();

//             return true;
//         }
//         case BenchmarkState::Running: {
//             if (iterNum++ != iterMax)
//                 return true;
            
            
//             cpuTime.stop();
//             realTime.stop();

//             currState = BenchmarkState::Done;

//             return false;
//         };
//         default: {
//             return false;
//         }
//     }
// }


// /* BenchmarkJob Implemenation */
// BenchmarkJob::BenchmarkJob(Benchmark *instance, int iterations, int warmupIterations)
// : m_benchmarkInstance(instance), m_context(iterations, warmupIterations), m_result({}) 
// { }

// void BenchmarkJob::proc() {
//     std::cout << "INFO: Starting Job\n" << std::flush;

//     // Run function
//     m_benchmarkInstance->BenchFn(m_context);

//     if (m_context.state() != BenchmarkState::Done) {
//         std::cout << "WARNING: Benchmark function concluded without completing all iterations. Perhaps the function is malformed?" << std::endl;
//     }
//     std::cout << "INFO: Job Completed\n" << std::flush;
//     std::cout << m_context.get_cpu_time() << std::endl;

//     m_result.cpuNs  = m_context.get_cpu_time();
//     m_result.realNs = m_context.get_real_time();
// }


// /*
// BenchmarkResult Implemenation
// */
// BenchmarkResult::BenchmarkResult()
// : cpuNs(0), realNs(0)
// { }
// BenchmarkResult::BenchmarkResult(unsigned long cpu, unsigned long real)
// : cpuNs(cpu), realNs(real)
// { }


// /* 
// Benchmark Implementation
// */
// Benchmark::Benchmark()
// : iterationCount(1), warmupCount(0), runCount(1)
// { }

// void Benchmark::Execute() {
//     this->FitTime();

//     this->RunAll();
// }

// void Benchmark::FitTime() {

//     while (true) {

//         // Run for specified number of iterations
//         BenchmarkResult result = this->RunOne();

//         // Check if time is significant or not
//         // Or if the real time taken is too long
//         // If it is, exit the function
//         if ( result.cpuNs  > BENCHY_MIN_TIME_MS*1e+6 || 
//              result.realNs > BENCHY_MAX_TIME_MS*1e+6)
//             break;
        
//         // Calculate avg time per iter
//         double timePerIter = (double)result.cpuNs / iterationCount;


//         // Calculate required iteraations
//         // Magic number time!
//         int requiredIter = (BENCHY_MIN_TIME_MS*1e+6) * 1.4f / timePerIter;

//         if (requiredIter > BENCHY_MAX_ITERATIONS) {
//             iterationCount = BENCHY_MAX_ITERATIONS;
//             break;
//         }

//         iterationCount = requiredIter;
//     }
// }


// void Benchmark::RunAll() {

//     std::unique_ptr<std::shared_ptr<BenchmarkJob>[]> jobs = std::unique_ptr<std::shared_ptr<BenchmarkJob>[]>(new std::shared_ptr<BenchmarkJob>[runCount]);
//     for (int i = 0; i < runCount; i++)
//         jobs[i] = std::make_shared<BenchmarkJob>(this, this->iterationCount, this->runCount);

//     for (int i = 0; i < runCount; i++) {
//         Benchy::instance()->pool()->queue_job(std::dynamic_pointer_cast<Job>(jobs[i]));
//     }

//     for (int i = 0; i < runCount; i++)
//         jobs[i]->wait();

//     std::cout << "Finished running benchmarks" << std::endl;

//     DataSeries<double> cpuData(5);
//     for (int i = 0; i < runCount; i++) {
//         BenchmarkResult result = jobs[i]->result();
//         cpuData.add_point(result.cpu_seconds() * 1e+3);
//         std::cout << "Run: " << i << ". Cpu: " << result.cpu_seconds()*1e3 << " milliseconds. Real: " << result.real_seconds() << "seconds" << std::endl;
//     }

//     std::cout << "Data Stats:\n";
//     std::cout << "\tSamples: " << 5 << std::endl;
//     std::cout << "\tMedian: " << cpuData.median() << std::endl;
//     std::cout << "\tRange: " << cpuData.range() << std::endl;
//     std::cout << "\tMean: " << cpuData.mean() << std::endl;
//     std::cout << "\tVariance: " << cpuData.variance() << std::endl;
//     std::cout << "\tStd Deviation:" << cpuData.stdev() << std::endl;
// }


// BenchmarkResult Benchmark::RunOne() {

//     // Create new context
//     BenchmarkContext context(iterationCount, warmupCount);

//     // TODO: Place in try loop
//     BenchFn(context);

//     // Check that benchmark completed properly
//     // If the state is not set correctly then the BenchFn is probably not
//     // formed correctly
//     if (context.state() != BenchmarkState::Done) {
//         std::cout << "WARNING: Benchmark function concluded without completing all iterations. Perhaps the function is malformed?" << std::endl;
//     }

//     // Forumlate and return reuslt
//     return BenchmarkResult(
//         context.get_cpu_time(),
//         context.get_real_time()
//     );
// }


/* ThreadQueue Implementation */
// ThreadQueue::ThreadQueue(int poolSize)
// : m_poolSize(poolSize), m_isRunning(true), m_threadPool(new std::thread[poolSize])
// { 
//     for (int i = 0; i < poolSize; i++) {
//         m_threadPool[i] = std::thread(std::bind(&ThreadQueue::thread_proc, this));
//     }
// }

// ThreadQueue::~ThreadQueue() {
//     m_isRunning.store(false);
//     m_cv.notify_all();

//     for (int i = 0; i < m_poolSize; i++)
//         m_threadPool[i].join();

//     delete[] m_threadPool;
// }

// void ThreadQueue::queue_job(BenchmarkJob *job) {
//     std::unique_lock<std::mutex> lock(m_queueLock);
//     m_jobQueue.push(job);
//     lock.unlock();

//     m_cv.notify_one();
// };

// void ThreadQueue::thread_proc() {

//     while (m_isRunning.load()) {
//         std::unique_lock<std::mutex> lock(m_queueLock);
//         m_cv.wait(lock, [&]() { return m_jobQueue.size() > 0 || !m_isRunning.load(); });

//         if (!m_isRunning) {
//             lock.unlock();
//             break;
//         }

//         BenchmarkJob *job = m_jobQueue.front();
//         m_jobQueue.pop();

//         lock.unlock();


//         job->execute();
//     }

// }



/* Benchy Implementation */
Benchy::Benchy() 
: m_threadPool(4) { 
    s_instance = this;
}