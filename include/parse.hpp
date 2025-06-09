#pragma once

#include "format_string.hpp"
#include "types.hpp"
#include <charconv>
#include <concepts>
#include <string_view>
#include <sys/types.h>
#include <type_traits>

namespace stdx::details {

using namespace std::string_view_literals;

// Шаблонная функция, возвращающая пару позиций в строке с исходными данными,
// соотвествующих I-ому плейсхолдеру
template <int I, format_string fmt, fixed_string source>
consteval auto get_current_source_for_parsing() {
  static_assert(I >= 0 && I < fmt.number_placeholders,
                "Invalid placeholder index");

  constexpr auto to_sv = [](const auto &fs) {
    return std::string_view(fs.data.begin(), fs.size() - 1);
  };

  constexpr auto fmt_sv = to_sv(fmt.fmt);
  constexpr auto src_sv = to_sv(source);
  constexpr auto &positions = fmt.placeholder_positions;

  // Получаем границы текущего плейсхолдера в формате
  constexpr auto pos_i = positions[I];
  constexpr size_t fmt_start = pos_i.first, fmt_end = pos_i.second;

  // Находим начало в исходной строке
  constexpr auto src_start = [&] {
    if constexpr (I == 0) {
      return fmt_start;
    } else {
      // Находим конец предыдущего плейсхолдера в исходной строке
      constexpr auto prev_bounds =
          get_current_source_for_parsing<I - 1, fmt, source>();
      const auto prev_end = prev_bounds.second;

      // Получаем разделитель между текущим и предыдущим плейсхолдерами
      constexpr auto prev_fmt_end = positions[I - 1].second;
      constexpr auto sep =
          fmt_sv.substr(prev_fmt_end + 1, fmt_start - (prev_fmt_end + 1));

      // Ищем разделитель после предыдущего значения
      auto pos = src_sv.find(sep, prev_end);
      return pos != std::string_view::npos ? pos + sep.size() : src_sv.size();
    }
  }();

  // Находим конец в исходной строке
  constexpr auto src_end = [&] {
    // Получаем разделитель после текущего плейсхолдера
    if constexpr (fmt_end == (fmt_sv.size() - 1)) {
      return src_sv.size();
    }
    constexpr auto sep =
        fmt_sv.substr(fmt_end + 1, (I < fmt.number_placeholders - 1)
                                       ? positions[I + 1].first - (fmt_end + 1)
                                       : fmt_sv.size() - (fmt_end + 1));
    // Ищем разделитель после текущего значения
    constexpr auto pos = src_sv.find(sep, src_start);
    return pos != std::string_view::npos ? pos : src_sv.size();
  }();
  return std::pair{src_start, src_end};
}

// Реализуйте семейство функция parse_value
template <typename T>
concept Type = !std::is_reference<T>() && (std::is_same<T, std::string_view>() || std::signed_integral<T> ||
               std::unsigned_integral<T>);

template <typename Type> consteval Type parse_value(std::string_view str) {
  Type value;
  std::from_chars(str.begin(), str.end(), value);
  return value;
}

// Шаблонная функция, выполняющая преобразования исходных данных в конкретный
// тип на основе I-го плейсхолдера

template <int idx, format_string fmt, fixed_string src, typename Type>
consteval Type parse_input() {

  constexpr auto src_positions =
      get_current_source_for_parsing<idx, fmt, src>();

  // Извлекаем значение из исходной строки
  constexpr std::string_view value{src.data.begin() + src_positions.first,
                                   src.data.begin() + src_positions.second};

  // Получаем спецификатор для преобразования
  constexpr std::string_view spec{
      fmt.fmt.data.begin() + fmt.placeholder_positions[idx].first + 1,
      fmt.fmt.data.begin() + fmt.placeholder_positions[idx].second};

  static_assert(spec.empty() || spec == "%d"sv || spec == "%u"sv || spec == "%s"sv, "Invalid the specifier type. Expected - {%d}, {%u}, {%s} or {}");

  // Преобразуем в заданное значение
  if constexpr (spec == "%d"sv) {
    return parse_value<ssize_t>(value);
  } else if constexpr (spec == "%u"sv) {
    return parse_value<size_t>(value);
  } else if constexpr (spec.empty() || spec == "%s"sv) {
    return value;
  }
}

} // namespace stdx::details
