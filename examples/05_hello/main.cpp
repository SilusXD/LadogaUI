#include <exception>
#include <filesystem>
#include <iostream>
#include <string>

#include <ladoga/ladoga.h>

class HelloWindow final : public ladoga::Window {
public:
    HelloWindow() {
        load_from_xml(std::filesystem::path(LADOGA_EXAMPLE_DIR) / "main.xml");
        register_action("onOk", [this] { onOk(); });
    }

    ladoga::Label& greeting() { return *find_child<ladoga::Label>("greeting"); }
    ladoga::TextBox& nameBox() { return *find_child<ladoga::TextBox>("nameBox"); }

private:
    void onOk() {
        const std::string name = nameBox().text();
        greeting().set_text(name.empty() ? "Привет, мир!" : "Привет, " + name + "!");
    }
};

int main(int argc, char** argv) {
    try {
        ladoga::Application app(argc, argv);
        HelloWindow window;
        return app.run(window);
    } catch (const std::exception& error) {
        std::cerr << "LadogaUI error: " << error.what() << '\n';
        return 1;
    }
}
