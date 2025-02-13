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
            // std::this_thread::sleep_for(std::chrono::microseconds(500));
            fibbonachi(10);
        }

    }

    static std::unique_ptr<Benchmark> create_benchmark() {
        return std::make_unique<TestBenchmark>();
    }

private:
    static bool s_registered;
};

bool TestBenchmark::s_registered = Benchmark::register_benchmark("test_benchmark", TestBenchmark::create_benchmark);

MICROBENCHMARK(Fibbonachi, {
    while (context.next()) {
        fibbonachi(10);
    }
})


void show_output(std::string name, BenchmarkResults results) {
    std::cout << name << " Stats:\n";
    std::cout << "\tRuns: " << results.runCount << std::endl;
    std::cout << "\tIterations: " << results.iterationCount << std::endl;
    std::cout << "\tMedian: " << results.cpuData.median() << std::endl;
    std::cout << "\tRange: " << results.cpuData.range() << std::endl;
    std::cout << "\tMean: " << results.cpuData.mean() << std::endl;
    std::cout << "\tVariance: " << results.cpuData.variance() << std::endl;
    std::cout << "\tStd Deviation:" << results.cpuData.stdev() << std::endl;
    std::cout << "\n\t Average real time in Microseconds: " << time_span<units::nanoseconds, double>(results.realData.mean()).as<units::microseconds>().get() << "us" << std::endl;
    std::cout << "\n\t Average CPU time in Microseconds: " << time_span<units::nanoseconds, double>(results.cpuData.mean()).as<units::microseconds>().get() << "us" << std::endl;
    std::cout << std::endl;
}

int main(void) {
    Benchy inst;

    // TestBenchmark b = TestBenchmark();
    std::unique_ptr<Benchmark> b = Benchmark::get_benchmark("test_benchmark");
    BenchmarkResults results = b->execute();

    show_output("TestBenchmark", results);
    // std::cout << "Data Stats:\n";
    // std::cout << "\tRuns: " << results.runCount << std::endl;
    // std::cout << "\tIterations: " << results.iterationCount << std::endl;
    // std::cout << "\tMedian: " << results.cpuData.median() << std::endl;
    // std::cout << "\tRange: " << results.cpuData.range() << std::endl;
    // std::cout << "\tMean: " << results.cpuData.mean() << std::endl;
    // std::cout << "\tVariance: " << results.cpuData.variance() << std::endl;
    // std::cout << "\tStd Deviation:" << results.cpuData.stdev() << std::endl;
    // std::cout << "\n\t Average real time in Microseconds: " << time_span<units::nanoseconds, double>(results.realData.mean()).as<units::microseconds>().get() << "us" << std::endl;
    // std::cout << "\n\t Average CPU time in Microseconds: " << time_span<units::nanoseconds, double>(results.cpuData.mean()).as<units::microseconds>().get() << "us" << std::endl;

    b = Benchmark::get_benchmark("Fibbonachi");
    results = b->execute();

    show_output("Fibbonachi", results);

    return 0;
}