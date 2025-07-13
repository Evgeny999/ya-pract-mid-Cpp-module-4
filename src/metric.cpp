#include "metric.hpp"

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
#include <unordered_set>
#include <variant>
#include <vector>

#include "function.hpp"

namespace analyser::metric {

void MetricExtractor::RegisterMetric(std::unique_ptr<IMetric> metric) { metrics.push_back(std::move(metric)); }

MetricResults MetricExtractor::Get(const function::Function &func) const {
    auto vec = metrics | rv::transform([func](const auto &metric) { return (*metric).Calculate(func); }) |
               std::ranges::to<std::vector>();

    return vec;
}

}  // namespace analyser::metric
