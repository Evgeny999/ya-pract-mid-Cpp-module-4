#pragma once
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

#include "metric.hpp"

namespace rv = std::ranges::views;
namespace rs = std::ranges;

namespace analyser::metric_accumulator {

struct IAccumulator {
    virtual void Accumulate(const metric::MetricResult &metric_result) = 0;
    virtual void Finalize() = 0;
    virtual void Reset() = 0;
    virtual ~IAccumulator() = default;

    bool IsFinalized() const { return is_finalized; }

protected:
    bool is_finalized = false;
};

struct MetricsAccumulator {
    template <typename Accumulator>
    void RegisterAccumulator(const std::string &metric_name, std::unique_ptr<Accumulator> acc) {
        if (accumulators.contains(metric_name)) {
            throw std::runtime_error(std::format("accumulator for metric {} already exists", metric_name));
        }
        accumulators[metric_name] = std::move(acc);
    }
    template <typename Accumulator>
    const Accumulator &GetFinalizedAccumulator(const std::string &metric_name) const {
        auto it = accumulators.find(metric_name);
        if (it == accumulators.end()) {
            throw std::runtime_error("this accumulator does not exist");
        }

        if (!it->second->IsFinalized()) {
            throw std::runtime_error(std::format("Accumulator is not finalized"));
        }

        auto acc_ptr = dynamic_cast<Accumulator *>(it->second.get());
        if (acc_ptr == nullptr) {
            throw std::runtime_error(std::format("Failed to get accumulataor for metric {}", metric_name));
        }

        return *acc_ptr;
    }
    void AccumulateNextFunctionResults(const std::vector<metric::MetricResult> &metric_results) const;

    void ResetAccumulators();

private:
    std::unordered_map<std::string, std::shared_ptr<IAccumulator>> accumulators;
};

}  // namespace analyser::metric_accumulator
