#include <unistd.h>

#include <algorithm>
#include <array>
#include <boost/stacktrace.hpp>
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
#include <stacktrace>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

#include "analyse.hpp"
#include "cmd_options.hpp"
#include "file.hpp"
#include "function.hpp"
#include "metric.hpp"
#include "metric_accumulator.hpp"
#include "metric_accumulator_impl/accumulators.hpp"
#include "metric_impl/metrics.hpp"

namespace metric_impl = analyser::metric::metric_impl;
namespace metric_acc_impl =
    analyser::metric_accumulator::metric_accumulator_impl;
// для захвата лямбдой (для классов и файлов сообщение отличается)
enum class FileOrClass { FILE, CLASS };

int main(int argc, char *argv[]) {

  analyser::cmd::ProgramOptions options;
  // распарсите входные параметры
  if (!options.Parse(argc, argv)) {
    std::print("Failed to parse input parameters");
    return false;
  }

  analyser::metric::MetricExtractor metric_extractor;
  // зарегистрируйте метрики в metric_extractor
  metric_extractor.RegisterMetric(
      std::make_unique<metric_impl::CodeLinesCountMetric>());
  metric_extractor.RegisterMetric(
      std::make_unique<metric_impl::CountParametersMetric>());
  metric_extractor.RegisterMetric(
      std::make_unique<metric_impl::CyclomaticComplexityMetric>());

  // запустите analyser::AnalyseFunctions
  auto resultAnalyse =
      analyser::AnalyseFunctions(options.GetFiles(), metric_extractor);

  analyser::metric_accumulator::MetricsAccumulator accumulator;
  // зарегистрируйте аккумуляторы метрик в accumulator
  // AverageAccumulator averageAccumulator{};
  accumulator.RegisterAccumulator(
      "CodeLinesCountMetric",
      std::make_unique<metric_acc_impl::AverageAccumulator>());
  accumulator.RegisterAccumulator(
      "CyclomaticComplexityMetric",
      std::make_unique<metric_acc_impl::SumAverageAccumulator>());
  accumulator.RegisterAccumulator(
      "CountParametersMetric",
      std::make_unique<metric_acc_impl::AverageAccumulator>());

  FileOrClass flag;
  auto accumulate_and_print = [&accumulator, &flag](const auto &info) {
    analyser::AccumulateFunctionAnalysis(info.second, accumulator);
    if (flag == FileOrClass::FILE) {
      std::print("Accumulated Analysis for file {}", info.first);
    } else if (flag == FileOrClass::CLASS) {
      std::print("Accumulated Analysis for class {}", info.first);
    }

    auto averageAccumulator =
        accumulator
            .GetFinalizedAccumulator<metric_acc_impl::AverageAccumulator>(
                "CyclomaticComplexityMetric");
    auto sumAverageAccumulator =
        accumulator
            .GetFinalizedAccumulator<metric_acc_impl::SumAverageAccumulator>(
                "CountParametersMetric");
    std::print("aggregated_averageAccumulator: {}", averageAccumulator.Get());
    auto sumAverage = sumAverageAccumulator.Get();
    std::print("aggregated_averageAccumulator: sum = {} average = {}",
               sumAverage.sum, sumAverage.average);
    accumulator.ResetAccumulators();
  };

  flag = FileOrClass::FILE;
  // запустите analyser::SplitByFiles
  auto resultSplitByFiles = analyser::SplitByFiles(resultAnalyse);
  // запустите analyser::AccumulateFunctionAnalysis для каждого подмножества

  // Ок, можно chunks возвращать, но тогда придётся вот этот код использовать,
  // чтобы в мапу собрать
  // Convert chunks to unordered_map
  auto convert_to_map = [](auto &&chunks) {
    std::unordered_map<std::string,
                       std::vector<std::pair<analyser::function::Function,
                                             analyser::metric::MetricResults>>>
        map;

    for (const auto &chunk : chunks) {
      if (!chunk.empty()) {
        // Get class name from first element in chunk
        const std::string class_name = chunk.front().first.class_name.value();

        // Insert all elements of this chunk into the map
        for (const auto &elem : chunk) {
          map[class_name].push_back(elem);
        }
      }
    }
    return map;
  };

  std::ranges::for_each(convert_to_map(resultSplitByFiles),
                        accumulate_and_print);
  // результатов метрик выведете результаты на консоль

  flag = FileOrClass::CLASS;
  // запустите analyser::SplitByClasses
  auto resultSplitByClasses = analyser::SplitByClasses(resultAnalyse);
  // Ок, можно chunks возвращать, но тогда придётся вот этот код использовать,
  // чтобы в мапу собрать
  // Convert chunks to unordered_map

  // запустите analyser::AccumulateFunctionAnalysis для каждого подмножества
  std::ranges::for_each(convert_to_map(resultSplitByClasses),
                        accumulate_and_print);

  // результатов метрик выведете результаты на консоль

  // запустите analyser::AccumulateFunctionAnalysis для всех результатов
  // метрик выведете результаты на консоль

  return 0;
}
