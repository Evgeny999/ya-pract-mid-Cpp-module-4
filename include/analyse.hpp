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
    std::vector<analyser::file::File> files_vector;
    // Создание списка стуктур File на основе имён файлов
    for (const auto &file : files) {
        files_vector.push_back(analyser::file::File{file});
    }

    std::vector<analyser::function::Function> functions_vector;
    for (const auto &file : files_vector) {
        // Получаем список функций для отдельного файла из списка файлов
        auto functions = analyser::function::FunctionExtractor{}.Get(file);
        // Объединяем список функций в один список
        functions_vector.insert(functions_vector.end(), functions.begin(), functions.end());
    }
    // Вуктор пар: объект функции - набор результатов вычисления метрик
    std::vector<std::pair<analyser::function::Function, analyser::metric::MetricResults>> result;

    // Набиваем итоговый вектор результатоми вычисления метрик для каждой функции

    for (const auto &function : functions_vector) {
        result.push_back({function, metric_extractor.Get(function)});
    }

    return result;
}
}  // namespace

auto SplitByClasses(const auto &analysis) {
    auto filtered_view = analysis | std::views::filter([](const auto &function_info) {
                             return function_info.first.class_name.has_value();
                         });
    // Итоговый список пар: имя класса - вектор пар (функция - метрики)
    std::unordered_map<std::string,
                       std::vector<std::pair<analyser::function::Function, analyser::metric::MetricResults>>>
        result;

    for (const auto &function_info : filtered_view) {
        // имя класса (ключ): вектор пар функция метрики (значение)
        result[function_info.first.class_name.value()].push_back(function_info);
    }

    return result;
}

auto SplitByFiles(const auto &analysis) {
    // Итоговый список пар: имя файла - вектор пар (функция - метрики)
    std::unordered_map<std::string,
                       std::vector<std::pair<analyser::function::Function, analyser::metric::MetricResults>>>
        result;
    for (const auto &function_info : analysis) {
        // имя файла (ключ): вектор пар функция метрики (значение)
        result[function_info.first.filename].push_back({function_info.first, function_info.second});
    }

    return result;
}

void AccumulateFunctionAnalysis(const auto &analysis,
                                const analyser::metric_accumulator::MetricsAccumulator &accumulator) {
    std::ranges::for_each(analysis,
                          [accumulator](const auto &elem) { accumulator.AccumulateNextFunctionResults(elem.second); });
}

}  // namespace analyser
