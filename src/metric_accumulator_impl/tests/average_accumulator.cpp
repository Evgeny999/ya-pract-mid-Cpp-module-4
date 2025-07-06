#include "metric_accumulator_impl/average_accumulator.hpp"

#include <gtest/gtest.h>

#include <cmath>

#include <analyse.hpp>

namespace analyser::metric_accumulator::metric_accumulator_impl::test {

TEST(AverageAccumulator, SimpleCheck) {
    AverageAccumulator averageAccumulator{};

    metric::MetricResult metricFirst, metricSecond;
    metricFirst.value = 3;
    metricSecond.value = 4;
    averageAccumulator.Accumulate(metricFirst);
    averageAccumulator.Accumulate(metricSecond);

    EXPECT_DOUBLE_EQ(averageAccumulator.Get(), 3.5);
}

}  // namespace analyser::metric_accumulator::metric_accumulator_impl::test
