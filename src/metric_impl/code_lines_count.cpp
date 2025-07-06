#include "metric_impl/code_lines_count.hpp"

#include <unistd.h>

#include <algorithm>
#include <array>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
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

MetricResult::ValueType CodeLinesCountMetric::CalculateImpl(const function::Function &f) const {
    /*Посчитаем число строк в функции (без учёта комментариев и gecns[ cnhjr])
     * следующим образом: выпишем все значащие экспрессии и будем проверять их
     * наличие в очередной строке AST.*/
    std::unordered_set<std::string> node_types = {
        "function_definition", "expression_statement", "return_statement",   "declaration",      "if_statement",
        "for_statement",       "while_statement",      "compound_statement", "switch_statement", "jump_statement"};

    return FindNumExpressions(f, node_types);
}

std::string CodeLinesCountMetric::Name() const { return "CodeLinesCountMetric"; }

}  // namespace analyser::metric::metric_impl
