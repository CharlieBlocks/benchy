#ifndef _BENCHY_BENCHMARK_H
#define _BENCHY_BENCHMARK_H

#include "time.hpp"
#include "benchmark_context.hpp"
#include "series.hpp"
#include "argument_providers.hpp"

#include <chrono>
#include <map>
#include <sstream>

// TODO: Define in Benchy class
#define BENCHY_MIN_TIME time_span<units::milliseconds>(500)
#define BENCHY_MAX_TIME time_span<units::seconds>(1)
#define BENCHY_MAX_ITERATIONS 50'000'000
#define BENCHY_MIN_CHANGE 100

#define _TYPE_NAME(x) #x

template<typename _T, _T _v>
struct constexpr_value {
    static constexpr _T value = _v;
    using type = _T;
};


/*
A data structure that holds the results of the benchmark
*/
struct BenchmarkResults {
    BenchmarkResults() = default;
    BenchmarkResults(int reserveSize)
    : cpuData(reserveSize), realData(reserveSize) { }

    // Copy Constructor
    BenchmarkResults(const BenchmarkResults &other)
    : cpuData(other.cpuData),
        realData(other.realData),
        iterationCount(other.iterationCount),
        runCount(other.runCount)
    {
        std::cout << "Warning. BenchmarkResults copy constructor called!" << std::endl;
    }

    // Move Constructor
    BenchmarkResults(const BenchmarkResults &&other)
    : cpuData(std::move(other.cpuData)),
        realData(std::move(other.realData)),
        iterationCount(other.iterationCount),
        runCount(other.runCount)
    {}

    DataSeries<double> cpuData;
    DataSeries<double> realData;

    int iterationCount;
    int runCount;
};


template<typename _InstanceType>
struct __Benchmark {
    virtual int GetVariationCount() = 0;
    virtual std::string GetVariationDefinition(int idx) = 0;
    virtual BenchmarkContext Execute(_InstanceType *instance, int variationIdx) = 0;
};

struct BenchmarkMetadata {
    unsigned int _warmupCount;
    unsigned int _iterationCount;
    unsigned int _runCount;
    bool _disableAutoFit;

    // Constructors
    inline constexpr BenchmarkMetadata()
    : _warmupCount(0),
        _iterationCount(1),
        _runCount(1),
        _disableAutoFit(false)
    { }
    inline constexpr BenchmarkMetadata(int warmupCount, int iterationCount, int runCount, bool disableAutoFit) 
    : _warmupCount(warmupCount),
        _iterationCount(iterationCount),
        _runCount(runCount),
        _disableAutoFit(disableAutoFit)
    { }

    // Copy Constructor
    inline constexpr BenchmarkMetadata(const BenchmarkMetadata &other)
    : _warmupCount(other._warmupCount),
        _iterationCount(other._iterationCount),
        _runCount(other._runCount),
        _disableAutoFit(other._disableAutoFit)
    { }

    // Move Constructor
    inline constexpr BenchmarkMetadata(const BenchmarkMetadata &&other)
    : _warmupCount(other._warmupCount),
        _iterationCount(other._iterationCount),
        _runCount(other._runCount),
        _disableAutoFit(other._disableAutoFit)
    { }


    // Copy Operator
    inline BenchmarkMetadata &operator =(const BenchmarkMetadata &other) {
        this->_warmupCount = other._warmupCount;
        this->_iterationCount = other._iterationCount;
        this->_runCount = other._runCount;
        this->_disableAutoFit = other._disableAutoFit;

        return *this;
    }

    // Move Operator
    inline BenchmarkMetadata &operator =(const BenchmarkMetadata &&other) {
        this->_warmupCount = other._warmupCount;
        this->_iterationCount = other._iterationCount;
        this->_runCount = other._runCount;
        this->_disableAutoFit = other._disableAutoFit;

        return *this;
    }
};

template<typename _InstanceType, typename ...Args>
struct ClassBenchmark : 
    public __Benchmark<_InstanceType>, 
    public BenchmarkMetadata 
{
public:
    ClassBenchmark(
        std::string name,
        void (_InstanceType::*benchFunc)(BenchmarkContext &, Args...)
    ) 
    : BenchmarkMetadata(),
        _name(name),
        _benchFunc(benchFunc),
        _argumentProviders({})
    { }

    // Deleted Copy constructor
    ClassBenchmark(const ClassBenchmark &other) = delete;

    // Move Constructor
    ClassBenchmark(const ClassBenchmark &&other) 
    : BenchmarkMetadata(std::move(other.BenchmarkMetadata)),
        _name(std::move(other._name)),
        _benchFunc(std::move(other._benchFunc)),
        _argumentProviders(std::move(other._argumentProviders))
    { }

    // Deleted Copy Assignment Operator
    inline ClassBenchmark &operator =(const ClassBenchmark &other) = delete;

    // Move Assignment Operator
    inline ClassBenchmark &operator =(ClassBenchmark &&other) {
        BenchmarkMetadata::operator=(other);

        this->_name = std::move(other._name);
        this->_benchFunc = other._benchFunc;
        this->_argumentProviders = std::move(other._argumentProviders);

        return *this;
    }


    /* Setters */
    decltype(auto) Params(ArgumentContainer<Args...> arguments) {
        _argumentProviders.push_back(std::make_unique(arguments));
        return this;
    }

    template<int N>
    decltype(auto) Params(ArgumentMatrix<N, Args...> arguments) {
        _argumentProviders.push_back(std::make_unique(arguments));
        return this;
    }

    decltype(auto) Iterations(int iter) {
        _iterationCount = iter;
        return this;
    }

    decltype(auto) Warmups(int iter) {
        _warmupCount = iter;
        return this;
    }

    decltype(auto) RunCount(int runs) {
        _runCount = runs;
        return this;
    }

    decltype(auto) DisableAutoFit() {
        _disableAutoFit = true;
        return this;
    }



    // __Benchmark<_InstanceType> implementation
    inline int GetVariationCount() override { 
        if constexpr (sizeof...(Args) == 0)
            return 1;

        int sum = 0;
        for (size_t i = 0; i < _argumentProviders.size(); ++i) {
            sum += _argumentProviders[i]->VariationCount();
        }
        
        return sum;
    }
    inline std::string GetVariationDefinition(int idx) override {
        int subIdx = 0;
        idx = calculate_variation_idx(idx, &subIdx);

        if (idx != -1) {
            auto args = _argumentProviders[idx]->GetVariation(subIdx);
            return create_function_definition(
                args
            );
        } else {
            return create_function_definition();
        }
    }

    BenchmarkContext Execute(_InstanceType *instance, int varIdx) override {
      
        BenchmarkContext context(_iterationCount, _warmupCount);

        if constexpr (sizeof...(Args) == 0) {
            // Run Function
            execute_run(instance, context);
        }
        else {
            int subIdx = 0;
            varIdx = execute_run(varIdx, &subIdx);
            
            execute_run(instance, context, _argumentProviders[varIdx]->GetVariation(subIdx));
        }

        return context;
    }

private:
    inline int calculate_variation_idx(int varIdx, int *subVariationIdx) {
        int currSum = 0;
        for (int i = 0; i < _argumentProviders.size(); ++i) {
            *subVariationIdx = currSum - varIdx;
            currSum += i;
            if (varIdx < currSum)
                return i;
        }

        return -1;
    }

    /* Benchmark Execution */
    inline void execute_run(_InstanceType *instance, BenchmarkContext &context) {
        (instance->*_benchFunc)(context);

        if (context.state() != BenchmarkState::Done)
            std::cout << "[WARNING]: Benchmark " << _name << " did not complete all iterations. Was the benchmark malformed?" << std::endl;
    }

    inline void execute_run(_InstanceType *instance, BenchmarkContext &context, std::tuple<Args &&...> &args) {
        call_function(instance, context, args, std::make_integer_sequence<int, sizeof...(Args)>());

        if (context.state() != BenchmarkState::Done)
            std::cout << "[WARNING]: Benchmark " << _name << " did not complete all iterations. Was the benchmark malformed?" << std::endl;
    }

    template<int ...S>
    inline void call_function(_InstanceType *instance, BenchmarkContext &context, std::tuple<Args &&...> &args, std::integer_sequence<int, S...>) {
        (instance->*_benchFunc)(context, std::forward(std::get<S>(args))...);
    }


    /* Building Function Definition*/
    template<int N, int n>
    void build_arg_list(std::stringstream &stream, std::tuple<Args &&...> &args) {
        stream << _TYPE_NAME(decltype(std::get<n>(args))) << std::to_string(std::get<n>(args));
        if constexpr (n == N - 1)
            return;
        
        stream << ", ";
        build_arg_list<N, n + 1>(stream, args);
    }

    std::string create_function_definition(std::tuple<Args &&...> &args) {
        std::stringstream ss;
        ss << _name << "(";

        build_arg_list(ss, args);

        ss << ")";

        return ss.str();
    }

    std::string create_function_definition() {
        std::stringstream ss;
        ss << _name << "(BenchmarkContext &context)";
        
        return ss.str();
    }


private:
    std::string _name;

    void (_InstanceType::*_benchFunc)(BenchmarkContext &, Args...);

    std::vector<std::unique_ptr<ArgumentProvider<Args...>>> _argumentProviders;
};




template<typename ...Args>
struct FunctionBenchmark :
    public __Benchmark<void>,
    public BenchmarkMetadata
{
public:
    FunctionBenchmark(
        std::string name,
        void (*benchFunc)(BenchmarkContext &, Args...)
    )
    : BenchmarkMetadata({ 0, 1, 5, false }),
        _name(name),
        _benchFunc(benchFunc),
        _argumentProviders()
    { }

    // Deleted Copy Constructor
    FunctionBenchmark(const FunctionBenchmark &other) = delete;

    // Move Constructor
    FunctionBenchmark(FunctionBenchmark &&other) 
    : BenchmarkMetadata(std::move(other.BenchmarkMetadata)),
        _name(std::move(other._name)),
        _benchFunc(std::move(other._benchFunc)),
        _argumentProviders(std::move(other._argumentProviders))
    { }

    // Deleted Copy Assignment Operator
    inline FunctionBenchmark &operator =(const FunctionBenchmark &other) = delete;

    // Move Assignment Operator
    inline FunctionBenchmark &operator =(FunctionBenchmark &&other) {
        BenchmarkMetadata::operator=(other);

        this->_name = std::move(other._name);
        this->_benchFunc = other._benchFunc;
        this->_argumentProviders = std::move(other._argumentProviders);

        return *this;
    }


    /* Setters */
    decltype(auto) Params(ArgumentContainer<Args...> arguments) {
        _argumentProviders.push_back(std::make_unique(arguments));
        return this;
    }

    template<int N>
    decltype(auto) Params(ArgumentMatrix<N, Args...> arguments) {
        _argumentProviders.push_back(std::make_unique(arguments));
        return this;
    }

    decltype(auto) Iterations(int iter) {
        _iterationCount = iter;
        return this;
    }

    decltype(auto) Warmups(int iter) {
        _warmupCount = iter;
        return this;
    }

    decltype(auto) RunCount(int runs) {
        _runCount = runs;
        return this;
    }

    decltype(auto) DisableAutoFit() {
        _disableAutoFit = true;
        return this;
    }

    // __Benchmark<void> implementation    
    inline int GetVariationCount() override {
        if constexpr (sizeof...(Args) == 0)
            return 1;

        int sum = 0;
        for (size_t i = 0; i < _argumentProviders.size(); ++i) {
            sum += _argumentProviders[i]->VariationCount();
        }
        
        return sum;
    }
    inline std::string GetVariationDefinition(int idx) override {
        int subIdx = 0;
        idx = calculate_variation_idx(idx, &subIdx);

        if (idx != -1) {
            auto args = _argumentProviders[idx]->GetVariation(subIdx);
            return create_function_definition(
                args
            );
        } else {
            return create_function_definition();
        }
    }

    BenchmarkContext Execute(void *, int varIdx) override {
        BenchmarkContext context(_iterationCount, _warmupCount);

        if constexpr (sizeof...(Args) == 0)
            execute_run(context);
        else {
            int subIdx = 0;
            varIdx = execute_run(varIdx, &subIdx);
            
            execute_run(context, _argumentProviders[varIdx]->GetVariation(subIdx));
        }

        return context;
    }

private:
    inline int calculate_variation_idx(int varIdx, int *subVariationIdx) {
        int currSum = 0;
        for (int i = 0; i < _argumentProviders.size(); ++i) {
            *subVariationIdx = currSum - varIdx;
            currSum += i;
            if (varIdx < currSum)
                return i;
        }

        return -1;
    }

    /* Benchmark Execution */
    inline void execute_run(BenchmarkContext &context) {
        _benchFunc(context);

        if (context.state() != BenchmarkState::Done)
            std::cout << "[WARNING]: Benchmark " << _name << " did not complete all iterations. Was the benchmark malformed?" << std::endl;
    }

    inline void execute_run(BenchmarkContext &context, std::tuple<Args &&...> &args) {
        call_function(context, args, std::make_integer_sequence<int, sizeof...(Args)>());

        if (context.state() != BenchmarkState::Done)
            std::cout << "[WARNING]: Benchmark " << _name << " did not complete all iterations. Was the benchmark malformed?" << std::endl;
    }

    template<int ...S>
    inline void call_function(BenchmarkContext &context, std::tuple<Args &&...> &args, std::integer_sequence<int, S...>) {
        _benchFunc(context, std::forward(std::get<S>(args))...);
    }


    /* Building Function Definition*/
    template<int N = sizeof...(Args), int n = 0>
    void build_arg_list(std::stringstream &stream, std::tuple<Args &&...> &args) {
        stream << _TYPE_NAME(decltype(std::get<n>(args))) << std::to_string(std::get<n>(args));
        if constexpr (n == N - 1)
            return;
        
        stream << ", ";
        build_arg_list<N, n + 1>(stream, args);
    }

    std::string create_function_definition(std::tuple<Args &&...> &args) {
        std::stringstream ss;
        ss << _name << "(BenchmarkContext &context";
        
        if constexpr (sizeof...(Args) > 0)
            build_arg_list(ss, args);

        ss << ")";

        return ss.str();
    }

    std::string create_function_definition() {
        std::stringstream ss;
        ss << _name << "(BenchmarkContext &context)";
        
        return ss.str();
    }

private:
    std::string _name;

    void (*_benchFunc)(BenchmarkContext &, Args...);

    std::vector<std::unique_ptr<ArgumentProvider<Args...>>> _argumentProviders;
};



class MicroBenchmarks {
public:

    template<typename ...Args>
    static std::shared_ptr<FunctionBenchmark<Args...>> RegisterBenchmark(
        std::string name,
        void (*benchFunc)(BenchmarkContext &, Args...)
    ) {
        std::shared_ptr<FunctionBenchmark<Args...>> ptr = std::make_shared<FunctionBenchmark<Args...>>(name, benchFunc);
        get_benchmark_list().push_back(ptr);

        return ptr;
    }

    static std::vector<std::shared_ptr<__Benchmark<void>>> &get_benchmark_list();
};

// template<typename ...Args>
// struct FunctionBenchmark : public __Benchmark<void> {

// }


// class BenchmarkExecutor {
// public:

//     virtual ~BenchmarkExecutor() = default;

//     virtual void execute() = 0;

//     // virtual void export(Exporter &exporter) = 0;
// };


// class ComponentBenchmark {

// };



// template<typename ...Args>
// class MicroBenchmark : public Benchmark {
// public:
//     MicroBenchmark();

//     void RegisterBenchmark() {

//     }
// };





class Benchmark {
public:
    /*
    Constructor for the Benchmark type.
    It takes the number of iterations to perform per run,
    the number of warmup iterations to perform before the actual benchmark runs,
    the total number of times to run the benchmark,
    and whether to adjust the number of iterations ran
    */
    Benchmark(unsigned int iterations, unsigned int warmupIterations, unsigned int runCount, bool adjustIterations);

    virtual ~Benchmark() = default;


    /*
    The actual benchmarking function to be run.
    This is expected to wrap the test code that you want to time
    in the context.
    */
    virtual void bench_fn(BenchmarkContext &context) = 0;


    /*
    Executes the benchmark, including the fit time for the requested number of 
    runs. The results are returned in a BenchmarkResults structure
    */
    BenchmarkResults execute();

public:
    using create_fn = std::unique_ptr<Benchmark>(*)();

    static bool register_benchmark(const std::string name, create_fn createFn);


    // TEMPORARY
    static std::unique_ptr<Benchmark> get_benchmark(const std::string name) {
        return registeredBenchmarks()[name]();
    }

private:
    /*
    Runs the benchmark for one run and checks what the returned time was,
    if the time is insignificant then the number of iterations will be increased
    until the time is expected to provide reliable results.
    */
    void check_time();


    /* 
    A helper function used by the check_time function. This runs the benchmark
    for a single instance and returns the resultant context
    */
    BenchmarkContext run_one();


    /*
    Returns the static variable containing the registered benchmarks. Unfortunatly we can not
    use a static member variable as we cannot guarantee that it will be initialised before the first
    call. Instead we use a function with a static variable inside it, this makes use of Construct On First Use
    to ensure the variable is created before an insertion is made.
    */
    static std::map<const std::string, create_fn> &registeredBenchmarks();

private:    
    unsigned int m_iterations;
    unsigned int m_warmupIterations;
    unsigned int m_runCount;
    bool m_adjustIterations;
};



template<typename T>
class BenchmarkRegisterHelper {
protected:
    static bool s_wasRegistered;
};

#endif // _BENCHY_BENCHMARK_H