# TODO List
- [x] Add custom time division implementation
- [ ] Add defines
- [ ] Support component benchmarks as well as microbenchmarks
- [ ] Add output exporters
- [ ] Restructure to have main function

## Microbenchmark Requirements
- Low profile
- Configure iteraitons, warmups, etc...

```c++
static void BM_Test(BenchmarkContext &context) {
    while (context.next()) {
        ...
    }
}

#define MICROBENCHMARK(BM_Test).Iterations(...).Warmups(...)
```

## Component Benchmarks
- Interchangable/Reusable setup/teardown states
- Clustering of multiple benchmarks
- Individual iterations, warmups, etc... but with defaults

```c++

class BM_Component : public ComponentBenchmark {
public:
    BM_Component() {
        RegisterBenchmark(Bench1, Setup1, nullptr);
        RegisterBenchmark(Bench1, Setup2, ArgumentMatrix<int, int> { {0, 1}, {0, 1} } );
    }

    // Setup Functions
    void Setup1();
    void Setup2();

    // Bench functions
    void Bench1(int arg);
    void Bench2(int a, int b);

    REGISTER_BENCH(Bench1, Setup1).Arg(1);
    REGISTER_BENCH(Bench2, Setup2).Matrix(ArgumentMatrix<int, int> {
        {0, 1, 2},
        {1, 2, 3}
    });
};
REGISTER_COMPONENT(BM_Component)

```
