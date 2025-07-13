#include "metric_impl/cyclomatic_complexity.hpp"
#include "metric_accumulator_impl/sum_average_accumulator.hpp"

#include <gtest/gtest.h>

#include <analyse.hpp>

namespace analyser::metric::metric_impl {

TEST(CyclomaticComplexity, SimpleCheck) {
    MetricExtractor metric_extractor;
    metric_extractor.RegisterMetric(std::make_unique<CyclomaticComplexityMetric>());

    std::vector<std::string> files = {"if.py"};
    auto metrics = AnalyseFunctions(files, metric_extractor);

    metric_accumulator::metric_accumulator_impl::SumAverageAccumulator acc;

    for (const auto &function_info : metrics) {
        for (const auto &metric : function_info.second) {
            acc.Accumulate(metric);
        }
    }
    acc.Finalize();
    auto result = acc.Get();
    EXPECT_EQ(result.sum, 2);
    // одна функция, 2(цикломатическая сложность) / 1 = 2
    EXPECT_EQ(result.average, 2);
}

}  // namespace analyser::metric::metric_impl
