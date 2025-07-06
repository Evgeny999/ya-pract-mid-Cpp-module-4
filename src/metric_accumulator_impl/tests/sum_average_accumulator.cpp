#include "metric_accumulator_impl/sum_average_accumulator.hpp"

#include <gtest/gtest.h>

#include <cmath>

namespace analyser::metric_accumulator::metric_accumulator_impl::test {

TEST(SumAverageAccumulator, SimpleCheck) {
    SumAverageAccumulator sumAverageAccumulator;

    metric::MetricResult metricFirst, metricSecond;
    metricFirst.value = 3;
    metricSecond.value = 4;
    sumAverageAccumulator.Accumulate(metricFirst);
    sumAverageAccumulator.Accumulate(metricSecond);

    auto result = sumAverageAccumulator.Get();
    EXPECT_DOUBLE_EQ(result.average, 3.5);
    EXPECT_DOUBLE_EQ(result.sum, 7);
}

}  // namespace analyser::metric_accumulator::metric_accumulator_impl::test
