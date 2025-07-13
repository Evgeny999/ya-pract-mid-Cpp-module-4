#include "metric_impl/code_lines_count.hpp"
#include "metric_accumulator_impl/sum_average_accumulator.hpp"

#include <analyse.hpp>
#include <gtest/gtest.h>

namespace analyser::metric::metric_impl {

TEST(CodeLinesCount, SimpleCheck) {
    MetricExtractor metric_extractor;
    metric_extractor.RegisterMetric(std::make_unique<CodeLinesCountMetric>());

    std::vector<std::string> files = {"comments.py"};
    auto metrics = AnalyseFunctions(files, metric_extractor);

    metric_accumulator::metric_accumulator_impl::SumAverageAccumulator acc;

    for (const auto &function_info : metrics) {
        for (const auto &metric : function_info.second) {
            acc.Accumulate(metric);
        }
    }
    acc.Finalize();
    auto result = acc.Get();
    EXPECT_EQ(result.sum, 4);
    // одна функция, 4(число строк кода) / 1 = 2
    EXPECT_EQ(result.average, 4);
}

}  // namespace analyser::metric::metric_impl
