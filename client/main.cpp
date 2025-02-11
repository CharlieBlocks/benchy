#include "benchy/benchy.hpp"
#include "benchy/time.hpp"

#include <iostream>


int fibbonachi(int n) {
    if (n < 3)
        return 1;
    return fibbonachi(n-1) + fibbonachi(n-2);
}


class TestBenchmark : 
    public Benchmark,
    public BenchmarkRegisterHelper<TestBenchmark> 
{
public:
    TestBenchmark() 
    : Benchmark(1, 0, 10, true)
    {

    }

    void bench_fn(BenchmarkContext &context) override {

        while (context.next()) {
            fibbonachi(20);
        }

    }

    static std::unique_ptr<Benchmark> create_benchmark() {
        return std::make_unique<TestBenchmark>();
    }

private:
    static bool s_registered;
};

bool TestBenchmark::s_registered = Benchmark::register_benchmark("test_benchmark", TestBenchmark::create_benchmark);

// BENCHMARK(Fibbonachi, {
//     while (context.next()) {
//         fibbonachi(20);
//     }
// })


int main(void) {
    Benchy inst;

    // TestBenchmark b = TestBenchmark();
    std::unique_ptr<Benchmark> b = Benchmark::get_benchmark("test_benchmark");
    BenchmarkResults results = b->execute();

    std::cout << "Data Stats:\n";
    std::cout << "\tRuns: " << results.runCount << std::endl;
    std::cout << "\tIterations: " << results.iterationCount << std::endl;
    std::cout << "\tMedian: " << results.cpuData.median() << std::endl;
    std::cout << "\tRange: " << results.cpuData.range() << std::endl;
    std::cout << "\tMean: " << results.cpuData.mean() << std::endl;
    std::cout << "\tVariance: " << results.cpuData.variance() << std::endl;
    std::cout << "\tStd Deviation:" << results.cpuData.stdev() << std::endl;


    time_span<units::seconds, int> sec(5);
    time_span<units::milliseconds, int> mil(500);

    std::cout << "Seconds: " << sec.get() << std::endl;
    std::cout << "Milliseconds: " << mil.get() << std::endl;
    std::cout << "Seconds as Microseconds: " << sec.as<units::microseconds>().get() << std::endl;

    return 0;
}