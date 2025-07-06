#include "metric_accumulator.hpp"

#include <unistd.h>

#include <algorithm>
#include <any>
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

namespace analyser::metric_accumulator {

void MetricsAccumulator::AccumulateNextFunctionResults(const std::vector<metric::MetricResult> &metric_results) const {
    for (const auto &metric : metric_results) {
        accumulators.at(metric.metric_name).get()->Accumulate(metric);
    }
}

void MetricsAccumulator::ResetAccumulators() { accumulators.clear(); }

}  // namespace analyser::metric_accumulator
