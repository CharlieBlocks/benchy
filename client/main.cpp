#include "benchy/benchy.hpp"

int fibbonachi(int n) {
    if (n < 3)
        return 1;
    return fibbonachi(n-1) + fibbonachi(n-2);
}

void test_func(BenchmarkContext &context, int n) {
    while (context.next()) {
        fibbonachi(n);
    }

}

int x = 10;

MICROBENCHMARK(test_func)
->Warmups(50)
->Iterations(10'000'000)
->RunCount(1)
->Params({ 10 })
->Name("BM_Fibbonachi");


MICROBENCHMARK(test_func)
->Warmups(50)
->Iterations(10'000'000)
->RunCount(1)
->Params({ x })
->Name("BM_FibbonachiParam");