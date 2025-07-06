#include "metric_impl/cyclomatic_complexity.hpp"

#include <gtest/gtest.h>

#include <analyse.hpp>

namespace analyser::metric::metric_impl {

TEST(CyclomaticComplexity, SimpleCheck) {
    MetricExtractor metric_extractor;
    metric_extractor.RegisterMetric(std::make_unique<CyclomaticComplexityMetric>());

    std::vector<std::string> files = {"if.py"};
    auto metrics = AnalyseFunctions(files, metric_extractor);

    for (const auto &function_info : metrics) {
        for (const auto &metric : function_info.second) {
            /*std::println("function name = {} metric name = {} metric value = {}",
                         function_info.first.name, metric.metric_name,
               metric.value);*/
            EXPECT_STREQ(metric.metric_name.c_str(), "CyclomaticComplexityMetric");
            EXPECT_EQ(metric.value, 2);
        }
    }
}

}  // namespace analyser::metric::metric_impl
