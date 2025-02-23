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
public:
    unsigned int _warmupCount;
    unsigned int _iterationCount;
    unsigned int _runCount;
    bool _disableAutoFit;

    /* Virtual Functions */
    virtual int GetVariationCount() = 0;
    virtual std::string GetVariationDefinition(int idx) = 0;
    virtual BenchmarkContext Execute(_InstanceType *instance, int variationIdx) = 0;

    // Constructors
    // inline constexpr __Benchmark()
    // : _warmupCount(0),
    //     _iterationCount(1),
    //     _runCount(1),
    //     _disableAutoFit(false)
    // { }
    // inline constexpr __Benchmark(int warmupCount, int iterationCount, int runCount, bool disableAutoFit) 
    // : _warmupCount(warmupCount),
    //     _iterationCount(iterationCount),
    //     _runCount(runCount),
    //     _disableAutoFit(disableAutoFit)
    // { }

    // Copy Constructor
    // inline constexpr __Benchmark(const __Benchmark &other)
    // : _warmupCount(other._warmupCount),
    //     _iterationCount(other._iterationCount),
    //     _runCount(other._runCount),
    //     _disableAutoFit(other._disableAutoFit)
    // { }

    // Move Constructor
    // inline constexpr __Benchmark(const __Benchmark &&other)
    // : _warmupCount(other._warmupCount),
    //     _iterationCount(other._iterationCount),
    //     _runCount(other._runCount),
    //     _disableAutoFit(other._disableAutoFit)
    // { }


    // Copy Operator
    inline __Benchmark &operator =(const __Benchmark &other) {
        this->_warmupCount = other._warmupCount;
        this->_iterationCount = other._iterationCount;
        this->_runCount = other._runCount;
        this->_disableAutoFit = other._disableAutoFit;

        return *this;
    }

    // Move Operator
    inline __Benchmark &operator =(const __Benchmark &&other) {
        this->_warmupCount = other._warmupCount;
        this->_iterationCount = other._iterationCount;
        this->_runCount = other._runCount;
        this->_disableAutoFit = other._disableAutoFit;

        return *this;
    }

};


template<typename _InstanceType, typename ...Args>
struct ClassBenchmark : 
    public __Benchmark<_InstanceType> {
public:
    ClassBenchmark(
        std::string name,
        void (_InstanceType::*benchFunc)(BenchmarkContext &, Args...)
    ) 
    : _name(name),
        _benchFunc(benchFunc),
        _argumentProviders({})
    { 
        this->_iterationCount = 1;
        this->_warmupCount = 0;
        this->_runCount = 1;
        this->_disableAutoFit = false;
    }

    // Deleted Copy constructor
    ClassBenchmark(const ClassBenchmark &other) = delete;

    // Move Constructor
    ClassBenchmark(const ClassBenchmark &&other) 
    : _name(std::move(other._name)),
        _benchFunc(std::move(other._benchFunc)),
        _argumentProviders(std::move(other._argumentProviders))
    { 
        this->_iterationCount = other._iterationCount;
        this->_warmupCount = other._warmupCount;
        this->_runCount = other._runCount;
        this->_disableAutoFit = other._disableAutoFit;
    }

    // Deleted Copy Assignment Operator
    inline ClassBenchmark &operator =(const ClassBenchmark &other) = delete;

    // Move Assignment Operator
    inline ClassBenchmark &operator =(ClassBenchmark &&other) {
        __Benchmark<_InstanceType>::operator=(other);

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
        this->_iterationCount = iter;
        return this;
    }

    decltype(auto) Warmups(int iter) {
        this->_warmupCount = iter;
        return this;
    }

    decltype(auto) RunCount(int runs) {
        this->_runCount = runs;
        return this;
    }

    decltype(auto) DisableAutoFit() {
        this->_disableAutoFit = true;
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
      
        BenchmarkContext context(this->_iterationCount, this->_warmupCount);

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
    public __Benchmark<void> {
public:
    FunctionBenchmark(
        std::string name,
        void (*benchFunc)(BenchmarkContext &, Args...)
    )
    : _name(name),
        _benchFunc(benchFunc),
        _argumentProviders()
    {
        _iterationCount = 1;
        _warmupCount = 0;
        _runCount = 1;
        _disableAutoFit = false;
    }

    // Deleted Copy Constructor
    FunctionBenchmark(const FunctionBenchmark &other) = delete;

    // Move Constructor
    FunctionBenchmark(FunctionBenchmark &&other) 
    : _name(std::move(other._name)),
        _benchFunc(std::move(other._benchFunc)),
        _argumentProviders(std::move(other._argumentProviders))
    {
        _iterationCount = other._iterationCount;
        _warmupCount = other._warmupCount;
        _runCount = other._runCount;
        _disableAutoFit = false;
    }

    // Deleted Copy Assignment Operator
    inline FunctionBenchmark &operator =(const FunctionBenchmark &other) = delete;

    // Move Assignment Operator
    inline FunctionBenchmark &operator =(FunctionBenchmark &&other) {
        __Benchmark::operator=(other);

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
        this->_iterationCount = iter;
        return this;
    }

    decltype(auto) Warmups(int iter) {
        this->_warmupCount = iter;
        return this;
    }

    decltype(auto) RunCount(int runs) {
        this->_runCount = runs;
        return this;
    }

    decltype(auto) DisableAutoFit() {
        this->_disableAutoFit = true;
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
        BenchmarkContext context(this->_iterationCount, this->_warmupCount);

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


#endif // _BENCHY_BENCHMARK_H