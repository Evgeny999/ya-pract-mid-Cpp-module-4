#include "metric_impl/code_lines_count.hpp"

#include <analyse.hpp>
#include <gtest/gtest.h>

namespace analyser::metric::metric_impl {

TEST(CodeLinesCount, SimpleCheck) {
    MetricExtractor metric_extractor;
    metric_extractor.RegisterMetric(std::make_unique<CodeLinesCountMetric>());

    std::vector<std::string> files = {"comments.py"};
    auto metrics = AnalyseFunctions(files, metric_extractor);
    for (const auto &function_info : metrics) {
        for (const auto &metric : function_info.second) {
            /*std::println("function name = {} metric name = {} metric value = {}",
                         function_info.first.name, metric.metric_name,
               metric.value);*/
            EXPECT_STREQ(metric.metric_name.c_str(), "CodeLinesCountMetric");
            EXPECT_EQ(metric.value, 4);
        }
    }
}

}  // namespace analyser::metric::metric_impl
