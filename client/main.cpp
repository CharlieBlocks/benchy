#include "benchy/benchy.hpp"
#include "benchy/time.hpp"

#include <iostream>
#include <tuple>



// class TestBenchmark : public ComponentBenchmark {
// public:
//     TestBenchmark() {
//         RegisterBenchmark(
//             "test_bench", 
//             &TestBenchmark::CustomBenchmark
//         )
//         ->Iterations(100)
//         ->Params<3>(ArgumentMatrix<3, int, int, int>(
//             { 0, 1, 2 }, 
//             { 3, 4, 5 },
//             { 6, 7, 8 }
//         ));

//         RegisterBenchmark(
//             "bench_2",
//             &TestBenchmark::EmptyBenchmark
//         )->Iterations(10);

//         ArgumentMatrix<2, int, int> matrix(
//             { 1, 2 },
//             { 3, 4}
//         );
//     }


//     void CustomBenchmark(int a, int b, int c) {
//         std::cout << "Args: " << a << ", " << b << ", " << c << std::endl;
//     }

//     void EmptyBenchmark() {
//         std::cout << "Empty!" << std::endl;
//     }

// };

int fibbonachi(int n) {
    if (n < 3)
        return 1;
    return fibbonachi(n-1) + fibbonachi(n-2);
}

void test_func(BenchmarkContext &context) {

    while (context.next()) {
        fibbonachi(10);
    }

}

MICROBENCHMARK(test_func)
->Warmups(50)
->Iterations(10'000'000)
->RunCount(5);


int main(void) {
    Benchy benchy(4);

    benchy.ExecuteMicroBenchmarks();

    std::unique_ptr<Exporter> exporter = std::make_unique<StdoutTableExporter>();
    benchy.ExportTo(exporter);

    return 0;
}


#if false


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



    // std::unique_ptr<Benchmark> b = Benchmark::get_benchmark("test_benchmark");
    // BenchmarkResults results = b->execute();
    // show_output("TestBenchmark", results);

    // b = Benchmark::get_benchmark("Fibbonachi");
    // results = b->execute();
    // show_output("Fibbonachi", results);

    return 0;
}

#endif