#include "metric_impl/parameters_count.hpp"

#include <gtest/gtest.h>

#include <analyse.hpp>

namespace analyser::metric::metric_impl {

TEST(CountParameters, SimpleCheck) {
    MetricExtractor metric_extractor;
    metric_extractor.RegisterMetric(std::make_unique<CountParametersMetric>());

    std::vector<std::string> files = {"many_parameters.py"};
    auto metrics = AnalyseFunctions(files, metric_extractor);

    for (const auto &function_info : metrics) {
        for (const auto &metric : function_info.second) {
            /*std::println("function name = {} metric name = {} metric value = {}",
                         function_info.first.name, metric.metric_name,
               metric.value);*/
            EXPECT_STREQ(metric.metric_name.c_str(), "CountParametersMetric");
            EXPECT_EQ(metric.value, 5);
        }
    }
}
}  // namespace analyser::metric::metric_impl
