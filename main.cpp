#include "scan.hpp"
#include <string_view>
#include <print>
#include <cstddef>

int main() {

    using namespace stdx::details;

    // Пример 1: Разбор строки, предоставленной пользователем (из вашего примера)
    // Определяем строку формата и исходную строку во время компиляции
    constexpr auto fmt = "Пользователь: {}, ID: {%u}, Очки: {%d}"_fs;
    constexpr auto src = fixed_string{"Пользователь: Алиса, ID: 101, Очки: -50"};

    // Выполняем операцию сканирования
    constexpr auto result = stdx::scan<fmt, src, std::string_view, size_t, int>();

    // Получаем доступ к разобранным значениям и проверяем их во время компиляции
    static_assert(result.value<0>() == "Алиса");
    static_assert(result.value<1>() == 101);
    static_assert(result.value<2>() == -50);

    // Выводим результат в консоль
    const auto user_name = result.value<0>();
    const auto user_id = result.value<1>();
    const auto user_score = result.value<2>();
    std::println("--- Пример 1: Пользовательские данные ---");
    std::println("Имя: {}", user_name);
    std::println("ID: {}", user_id);
    std::println("Очки: {}\n", user_score);

    // Пример 2: Разбор строки версии
    constexpr auto version_fmt = "v{%u}.{%u}.{%u}-{}"_fs;
    constexpr auto version_src = fixed_string{"v1.2.3-beta"};
    constexpr auto version_result = stdx::scan<version_fmt, version_src, unsigned, unsigned, unsigned, std::string_view>();
    static_assert(version_result.value<0>() == 1, "Major version mismatch");
    static_assert(version_result.value<1>() == 2, "Minor version mismatch");
    static_assert(version_result.value<2>() == 3, "Patch version mismatch");
    static_assert(version_result.value<3>() == "beta", "Tag mismatch");

    // Выводим результат в консоль
    const auto major = version_result.value<0>();
    const auto minor = version_result.value<1>();
    const auto patch = version_result.value<2>();
    const auto tag = version_result.value<3>();
    std::println("--- Пример 2: Версия ---");
    std::println("Major: {}", major);
    std::println("Minor: {}", minor);
    std::println("Patch: {}", patch);
    std::println("Tag: {}\n", tag);

    // Пример 3: Разбор геометрических данных
    constexpr auto point_fmt = "Point({%d}, {%d})"_fs;
    constexpr auto point_src = fixed_string{"Point(15, -42)"};
    constexpr auto point_result = stdx::scan<point_fmt, point_src, int, int>();
    static_assert(point_result.value<0>() == 15, "X coordinate mismatch");
    static_assert(point_result.value<1>() == -42, "Y coordinate mismatch");

    // Выводим результат в консоль
    const auto x = point_result.value<0>();
    const auto y = point_result.value<1>();
    std::println("--- Пример 3: Координаты точки ---");
    std::println("X: {}", x);
    std::println("Y: {}\n", y);

    // Пример 4: Извлечение данных из строки лога
    constexpr auto log_fmt = "[ERROR {%u}]: {}"_fs;
    constexpr auto log_src = fixed_string{"[ERROR 404]: Not Found"};
    constexpr auto log_result = stdx::scan<log_fmt, log_src, unsigned, std::string_view>();
    static_assert(log_result.value<0>() == 404, "Error code mismatch");
    static_assert(log_result.value<1>() == "Not Found", "Error message mismatch");

    // Выводим результат в консоль
    const auto error_code = log_result.value<0>();
    const auto error_message = log_result.value<1>();
    std::println("--- Пример 4: Данные лога ---");
    std::println("Код ошибки: {}", error_code);
    std::println("Сообщение: {}\n", error_message);

    // Пример 5: Разбор сложной строки лога с временной меткой
    constexpr auto ts_log_fmt = "[{%u}-{%u}-{%u} {%u}:{%u}:{%u}] [{}] {}"_fs;
    constexpr auto ts_log_src = fixed_string{"[2024-05-21 15:45:02] [INFO] User authentication successful"};
    constexpr auto ts_log_result = stdx::scan<ts_log_fmt, ts_log_src,
                                              unsigned, unsigned, unsigned, // Date
                                              unsigned, unsigned, unsigned, // Time
                                              std::string_view,             // Level
                                              std::string_view>();          // Message

    static_assert(ts_log_result.value<0>() == 2024, "Year mismatch");
    static_assert(ts_log_result.value<1>() == 5, "Month mismatch");
    static_assert(ts_log_result.value<2>() == 21, "Day mismatch");
    static_assert(ts_log_result.value<3>() == 15, "Hour mismatch");
    static_assert(ts_log_result.value<4>() == 45, "Minute mismatch");
    static_assert(ts_log_result.value<5>() == 2, "Second mismatch");
    static_assert(ts_log_result.value<6>() == "INFO", "Log level mismatch");
    static_assert(ts_log_result.value<7>() == "User authentication successful", "Log message mismatch");

    // Выводим результат в консоль
    const auto year = ts_log_result.value<0>(), month = ts_log_result.value<1>(), day = ts_log_result.value<2>();
    const auto hour = ts_log_result.value<3>(), minute = ts_log_result.value<4>(), second = ts_log_result.value<5>();
    const auto level = ts_log_result.value<6>();
    const auto message = ts_log_result.value<7>();
    std::println("--- Пример 5: Детализированный лог ---");
    std::println("Дата: {}-{:02}-{}", year, month, day);
    std::println("Время: {}:{:02}:{:02}", hour, minute, second);
    std::println("Уровень: {}", level);
    std::println("Сообщение: {}\n", message);

    return 0;
}