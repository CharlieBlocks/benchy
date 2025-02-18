#include "benchy/benchy.hpp"
#include "benchy/time.hpp"

#include <iostream>
#include <tuple>


// template<typename ...Args>
// class ArgumentProvider {
// public:
//     virtual ~ArgumentProvider() = default;

//     virtual int VariationCount() = 0;

//     virtual std::tuple<Args...> GetVariation(int idx) = 0;
// };

// template<typename ...Args>
// class ArgumentContainer : public ArgumentProvider<Args...> {
// public:
//     ArgumentContainer(Args &&... args) 
//     : _arguments(std::forward_as_tuple(args...))
//     { }

//     // Copy Constructor
//     ArgumentContainer(const ArgumentContainer &other)
//     : _arguments(other._arguments)
//     { }

//     // Move Constructor
//     ArgumentContainer(ArgumentContainer &&other)
//     : _arguments(std::move(other._arguments))
//     { }

//     int VariationCount() noexcept override {
//         return 1;
//     }

//     std::tuple<Args...> GetVariation(int idx) override {
//         return _arguments;
//     }

// private:
//     std::tuple<Args &&...> _arguments;
// };


// template<int N, typename ...Args>
// struct variation_collector {

//     template<int _Base, int _Pow>
//     static constexpr typename std::enable_if<_Pow == 0, int>::type pow() {
//         return 1;
//     }

//     template<int _Base, int _Pow>
//     static constexpr typename std::enable_if<_Pow == 1, int>::type pow() {
//         return _Base;
//     }

//     template<int _Base, int _Pow>
//     static constexpr typename std::enable_if<(_Pow > 1), int>::type pow() {
//         return _Base * pow<_Base, _Pow - 1>();
//     }



//     template<int n, typename Arg>
//     static typename std::enable_if<n == sizeof...(Args) - 1, std::tuple<Arg>>::type collect(std::tuple<std::array<Args, N>...> &data, int idx) {
//         return std::tuple<Arg>(std::get<n>(data)[idx % N]);
//     }

//     template<int n = 0, typename Arg, typename ...Rest>
//     static typename std::enable_if<n != sizeof...(Args) - 1, std::tuple<Arg, Rest...>>::type collect(std::tuple<std::array<Args, N>...> &data, int idx) {
//         return std::tuple_cat(
//             std::tuple<Arg>(std::get<n>(data)[idx / pow<N, sizeof...(Rest)>()]),
//             collect<n + 1, Rest...>(data, idx)
//         );
//     }

// };


// //
// // Params: a, b
// // a: (1, 2, 3)
// // b: (4, 5, 6)
// //
// // Total Combinations = len(a) * len(b)
// // 
// template<int N, typename ...Args>
// class ArgumentMatrix : public ArgumentProvider<Args...> {
// public:
//     ArgumentMatrix(std::array<Args, N>... argCollections) 
//     : _argMatrix(argCollections...)
//     { }

//     // Copy Constructor
//     ArgumentMatrix(const ArgumentMatrix<N, Args...> &other)
//     : _argMatrix(other._argMatrix)
//     { }

//     // Move Constructor
//     ArgumentMatrix(ArgumentMatrix<N, Args...> &&other)
//     : _argMatrix(std::move(other._argMatrix))
//     { }


//     // Variation Count
//     int VariationCount() noexcept override {
//         return std::pow(N, sizeof...(Args));
//     }

//     std::tuple<Args...> GetVariation(int idx) override {
//         return variation_collector<N, Args...>::template collect<0, Args...>(_argMatrix, idx);
//     }


// private:
//     std::tuple<std::array<Args, N>...> _argMatrix;
// };


// template<typename _InstanceType>
// struct _RegisteredBenchmark {

//     virtual void Invoke(_InstanceType *instance) = 0;

// };

// template<typename _InstanceType, typename ...Args>
// struct RegisteredBenchmark : public _RegisteredBenchmark<_InstanceType> {
// public:
//     RegisteredBenchmark(
//         std::string name, 
//         void (_InstanceType::*benchFunc)(Args...)
//     ) {
//         m_iterationCount = 1;
//         m_name = name;
//         m_benchFunc = benchFunc;
//     }

//     // Copy Constructor
//     RegisteredBenchmark(const RegisteredBenchmark &other)
//     : m_name(other.m_name), 
//         m_benchFunc(other.m_benchFunc), 
//         m_iterationCount(other.m_iterationCount),
//         m_arguments(other.m_arguments)
//     { }

//     // Move Constructor
//     RegisteredBenchmark(RegisteredBenchmark &&other)
//     : m_name(std::move(other.m_name)), 
//         m_benchFunc(std::move(other.m_benchFunc)),
//         m_iterationCount(std::move(other.m_iterationCount)),
//         m_arguments(std::move(other.m_arguments))
//     { }


//     void Invoke(_InstanceType *instance) override {
//         std::cout << "Iterations: " << m_iterationCount << std::endl;
//         std::cout << "Arg Count: " << m_arguments.size() << std::endl;

//         if constexpr (sizeof...(Args) == 0)
//             (instance->*m_benchFunc)(); // Invoke void function
//         else {
//             for (size_t i = 0; i < m_arguments.size(); i++) {
//                 for (int j = 0; j < m_arguments[i]->VariationCount(); j++)
//                     call_function(instance, m_arguments[i]->GetVariation(j), std::make_integer_sequence<int, sizeof...(Args)>());
//             }
//         }
//     }


//     // Setter Methods
//     decltype(auto) Params(ArgumentContainer<Args...> arguments) {
//         m_arguments.push_back(std::make_unique<ArgumentContainer<Args...>>(arguments));
//         return this;
//     }

//     template<int N>
//     decltype(auto) Params(ArgumentMatrix<N, Args...> arguments) {
//         m_arguments.push_back(std::make_unique<ArgumentMatrix<N, Args...>>(arguments));
//         return this;
//     }

//     decltype(auto) Iterations(int iter) {
//         m_iterationCount = iter;
//         return this;
//     }

// private:
//     template<int ...S>
//     void call_function(_InstanceType *instance, std::tuple<Args...> args, std::integer_sequence<int, S...>) {
//         (instance->*m_benchFunc)(std::get<S>(args)...);
//     }

// private:
//     std::string m_name;
//     void (_InstanceType::*m_benchFunc)(Args...);
//     std::vector<std::unique_ptr<ArgumentProvider<Args...>>> m_arguments;

//     int m_iterationCount;
// };


// class ComponentBenchmark {
// public:
    
//     template<typename _SuperClass, typename ...Args>
//     std::shared_ptr<RegisteredBenchmark<ComponentBenchmark, Args...>> RegisterBenchmark(
//         std::string name, 
//         void (_SuperClass::*benchFunc)(Args...)
//     ) {
//         m_registeredBenchmarks.push_back(
//             std::make_shared<RegisteredBenchmark<ComponentBenchmark, Args...>>(
//                 name,
//                 static_cast<void (ComponentBenchmark::*)(Args...)>(benchFunc)
//             )
//         );

//         return std::dynamic_pointer_cast<RegisteredBenchmark<ComponentBenchmark, Args...>>(m_registeredBenchmarks.at(m_registeredBenchmarks.size() - 1));
//     }


//     void RunAll() {
//         auto it = m_registeredBenchmarks.begin();
//         while (it != m_registeredBenchmarks.end()) {
//             (*it)->Invoke(this);
//             it++;
//         }
//     }


// private:
//     std::vector<std::shared_ptr<_RegisteredBenchmark<ComponentBenchmark>>> m_registeredBenchmarks;
// };



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


int main(void) {

    FunctionBenchmark<> bench(
        "test",
        &test_func
    );
    bench.Warmups(50);
    bench.Iterations(100'000);
    bench.RunCount(5);

    std::cout << bench.GetVariationCount() << std::endl;

    BenchmarkContext res = bench.Execute(nullptr, 0);

    std::cout << "CPU Time: " << res.get_cpu_time().as<units::microseconds>().get() << "ms" << std::endl;

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