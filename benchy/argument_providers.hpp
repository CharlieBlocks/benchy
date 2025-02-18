#ifndef _BENCHY_ARG_PROVIDERS_H
#define _BENCHY_ARG_PROVIDERS_H

#include <tuple>
#include <type_traits>
#include <array>


// Power utility
inline constexpr int pow(const int base, unsigned int const exponent) {
    return (exponent == 0)     
            ? 1 
            : (exponent % 2 == 0) 
              ? pow(base, exponent/2)*pow(base, exponent/2) 
              : base * pow(base, (exponent-1)/2) * pow(base, (exponent-1)/2);
}

template <int base, unsigned int exponent>
using pow_ = std::integral_constant<int, pow(base, exponent)>;


// Variation Selector Utility
template<int N, typename ...Args>
struct variation_collector {

    template<int n, typename Arg>
    static typename std::enable_if<n == sizeof...(Args) - 1, std::tuple<Arg &&>>::type collect(std::tuple<std::array<Args &&, N>...> &data, int idx) {
        return std::tuple<Arg>(std::forward(std::get<n>(data)[idx % N]));
    }

    template<int n = 0, typename Arg, typename ...Rest>
    static typename std::enable_if<n != sizeof...(Args) - 1, std::tuple<Arg &&, Rest &&...>>::type collect(std::tuple<std::array<Args &&, N>...> &data, int idx) {
        return std::tuple_cat(
            std::tuple<Arg &&>(std::forward(std::get<n>(data)[idx / pow_<N, sizeof...(Rest)>::value])),
            collect<n + 1, Rest...>(data, idx)
        );
    }

};


template<typename ...Args>
struct ArgumentProvider {
public:
    virtual ~ArgumentProvider() = default;

    virtual int VariationCount() = 0;

    virtual std::tuple<Args &&...> &&GetVariation(int idx) = 0;
};


template<typename ...Args>
class ArgumentContainer : public ArgumentProvider<Args...> {
public:
    // Standard Constructor
    ArgumentContainer(Args &&... args)
    : _arguments(std::forward_as_tuple(args...))
    { }

    // Copy Constructor
    ArgumentContainer(const ArgumentContainer<Args...> &other)
    : _arguments(other._arguments)
    { }

    // Move Constructor
    ArgumentContainer(const ArgumentContainer<Args...> &&other)
    : _arguments(std::move(other._arguments))
    {}


    // ArgumentProvider Implementation
    constexpr inline int VariationCount() noexcept override {
        return 1;
    }

    std::tuple<Args &&...> &&GetVariation(int idx) override {
        auto copy = _arguments;
        return std::move(copy);
    }


private:
    std::tuple<Args &&...> _arguments;
};


template<int N, typename ...Args>
class ArgumentMatrix : public ArgumentProvider<Args...> {
public:
    ArgumentMatrix(std::array<Args &&, N>... argCollections)
    : _argMatrix(argCollections...)
    { }

    // Copy Constructor
    ArgumentMatrix(const ArgumentMatrix<N, Args...> &other)
    : _argMatrix(other._argMatrix)
    { }

    // Move Constructor
    ArgumentMatrix(const ArgumentMatrix<N, Args...> &&other)
    : _argMatrix(std::move(other._argMatrix))
    { }


    // ArgumentProvider Implementation
    constexpr inline int VariationCount() noexcept override {
        return std::pow(N, sizeof...(Args));
    }

    inline std::tuple<Args &&...> &&GetVariation(int idx) override {
        return std::move(variation_collector<N, Args...>::template collect<0, Args...>(_argMatrix, idx));
    }

private:
    std::tuple<std::array<Args &&, N>...> _argMatrix;
};


#endif // _BENCHY_ARG_PROVIDERS_H