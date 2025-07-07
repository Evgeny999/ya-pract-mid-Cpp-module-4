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
      std::make_unique<analyser::metric::metric_impl::CodeLinesCountMetric>());
  metric_extractor.RegisterMetric(
      std::make_unique<analyser::metric::metric_impl::CountParametersMetric>());
  metric_extractor.RegisterMetric(
      std::make_unique<
          analyser::metric::metric_impl::CyclomaticComplexityMetric>());

  // запустите analyser::AnalyseFunctions
  auto resultAnalyse =
      analyser::AnalyseFunctions(options.GetFiles(), metric_extractor);
  std::ranges::for_each(resultAnalyse, [](const auto &functionInfo) {
    std::println("{}", functionInfo.first.filename +
                           (functionInfo.first.class_name.has_value()
                                ? "::" + functionInfo.first.class_name.value()
                                : "") +
                           functionInfo.first.name);
    std::ranges::for_each(functionInfo.second, [](const auto &metric) {
      std::println("{}",
                   metric.metric_name + ':' + std::to_string(metric.value));
    });
  });

  analyser::metric_accumulator::MetricsAccumulator accumulator;
  // зарегистрируйте аккумуляторы метрик в accumulator
  // AverageAccumulator averageAccumulator{};
  accumulator.RegisterAccumulator(
      "CodeLinesCountMetric",
      std::make_unique<analyser::metric_accumulator::metric_accumulator_impl::
                           AverageAccumulator>());
  accumulator.RegisterAccumulator(
      "CodeLinesCountMetric",
      std::make_unique<analyser::metric_accumulator::metric_accumulator_impl::
                           SumAverageAccumulator>());
  accumulator.RegisterAccumulator(
      "CyclomaticComplexityMetric",
      std::make_unique<analyser::metric_accumulator::metric_accumulator_impl::
                           AverageAccumulator>());
  accumulator.RegisterAccumulator(
      "CyclomaticComplexityMetric",
      std::make_unique<analyser::metric_accumulator::metric_accumulator_impl::
                           SumAverageAccumulator>());
  accumulator.RegisterAccumulator(
      "CountParametersMetric",
      std::make_unique<analyser::metric_accumulator::metric_accumulator_impl::
                           AverageAccumulator>());
  accumulator.RegisterAccumulator(
      "CountParametersMetric",
      std::make_unique<analyser::metric_accumulator::metric_accumulator_impl::
                           SumAverageAccumulator>());

  // запустите analyser::SplitByFiles
  auto resultSplitByFiles = analyser::SplitByFiles(resultAnalyse);
  // запустите analyser::AccumulateFunctionAnalysis для каждого подмножества

  std::ranges::for_each(resultSplitByFiles, [&accumulator](
                                                const auto &file_info) {
    analyser::AccumulateFunctionAnalysis(file_info.second, accumulator);
    std::print("Accumulated Analysis for file {}", file_info.first);
    auto averageAccumulator = accumulator.GetFinalizedAccumulator<
        analyser::metric_accumulator::metric_accumulator_impl::
            AverageAccumulator>("CyclomaticComplexityMetric");
    auto sumAverageAccumulator = accumulator.GetFinalizedAccumulator<
        analyser::metric_accumulator::metric_accumulator_impl::
            SumAverageAccumulator>("CountParametersMetric");
    std::print("aggregated_averageAccumulator: {}", averageAccumulator.Get());
    auto sumAverage = sumAverageAccumulator.Get();
    std::print("aggregated_averageAccumulator: sum = {} average = {}",
               sumAverage.sum, sumAverage.average);
    accumulator.ResetAccumulators();
  });
  // результатов метрик выведете результаты на консоль

  // запустите analyser::SplitByClasses
  auto resultSplitByClasses = analyser::SplitByClasses(resultAnalyse);
  // запустите analyser::AccumulateFunctionAnalysis для каждого подмножества
  std::ranges::for_each(resultSplitByClasses, [&accumulator](
                                                  const auto &class_info) {
    analyser::AccumulateFunctionAnalysis(class_info.second, accumulator);
    std::print("Accumulated Analysis for file {}", class_info.first);
    auto averageAccumulator = accumulator.GetFinalizedAccumulator<
        analyser::metric_accumulator::metric_accumulator_impl::
            AverageAccumulator>("CyclomaticComplexityMetric");
    auto sumAverageAccumulator = accumulator.GetFinalizedAccumulator<
        analyser::metric_accumulator::metric_accumulator_impl::
            SumAverageAccumulator>("CountParametersMetric");
    std::print("aggregated_averageAccumulator: {}", averageAccumulator.Get());
    auto sumAverage = sumAverageAccumulator.Get();
    std::print("aggregated_averageAccumulator: sum = {} average = {}",
               sumAverage.sum, sumAverage.average);
    accumulator.ResetAccumulators();
  });
  // результатов метрик выведете результаты на консоль

  // запустите analyser::AccumulateFunctionAnalysis для всех результатов
  // метрик выведете результаты на консоль

  return 0;
}
