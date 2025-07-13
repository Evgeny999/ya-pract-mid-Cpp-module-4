#pragma once
#include <unistd.h>

#include <algorithm>
#include <array>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <print>
#include <ranges>
#include <sstream>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

#include "file.hpp"
#include "function.hpp"
#include "metric.hpp"
#include "metric_accumulator.hpp"
#include "metric_impl/code_lines_count.hpp"
#include "metric_impl/cyclomatic_complexity.hpp"
#include "metric_impl/naming_style.hpp"
#include "metric_impl/parameters_count.hpp"

namespace analyser {

namespace rv = std::ranges::views;
namespace rs = std::ranges;

namespace {
auto AnalyseFunctions(const std::vector<std::string> &files,
                      const analyser::metric::MetricExtractor &metric_extractor) {

    auto result =
        // Создание списка стуктур File на основе имён файлов
        files | rv::transform([](const auto &file) { return analyser::file::File{file}; }) |
        // Получаем список функций для отдельного файла из списка файлов
        rv::transform([](const auto &fileStruct) { return analyser::function::FunctionExtractor{}.Get(fileStruct); }) |
        rv::join | rv::transform([&metric_extractor](const auto &functionStruct) {
            return std::make_pair(functionStruct, metric_extractor.Get(functionStruct));
        });

    return result;
}
}  // namespace
/*
Возможно сигнатура функции должна быть auto SplitByClasses(auto &&analysis)?
Иначе нужен сложный воркэраунд т.к. по константному отображению диапазонга
нельзя итерироваться
*/
auto SplitByClasses(const auto &analysis) {
    // Step 1: Create a non-const view without using pipes
    auto non_const_view = std::views::all(const_cast<std::remove_cvref_t<decltype(analysis)> &>(analysis));

    // Step 2: Materialize using ranges::to
    auto materialized = non_const_view | std::ranges::to<std::vector>();

    auto filtered_view = materialized | std::views::filter([](const auto &function_info) {
                             return function_info.first.class_name.has_value();
                         });

    auto result = filtered_view | std::views::chunk_by([](const auto &function_first, const auto &function_second) {
                      return function_first.first.class_name.value() == function_second.first.class_name.value();
                  });

    return result;
}

auto SplitByFiles(const auto &analysis) {
    // Step 1: Create a non-const view without using pipes
    auto non_const_view = std::views::all(const_cast<std::remove_cvref_t<decltype(analysis)> &>(analysis));

    // Step 2: Materialize using ranges::to
    auto materialized = non_const_view | std::ranges::to<std::vector>();
    auto filtered_view = materialized | std::views::filter([](const auto &function_info) {
                             return function_info.first.class_name.has_value();
                         });

    auto result = filtered_view | std::views::chunk_by([](const auto &function_first, const auto &function_second) {
                      return function_first.first.class_name.value() == function_second.first.class_name.value();
                  });

    return result;
}

void AccumulateFunctionAnalysis(const auto &analysis,
                                const analyser::metric_accumulator::MetricsAccumulator &accumulator) {
    std::ranges::for_each(analysis,
                          [accumulator](const auto &elem) { accumulator.AccumulateNextFunctionResults(elem.second); });
}

}  // namespace analyser
