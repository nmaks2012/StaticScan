#pragma once

#include <array>
#include <expected>
#include <utility>

#include "types.hpp"

namespace stdx::details {

// Шаблонный класс для хранения форматирующей строчки и ее особенностей
template <fixed_string str> class format_string {

public:
  static constexpr auto fmt = str;
  static consteval std::expected<size_t, parse_error> get_number_placeholders();

  static constexpr size_t number_placeholders =
      get_number_placeholders().value();

  using PlaceholdersContainer =
      std::array<std::pair<size_t, size_t>, number_placeholders>;

  static consteval PlaceholdersContainer get_placeholder_positions();

  static constexpr PlaceholdersContainer placeholder_positions =
      get_placeholder_positions();
};

template <fixed_string str>
consteval std::expected<size_t, parse_error>
format_string<str>::get_number_placeholders() {
  constexpr size_t N = str.size();
  if (!N)
    return 0;
  size_t placeholder_count = 0;
  size_t pos = 0;
  const size_t size = N - 1; // -1 для игнорирования нуль-терминатора

  while (pos < size) {
    // Пропускаем все символы до '{'
    if (str.data[pos] != '{') {
      ++pos;
      continue;
    }

    // Проверяем незакрытый плейсхолдер
    if (pos + 1 >= size) {
      return std::unexpected(parse_error{"Unclosed last placeholder"});
    }

    // Начало плейсхолдера
    ++placeholder_count;
    ++pos;

    // Проверка спецификатора формата
    if (str.data[pos] == '%') {
      ++pos;
      if (pos >= size) {
        return std::unexpected(parse_error{"Unclosed last placeholder"});
      }

      // Проверяем допустимые спецификаторы
      const char spec = str.data[pos];
      constexpr char valid_specs[] = {'d', 'u', 's'};
      bool valid = false;

      for (const char s : valid_specs) {
        if (spec == s) {
          valid = true;
          break;
        }
      }

      if (!valid) {
        return std::unexpected(parse_error{"Invalid specifier."});
      }
      ++pos;
    }

    // Проверяем закрывающую скобку
    if (pos >= size || str.data[pos] != '}') {
      return std::unexpected(
          parse_error{"\'}\' hasn't been found in appropriate place"});
    }
    ++pos;
  }

  return placeholder_count;
}

template <fixed_string str>
consteval format_string<str>::PlaceholdersContainer
format_string<str>::get_placeholder_positions() {

  PlaceholdersContainer result;

  // str.data провалидирована в get_number_placeholders
  // поэтому можем пройтись и добавить позиции плейсхолдеров без доп.проверок.
  for (size_t i = 0, pos = 0; i < str.data.size(); ++i) {
    if (str.data[i] == '{') {
      result[pos].first = i;
    }
    if (str.data[i] == '}') {
      result[pos].second = i;
      ++pos;
    }
  }

  return result;
};

// Пользовательский литерал
template <fixed_string str> consteval auto operator"" _fs() {
  return format_string<str>();
}

} // namespace stdx::details
