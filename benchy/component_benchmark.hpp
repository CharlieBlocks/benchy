#include <tuple>


class AnonymousArgumentProvider {
public:

    virtual int VariationCount();

    virtual void *GetVariationPtr(int idx, int *size);

};


template<typename ...Args>
class ArgumentProvider : AnonymousArgumentProvider {
public:

    /// @brief Gets a argument list of a specific variation
    /// @param idx The variation to fetch. In range [0, variation_count())
    /// @return A tuple containg the args
    virtual std::tuple<Args...> &GetVariation(int idx);

    void *GetVariationPtr(int idx, int *size) override final {
        std::tuple<Args...> data = GetVariation(idx);

        *size = sizeof(Args...);
        return &data;
    }
};


/// @brief Superclass of ArgumentProvider. Contains a list of argument combinations
/// @tparam ...Args The arguments to hold
template<int _VariationCount, typename ...Args>
class ArgumentContainer : public AnonymousArgumentProvider<Args...> {
public:
    ArgumentContainer(std::tuple<Args...> initialiser[_VariationCount]) 
    : __argumentVariations(initialiser)
    { }

    constexpr inline int variation_count() override noexcept final { return _VariationCount; }

    inline std::tuple<Args...> get_variation(int idx) override final {
        if (idx < 0 || idx >= _VariationCount)
            throw 0;
        return __argumentVariations[idx];
    }

private:
    std::tuple<Args...> __argumentVariations[_VariationCount];
};


struct BenchmarkMetadata {
public:
    int iterationCount;
    int warmupCount;
    // etc...
};

template<typename _Func>
class BenchmarkRegistra {
public:
    typedef void (*RegisterFn)(_Func, BenchmarkMetadata);

    BenchmarkRegistra(RegisterFn callback) 
    : callbackFn(callback), metadata({ }) {

    }

    inline decltype(auto) Iterations(int iter) {
        metadata.iterationCount = iter;
        return this;
    }

    inline decltye(auto) Warmups(int iter) {
        metadata.warmupCount = iter;
        return this;
    }

    void Register() {
        callbackFn(_Func, metadata);
    }

private:
    RegisterFn callbackFn;
    BenchmarkMetadata metadata;
};

template<typename _InstanceType>
struct __RegisteredBenchmark {
public:
    __RegisteredBenchmark(AnonymousArgumentProvider provider)
    : argumentProvider(provider) { }

    virtual void Invoke(_InstanceType *instance, void *data);

    AnonymousArgumentProvider &GetArgumentProvider() {
        return argumentProvider;
    }


private:
    AnonymousArgumentProvider argumentProvider;
};

template<typename _BaseType, typename ...Args>
struct RegisteredBenchmark : public __RegisteredBenchmark<_BaseType> {
public:
    RegisteredBenchmark(
        void (_BaseType::*fn)(Args...), 
        void(_BaseType::*setupFunc)(void), 
        AnonymousArgumentProvider argProvider,
        BenchmarkMetadata metadata
    ) 
    : __RegisteredBenchmark(argProvider),
      _benchFn(fn),
      _setupFunc(setupFunc) 
    { }


    void Invoke(_BaseType *instance, void *data) override final {
        (instance->*_setupFunc)();
        (instance->*_benchFn)((std::tuple<Args...>(data));
    }


private:
    void (_BaseType::*_benchFn)(Args...);
    void (_BaseType::*_setupFunc)(void);

    BenchmarkMetadata _metadata;
};

class ComponentBenchmark {
public:
    using _SetupFunc = void (*)(void);
public:
    ComponentBenchmark() = default;
    ~ComponentBenchmark() = default;

    template<typename _SuperClass, typename ...Args>
    void RegisterBenchmark(void (_SuperClass::*benchFunc)(Args...), _SetupFunc setupFunc, AnonymousArgumentProvider container);


private:

private:
    std::map<std::string, 


};