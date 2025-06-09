#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <iterator>
#include <tuple>

namespace stdx::details {

// Шаблонный класс, хранящий C-style строку фиксированной длины
template <std::size_t N> struct fixed_string {

  std::size_t str_size = N;
  std::array<char, N> data{};

  constexpr fixed_string(const char (&str)[N]) {
    std::copy_n(str, N, data.data());
  }

  template <std::size_t M>
    requires(M <= N)
  constexpr fixed_string(const char (&str)[M]) {
    str_size = M;
    std::copy_n(str, M, data.data());
  }

  constexpr fixed_string(const char *first, const char *last)
    requires(requires { std::distance(first, last) == N; })
  {
    std::copy_n(first, std::distance(first, last), data.begin());
  }

  constexpr size_t size() const { return str_size; }
};

// Шаблонный класс, хранящий fixed_string достаточной длины для хранения ошибки
// парсинга

// ваш код здесь
struct parse_error : public fixed_string<100> {};

// Шаблонный класс для хранения результатов парсинга

template <typename... Type> class scan_result {

public:
  consteval scan_result(std::tuple<Type...> &&result) : result_(result) {}

  template <size_t idx> consteval auto &value() const {
    return std::get<idx>(result_);
  }

private:
  std::tuple<Type...> result_;
};

} // namespace stdx::details
