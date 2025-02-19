#ifndef _BENCHY_DEFINES_H
#define _BENCHY_DEFINES_H

#include "benchmark.hpp"
#include "benchmark_context.hpp"

#include <memory>

#define __BENCHMARK_CONCAT(a, b, c) __BENCHMARK_CONCAT2(a, b, c)
#define __BENCHMARK_CONCAT2(a, b, c) a##b##c

#define __BENCHMARK_NAME(f) __BENCHMARK_CONCAT(Benchmark_, f, __COUNTER__)


#define MICROBENCHMARK(function) static auto const* __BENCHMARK_NAME(function) = (MicroBenchmarks::RegisterBenchmark(#function, &function))->Iterations(1)

// #define MICROBENCHMARK(name, content) \
// class Bench_##name : public Benchmark { \
// public: \
//     Bench_##name() \
//     : Benchmark(1, 0, 10, true) { } \
//     void bench_fn(BenchmarkContext &context) override { \
//         content \
//     } \
//     static std::unique_ptr<Benchmark> create_benchmark() { \
//         return std::make_unique<Bench_##name>(); \
//     } \
// private: \
//     static bool s_registered; \
// }; \
// bool Bench_##name::s_registered = Benchmark::register_benchmark(#name, Bench_##name::create_benchmark);

#endif // _BENCHY_DEFINES_H