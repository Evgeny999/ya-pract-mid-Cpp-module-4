#include "metric_impl/parameters_count.hpp"

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
#include <variant>
#include <vector>

namespace analyser::metric::metric_impl {

MetricResult::ValueType CountParametersMetric::CalculateImpl(const function::Function &f) const {
    size_t start = 0;
    const std::string marker = "(parameters";
    const std::string &ast = f.ast;
    size_t end = 0;

    start = ast.find(marker, start);
    // Выделяем ту часть ast функции, которая касается параметров
    size_t open_braces = 1;
    end = start + marker.length();

    while (end < ast.size() && open_braces > 0) {
        if (ast[end] == '(')
            open_braces++;
        else if (ast[end] == ')')
            open_braces--;
        end++;
    }

    auto params_ast = ast.substr(start, end - start);
    // std::print("ast = {}", params_ast);
    //  для подсчёта параметров считаем количество строк с "identifier" и
    //  "(typed_default_parameter" в начале (после некоторо поличества пробелов)
    const std::array<std::string, 3> targets = {
        std::string{"(identifier"},               // 10 chars
        std::string{"(typed_default_parameter"},  // 21 chars
        std::string{"(default_parameter"}         // 21 chars
    };
    return std::ranges::count_if(params_ast | std::views::split('\n'), [targets](auto &&line) {
        // Find first non-space character
        auto first_char = std::ranges::find_if_not(line, ::isspace);

        // Check if remaining chars start with "identifier"
        return std::ranges::any_of(targets, [&](std::string_view target) {
            return std::ranges::equal(std::ranges::subrange(first_char, std::next(first_char, target.size())), target);
        });
    });
}

std::string CountParametersMetric::Name() const { return "CountParametersMetric"; }

}  // namespace analyser::metric::metric_impl
