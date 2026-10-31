#include <exception>
#include <filesystem>
#include <iostream>
#include <string>

#include <ladoga/ladoga.h>

#include "../gui_main.h"

class MainWindow final : public ladoga::Window {
public:
    MainWindow() {
        load_from_xml(std::filesystem::path(LADOGA_EXAMPLE_DIR) / "main.xml");
        register_action("onOk", [this] { onOk(); });
    }

    ladoga::Label& greeting() { return *find_child<ladoga::Label>("greeting"); }
    ladoga::TextBox& nameBox() { return *find_child<ladoga::TextBox>("nameBox"); }

private:
    void onOk() {
        const std::string name = nameBox().text();
        greeting().set_text(name.empty() ? "Привет, мир!" : "Привет, " + name + "!");
        set_title(name.empty() ? "LadogaUI - OK" : "LadogaUI - " + name);
    }
};

LADOGA_EXAMPLE_MAIN {
    try {
        ladoga::Application app(0, nullptr);
        MainWindow window;
        return app.run(window);
    } catch (const std::exception& error) {
        ladoga::examples::report_error(error);
        return 1;
    }
}
