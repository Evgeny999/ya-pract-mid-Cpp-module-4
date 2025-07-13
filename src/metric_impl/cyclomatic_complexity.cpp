#include "metric_impl/cyclomatic_complexity.hpp"

#include <unistd.h>

#include <algorithm>
#include <array>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <functional>
#include <iostream>
#include <ranges>
#include <sstream>
#include <string>
#include <unordered_set>
#include <variant>
#include <vector>

namespace analyser::metric::metric_impl {

MetricResult::ValueType CyclomaticComplexityMetric::CalculateImpl(const function::Function &f) const {
    /*цикломатическая сложность для функции =
    число операторов принятия решений + 1*/
    std::unordered_set<std::string> node_types = {"if_statement",  "if_statement",     "while_statement",
                                                  "for_statement", "switch_statement", "case_statement",
                                                  "catch_clause"};

    return FindNumExpressions(f, node_types) + 1;
}

std::string CyclomaticComplexityMetric::Name() const { return "CyclomaticComplexityMetric"; }

}  // namespace analyser::metric::metric_impl
