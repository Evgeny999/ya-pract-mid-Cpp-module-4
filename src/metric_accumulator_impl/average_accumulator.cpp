#include "metric_accumulator_impl/average_accumulator.hpp"

#include <unistd.h>

#include <algorithm>
#include <array>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <ranges>
#include <sstream>
#include <string>
#include <variant>
#include <vector>

namespace analyser::metric_accumulator::metric_accumulator_impl {

void AverageAccumulator::Accumulate(const metric::MetricResult &metric_result) {
    sum += metric_result.value;
    ++count;
}

void AverageAccumulator::Finalize() {
    average = static_cast<double>(sum) / count;
    is_finalized = true;
}

void AverageAccumulator::Reset() {
    sum = 0;
    count = 0;
    average = 0;
}

double AverageAccumulator::Get() const {
    if (is_finalized == false) {
        throw std::runtime_error("AverageAccumulator is not finalized");
    }
    return average;
}

// здесь ваш код

}  // namespace analyser::metric_accumulator::metric_accumulator_impl
