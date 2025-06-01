#pragma once

#include <tuple>
#include <utility>

#include "format_string.hpp"
#include "parse.hpp"
#include "types.hpp"

namespace stdx {

using namespace stdx::details;

// Главная функция
template <details::format_string fmt, details::fixed_string source,
          typename... Type>
consteval details::scan_result<Type...> scan() {

  auto scan_items = [&]<size_t... Is>(std::index_sequence<Is...>) consteval {
    return std::tuple{parse_input<Is, fmt, source, Type>()...};
  };

  return details::scan_result<Type...>{
      scan_items(std::index_sequence_for<Type...>())};
}

} // namespace stdx