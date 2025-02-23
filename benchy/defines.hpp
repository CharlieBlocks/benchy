#ifndef _BENCHY_DEFINES_H
#define _BENCHY_DEFINES_H

#include "benchmark.hpp"


#define __BENCHMARK_CONCAT(a, b, c) __BENCHMARK_CONCAT2(a, b, c)
#define __BENCHMARK_CONCAT2(a, b, c) a##b##c

#define __BENCHMARK_NAME(f) __BENCHMARK_CONCAT(Benchmark_, f, __COUNTER__)

#define MICROBENCHMARK(function) static auto const* __BENCHMARK_NAME(function) = (MicroBenchmarks::RegisterBenchmark(#function, &function))->Iterations(1)


#endif // _BENCHY_DEFINES_H