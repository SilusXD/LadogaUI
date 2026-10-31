#include <charconv>
#include <exception>
#include <filesystem>
#include <optional>
#include <string>
#include <system_error>

#include <ladoga/ladoga.h>

#include "../gui_main.h"

namespace {

/// Разбирает строку целиком как целое число. Пустая строка, пробелы и буквы —
/// ошибка (std::nullopt).
std::optional<int> parse_int(const std::string& text) {
    if (text.empty()) {
        return std::nullopt;
    }
    int value = 0;
    const char* begin = text.data();
    const char* end = begin + text.size();
    const auto [ptr, error] = std::from_chars(begin, end, value);
    if (error != std::errc{} || ptr != end) {
        return std::nullopt;
    }
    return value;
}

} // namespace

class SumWindow final : public ladoga::Window {
public:
    SumWindow() {
        load_from_xml(std::filesystem::path(LADOGA_EXAMPLE_DIR) / "main.xml");
        register_action("onSum", [this] { onSum(); });
    }

    ladoga::TextBox& boxA() { return *find_child<ladoga::TextBox>("boxA"); }
    ladoga::TextBox& boxB() { return *find_child<ladoga::TextBox>("boxB"); }
    ladoga::Label& result() { return *find_child<ladoga::Label>("result"); }

private:
    void onSum() {
        const std::optional<int> a = parse_int(boxA().text());
        const std::optional<int> b = parse_int(boxB().text());
        if (!a || !b) {
            result().set_text("Ошибка: оба поля должны содержать целое число");
            return;
        }
        // long long: сумма двух int может не поместиться в int.
        const long long sum = static_cast<long long>(*a) + static_cast<long long>(*b);
        result().set_text("Результат: " + std::to_string(sum));
    }
};

LADOGA_EXAMPLE_MAIN {
    try {
        ladoga::Application app(0, nullptr);
        SumWindow window;
        return app.run(window);
    } catch (const std::exception& error) {
        ladoga::examples::report_error(error);
        return 1;
    }
}
