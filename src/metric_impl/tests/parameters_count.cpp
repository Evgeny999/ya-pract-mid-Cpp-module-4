#include "metric_impl/parameters_count.hpp"
#include "metric_accumulator_impl/sum_average_accumulator.hpp"

#include <gtest/gtest.h>

#include <analyse.hpp>

namespace analyser::metric::metric_impl {

TEST(CountParameters, SimpleCheck) {
    MetricExtractor metric_extractor;
    metric_extractor.RegisterMetric(std::make_unique<CountParametersMetric>());

    std::vector<std::string> files = {"many_parameters.py"};
    auto metrics = AnalyseFunctions(files, metric_extractor);

    metric_accumulator::metric_accumulator_impl::SumAverageAccumulator acc;

    for (const auto &function_info : metrics) {
        for (const auto &metric : function_info.second) {
            acc.Accumulate(metric);
        }
    }
    acc.Finalize();
    auto result = acc.Get();
    EXPECT_EQ(result.sum, 5);
    // одна функция, 5(число параметров) / 1 = 2
    EXPECT_EQ(result.average, 5);
}
}  // namespace analyser::metric::metric_impl
