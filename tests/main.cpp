#include "format_string.hpp"
#include "parse.hpp"
#include "scan.hpp"
#include "types.hpp"
#include <array>
#include <bits/ranges_algo.h>
#include <cstddef>
#include <string_view>
#include <sys/types.h>
#include <utility>

int main() {

  using namespace stdx::details;

  // ################ fixed_string ##################################
  {
    // Фиксированная строка заданного размера
    constexpr auto src = fixed_string{"i = 10"};
    static_assert(src.size() == 7,
                  "Invalid size fixed string, created on fixed size");
  }
  {
    // Фиксированная строка с меньшим размером
    constexpr fixed_string<10> src{"123"};
    static_assert(src.size() == 4,
                  "Invalid size fixed_string, for smaller size array");
  }
  {
    // Фиксированная строка от двух указателей
    constexpr const std::array<const char, 10> str = {"abcdefg12"};
    constexpr auto src = fixed_string<str.size()>{str.begin(), str.end()};
    static_assert(src.size() == str.size(),
                  "Invalid size fixed_string, createt from 2 pointers");
  }
  // ################ fixed_string ##################################

  // ################ format_string #################################
  {
    // Проверка количества и позиций плейсхолдеров
    constexpr auto fmt = "{}123{}%d{}"_fs;
    static_assert(fmt.number_placeholders == 3,
                  "Invalid placeholder counts, expected 3");

    constexpr std::array<std::pair<size_t, size_t>, 3> expect_arr = {
        {{0, 1}, {5, 6}, {9, 10}}};

    static_assert(std::ranges::equal(fmt.placeholder_positions, expect_arr),
                  "");
  }
  // ################ format_string #################################

  // ################ parse_input #################################
  {
    // {%d} - Целое число
    constexpr auto src = fixed_string("d - -10");
    constexpr auto fmt = "d - {%d}"_fs;
    constexpr auto d = parse_input<0, fmt, src, int>();
    static_assert(d == -10, "Expected int - 10");
  }
  {
    // {%u} - Натуральное число
    constexpr auto src = fixed_string("u - 10");
    constexpr auto fmt = "u - {%u}"_fs;
    constexpr auto u = parse_input<0, fmt, src, size_t>();
    static_assert(u == 10, "Expected size_t - 10");
  }
  {
    // {} - Строка
    constexpr auto src = fixed_string("s - abc");
    constexpr auto fmt = "s - {}"_fs;
    constexpr auto s = parse_input<0, fmt, src, std::string_view>();
    static_assert(s == "abc"sv, "Expected string - abc");
  }
  // ################ parse_input #################################

  // ################ scan #################################
  {
    // {%d} - Целое число а также мин и макс значение для x32
    constexpr auto fmt = "{%d} zeros ahead {%d} min {%d} max {%d}"_fs;
    constexpr auto src = fixed_string(
        "-10 zeros ahead -00001000 min -2147483648 max 2147483647");
    constexpr auto res = stdx::scan<fmt, src, int, int, int, int>();
    static_assert(res.value<0>() == -10, "Invalid result. Expected int -10");
    static_assert(res.value<1>() == -1000,
                  "Invalid result. Expected int -1000");
    static_assert(res.value<2>() == -2147483648,
                  "Invalid result. Expected int -2147483648");
    static_assert(res.value<3>() == 2147483647,
                  "Invalid result. Expected int 2147483647");
  }
  {
    // {%u} - Натуральное число и max для x32
    constexpr auto fmt = "simple test {%u} max {%u}"_fs;
    constexpr auto src = fixed_string("simple test 10 max 4294967295");
    constexpr auto res = stdx::scan<fmt, src, size_t, size_t>();
    static_assert(res.value<0>() == 10, "Invalid result. Expected size_t 10");
    static_assert(res.value<1>() == 4294967295,
                  "Invalid result. Expected size_t 4294967295");
  }
  {
    // {}{%s} - Строка
    constexpr auto fmt = "{} {%s}"_fs;
    constexpr auto src = fixed_string("qwerty smile");
    constexpr auto res =
        stdx::scan<fmt, src, std::string_view, std::string_view>();
    static_assert(res.value<0>() == "qwerty"sv,
                  "Invalid result. Expected std::string_view qwerty");
    static_assert(res.value<1>() == "smile"sv,
                  "Invalid result. Expected std::string_view smile");
  }
  {
    // {} - Пустая строка
    constexpr auto fmt = "{}"_fs;
    constexpr auto src = fixed_string("");
    constexpr auto res = stdx::scan<fmt, src, std::string_view>();
    static_assert(res.value<0>() == ""sv,
                  "Invalid result. Expected empty std::string_view");
  }
  {
    // Разное количество аргументов
    constexpr auto fmt = "{%d} {%d}"_fs;
    constexpr auto src = fixed_string("-999");
    constexpr auto res = stdx::scan<fmt, src, int>();
    static_assert(res.value<0>() == -999,
                  "Invalid result. Expected empty int -999");
  }
  {
    // Все спецификаторы вместе
    constexpr auto fmt = "sss {%s} jj {} dd {%d} uuuu {%u}"_fs;
    constexpr auto src = fixed_string("sss qwerty jj Hello dd -100 uuuu 999");
    constexpr auto res =
        stdx::scan<fmt, src, std::string_view, std::string_view, int, size_t>();
    static_assert(res.value<0>() == "qwerty"sv,
                  "Invalid result. Expected std::string_view - qwerty");
    static_assert(res.value<1>() == "Hello"sv,
                  "Invalid result. Expected std::string_view - Hello");
    static_assert(res.value<2>() == -100, "Invalid result. Expected int - 100");
    static_assert(res.value<3>() == 999, "Invalid result. Expected size_t 999");
  }

  {
    // Проверка различных вариаций типов натуральных и целых чисел
    constexpr auto fmt = "{%d} {%d} {%d} {%d} {%u} {%u} {%u} {%u}"_fs;
    constexpr auto src = fixed_string("-99 -888 -777 -666 111 222 333 444");
    constexpr auto res = stdx::scan<fmt, src, int8_t, int16_t, int32_t, int64_t,
                                    u_int8_t, u_int16_t, u_int32_t, u_int64_t>();
    static_assert(res.value<0>() == -99,
                  "Invalid result. Expected empty int -99");
    static_assert(res.value<1>() == -888,
                  "Invalid result. Expected empty int -888");
    static_assert(res.value<2>() == -777,
                  "Invalid result. Expected empty int -777");
    static_assert(res.value<3>() == -666,
                  "Invalid result. Expected empty int -666");
    static_assert(res.value<4>() == 111,
                  "Invalid result. Expected empty size_t 111");
    static_assert(res.value<5>() == 222,
                  "Invalid result. Expected empty size_t 222");
    static_assert(res.value<6>() == 333,
                  "Invalid result. Expected empty size_t 333");
    static_assert(res.value<7>() == 444,
                  "Invalid result. Expected empty size_t 444");
  }

  // Ошибочные сценарии
  //   {
  //     // {%u} - неверное натуральное число
  //     constexpr auto fmt = "simple test {%u} max {%u}"_fs;
  //     constexpr auto src = fixed_string("simple test -10 max -4294967295");
  //     constexpr auto res = stdx::scan<fmt, src, size_t, size_t>();
  //     static_assert(res.value<0>() == 10, "Invalid result. Expected size_t
  //     10"); static_assert(res.value<1>() == 4294967295,
  //                   "Invalid result. Expected size_t 4294967295");
  //   }
  //   {
  //     // {%s} - числа вместо строки
  //     constexpr auto fmt = "{%s}"_fs;
  //     constexpr auto src = fixed_string("111");
  //     constexpr auto res = stdx::scan<fmt, src, std::string_view>();
  //     static_assert(res.value<0>() == 111, "Invalid result.");
  //   }
  //   {
  //     // несовпадающие аргументы
  //     constexpr auto fmt = "{%s} {%d} {%u}"_fs;
  //     constexpr auto src = fixed_string("111 -- adfdf");
  //     constexpr auto res = stdx::scan<fmt, src, std::string_view, int,
  //     size_t>(); static_assert(res.value<0>() == 111, "Invalid result.");
  //     static_assert(res.value<1>() == 00, "Invalid result.");
  //     static_assert(res.value<2>() == 00, "Invalid result.");
  //   }
    //  {
    //   // неподдерживаемый тип
    //   constexpr auto fmt = "{%s}"_fs;
    //   constexpr auto src = fixed_string("sss");
    //   constexpr auto res = stdx::scan<fmt, src, std::string>();
    //   static_assert(res.value<0>() == "sss"sv, "Invalid result.");
    // }

  // ################ scan #################################
}
