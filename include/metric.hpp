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
#include <print>
#include <ranges>
#include <sstream>
#include <string>
#include <unordered_set>
#include <variant>
#include <vector>

#include "function.hpp"

namespace fs = std::filesystem;
namespace rv = std::ranges::views;
namespace rs = std::ranges;

namespace analyser::metric {

struct MetricResult {
    using ValueType = int;
    // using ValueType = std::variant<int, std::string>; // если захотите
    // реализовывать метрику naming style
    std::string metric_name;  // Название метрики
    ValueType value;          // Значение метрики
};

struct IMetric {
    virtual ~IMetric() = default;
    MetricResult Calculate(const function::Function &f) const {
        return MetricResult{.metric_name = Name(), .value = CalculateImpl(f)};
    }

protected:
    virtual MetricResult::ValueType CalculateImpl(const function::Function &f) const = 0;
    virtual std::string Name() const = 0;
};

using MetricResults = std::vector<MetricResult>;

struct MetricExtractor {
    void RegisterMetric(std::unique_ptr<IMetric> metric);

    MetricResults Get(const function::Function &func) const;
    std::vector<std::unique_ptr<IMetric>> metrics;
};

namespace {
MetricResult::ValueType FindNumExpressions(const function::Function &f,
                                           const std::unordered_set<std::string> node_types) {
    /*std::println("***HERE1***");
    std::println("f.ast\n = {}", f.ast);*/
    return std::ranges::count_if(f.ast | std::views::split('\n'),  // Split by lines
                                 [&node_types](auto &&line_range) {
                                     std::string line(line_range.begin(), line_range.end());
                                     // std::println("line = {}", line);
                                     //   Check if any word in the line matches a target
                                     return std::ranges::any_of(
                                         line_range | std::views::split(' '),  // Split line into words
                                         [&node_types](auto &&word_range) {
                                             std::string word(word_range.begin(), word_range.end());
                                             // Ведущий символ '(' нас не интересует
                                             if (!word.empty() && word[0] == '(') {
                                                 word = word.substr(1);  // Remove first character
                                             }
                                             // std::println("word = {}", word);
                                             return !word.empty() && node_types.contains(word);
                                         });
                                 });
}
}  // namespace

}  // namespace analyser::metric
