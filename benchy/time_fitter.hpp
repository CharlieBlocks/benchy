#ifndef _BENCHY_TIME_FITTER_H
#define _BENCHY_TIME_FITTER_H

#include <memory>

#include "benchmark.hpp"

template<typename _InstanceType>
class TimeFitter {
public:
    TimeFitter(std::shared_ptr<__Benchmark<_InstanceType>> benchmark, _InstanceType *instance, int varIdx) 
    : _benchmark(benchmark), _instance(instance), _varIdx(varIdx)
    { }

    void Fit() {

        while (true) {

            BenchmarkContext result = _benchmark->Execute(_instance, _varIdx);

            if (result.get_cpu_time() > BENCHY_MIN_TIME ||
                result.get_cpu_time() > BENCHY_MAX_TIME)
                break;
            
            double timePerIter = (double)result.get_cpu_time().get() / _benchmark->_iterationCount;

            // Calculate required time
            unsigned long requiredIter = BENCHY_MIN_TIME.as<units::nanoseconds>().get() / timePerIter;
            _benchmark->_iterationCount = requiredIter;

            if (requiredIter - _benchmark->_iterationCount < BENCHY_MIN_CHANGE) {
                break;
            }

            if (requiredIter > BENCHY_MAX_ITERATIONS) {
                _benchmark->_iterationCount = BENCHY_MAX_ITERATIONS;
                break;
            }
        }

    }

private:
    std::shared_ptr<__Benchmark<_InstanceType>> _benchmark;
    _InstanceType *_instance;
    int _varIdx;
};


#endif // _BENCHY_TIME_FITTER_H