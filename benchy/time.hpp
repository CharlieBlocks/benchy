#ifndef _BENCHY_TIME_HPP_
#define _BENCHY_TIME_HPP_


#define CALC_RATIO(ratio) ((double)ratio::Numerator / ratio::Denominator)
#define CONVERT_VALUE(x, oldRatio, newRatio) x / CALC_RATIO(oldRatio) * CALC_RATIO(newRatio)

template<int _Numerator, int _Denominator>
struct ratio {
    static const int Numerator = _Numerator;
    static const int Denominator = _Denominator;
};


namespace units {

    // Second divisions
    typedef ratio<1, 1> seconds;
    typedef ratio<1, 100> milliseconds;
    typedef ratio<1, 100'000> microseconds;
    typedef ratio<1, 100'000'000> nanoseconds;

    typedef ratio<60, 1> minutes;

}


template<typename _Ratio, typename _Storage = unsigned long>
struct time_span {
public:
    constexpr time_span() : value(0) { }
    constexpr time_span(const _Storage val) : value(val) { }

    // Copy Constructor
    constexpr time_span(const time_span &other) 
    : value(other.value) { }
    // Move Constructor
    constexpr time_span(const time_span &&other)
    : value(other.value) { }


    /*
    Gets the raw underlying value
    */
    constexpr inline _Storage get() const { return value; }

    /*
    Converts to a different time ratio
    */
    template<typename _NewRatio>
    constexpr inline time_span<_NewRatio, _Storage> as() const noexcept {
        // Convert value and return
        return time_span<_NewRatio, _Storage>(
            value * (((double)_Ratio::Numerator / _Ratio::Denominator) / ((double)_NewRatio::Numerator / _NewRatio::Denominator))
        );
    }

    /*
    Converts to a different storage type
    */
    template<typename _NewStorage>
    constexpr inline time_span<_Ratio, _NewStorage> cast() const noexcept {
        return time_span<_Ratio, _NewStorage>(
            static_cast<_NewStorage>(value)
        );
    }

    // Copy-Assignment Operator
    constexpr inline time_span &operator=(time_span &other) {
        this->value = other.value;
        return *this;
    }

    // Move-Assignment Operator
    constexpr inline time_span &operator=(time_span &&other) {
        this->value = other.value;
        return *this;
    }


    // (+-*/) operators
    template<typename _OtherRatio>
    constexpr inline time_span operator +(const time_span<_OtherRatio, _Storage> &rhs) const noexcept {
        return time_span<_Ratio, _Storage>(
            this->value + CONVERT_VALUE(rhs.value, _OtherRatio, _Ratio)
        );
    }
    template<typename _OtherRatio>
    constexpr inline time_span operator -(const time_span<_OtherRatio, _Storage> &rhs) const noexcept {
        return time_span<_Ratio, _Storage>(
            this->value - CONVERT_VALUE(rhs.value, _OtherRatio, _Ratio)
        );
    }
    template<typename _OtherRatio>
    constexpr inline time_span operator *(const time_span<_OtherRatio, _Storage> &rhs) const noexcept {
        return time_span<_Ratio, _Storage>(
            this->value * CONVERT_VALUE(rhs.value, _OtherRatio, _Ratio)
        );
    }
    template<typename _OtherRatio>
    constexpr inline time_span operator /(const time_span<_OtherRatio, _Storage> &rhs) const noexcept {
        return time_span<_Ratio, _Storage>(
            this->value / CONVERT_VALUE(rhs.value, _OtherRatio, _Ratio)
        );
    }

    // (+-*/)= operators
    template<typename _OtherRatio>
    constexpr inline time_span &operator +=(const time_span<_OtherRatio, _Storage> &rhs) noexcept {
        value += CONVERT_VALUE(rhs.value, _OtherRatio, _Ratio);
        return *this;
    }
    template<typename _OtherRatio>
    constexpr inline time_span &operator -=(const time_span<_OtherRatio, _Storage> &rhs) noexcept {
        value -= CONVERT_VALUE(rhs.value, _OtherRatio, _Ratio);
        return *this;
    }
    template<typename _OtherRatio>
    constexpr inline time_span &operator *=(const time_span<_OtherRatio, _Storage> &rhs) noexcept {
        value *= CONVERT_VALUE(rhs.value, _OtherRatio, _Ratio);
        return *this;
    }
    template<typename _OtherRatio>
    constexpr inline time_span &operator /=(const time_span<_OtherRatio, _Storage> &rhs) noexcept {
        value /= CONVERT_VALUE(rhs.value, _OtherRatio, _Ratio);
        return *this;
    }

    // Comparison Operators
    template<typename _OtherRatio>
    constexpr inline bool operator >(const time_span<_OtherRatio, _Storage> &rhs) const noexcept {
        return this->value > CONVERT_VALUE(rhs.value, _OtherRatio, _Ratio);
    }
    template<typename _OtherRatio>
    constexpr inline bool operator <(const time_span<_OtherRatio, _Storage> &rhs) {
        return this->value < CONVERT_VALUE(rhs.value, _OtherRatio, _Ratio);
    }
    template<typename _OtherRatio>
    constexpr inline bool operator >=(const time_span<_OtherRatio, _Storage> &rhs) {
        return this->value >= CONVERT_VALUE(rhs.value, _OtherRatio, _Ratio);
    }
    template<typename _OtherRatio>
    constexpr inline bool operator <=(const time_span<_OtherRatio, _Storage> &rhs) {
        return this->value <= CONVERT_VALUE(rhs.value, _OtherRatio, _Ratio);
    }

public:
    _Storage value;
};


#endif // _BENCHY_TIME_HPP_
