#pragma once
#include <random>

class NumberGenerator {
protected:
    std::random_device randomDevice;
    std::mt19937 generator;

    NumberGenerator()
        : generator(randomDevice())
    {}
};

template<typename T>
class RealNumberGenerator : public NumberGenerator {
private:
    std::uniform_real_distribution<T> distribution;

public:
    RealNumberGenerator()
        : NumberGenerator(), distribution(0.0, 1.0)
    {}

    RealNumberGenerator(const RealNumberGenerator& other)
        : NumberGenerator(), distribution(other.distribution)
    {}

    T getValue() {
        return distribution(generator);
    }

    T getUnder(T max) {
        return distribution(generator) * max;
    }

    T getRange(T min, T max) {
        return min + distribution(generator) * (max - min);
    }

    bool probability(T threshold) {
        return getValue() < threshold;
    }
};

template<typename T>
class IntegerNumberGenerator : public NumberGenerator {
public:
    IntegerNumberGenerator() = default;

    T getUnder(T max) {
        std::uniform_int_distribution<T> dist(0, max);
        return dist(generator);
    }

    T getRange(T min, T max) {
        std::uniform_int_distribution<T> dist(min, max);
        return dist(generator);
    }
};

template<typename T>
class RNG {
public:
    static T get() {
        static RealNumberGenerator<T> realGen;
        return realGen.getValue();
    }

    static T getUnder(T max) {
        static RealNumberGenerator<T> realGen;
        return realGen.getUnder(max);
    }

    static T getRange(T min, T max) {
        static RealNumberGenerator<T> realGen;
        return realGen.getRange(min, max);
    }

    static bool proba(T threshold) {
        static RealNumberGenerator<T> realGen;
        return realGen.probability(threshold);
    }
};

using RNGf = RNG<float>;

template<typename T>
class RNGi {
public:
    static T getUnder(T max) {
        static IntegerNumberGenerator<T> intGen;
        return intGen.getUnder(max);
    }

    static T getRange(T min, T max) {
        static IntegerNumberGenerator<T> intGen;
        return intGen.getRange(min, max);
    }
};

using RNGi32 = RNGi<int32_t>;
using RNGi64 = RNGi<int64_t>;
using RNGu32 = RNGi<uint32_t>;
using RNGu64 = RNGi<uint64_t>;