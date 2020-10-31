#include <ksg/Frame.hpp>
#include <ksg/TextArea.hpp>
#include <ksg/EditableText.hpp>
#include <ksg/TextButton.hpp>
#include <ksg/SelectionMenu.hpp>

#include <SFML/Graphics/RenderWindow.hpp>

#include <thread>
#include <chrono>

using namespace ksg;

namespace {

class EditableTextFrame final : public Frame {
public:
    EditableTextFrame(): m_request_close_flag(false) {}

    bool requesting_to_close() const { return m_request_close_flag; }
    void setup_frame();
private:
    SelectionMenu m_menu;
    TextArea m_option_text;

    TextArea m_text_area;
    EditableText m_editable_text;
    TextButton m_exit_button;

    bool m_request_close_flag;
};

} // end of <anonymous> namespace

int main() {
#   if 0
    MultiType<int, char> imsad;
    MultiType<int, char> b = imsad;
#   endif
    EditableTextFrame dialog;
    dialog.setup_frame();

    sf::RenderWindow window(
        sf::VideoMode(unsigned(dialog.width()), unsigned(dialog.height())), 
        "Window Title");
    window.setFramerateLimit(20);
    bool has_events = true;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            has_events = true;
            dialog.process_event(event);
            if (event.type == sf::Event::Closed)
                window.close();
        }
        if (dialog.requesting_to_close())
            window.close();
        if (has_events) {
            window.clear();
            window.draw(dialog);
            window.display();
            has_events = false;
        } else {
            std::this_thread::sleep_for(std::chrono::microseconds(16667));
        }
    }
    return 0;
}

namespace {

void EditableTextFrame::setup_frame() {
    m_option_text.set_text(U"none selected");

    m_menu.set_size(150.f, 150.f);
    m_menu.add_options({
        U"Option 1",
        U"Option 2",
        U"Option 3"
    });
    m_menu.set_response_function([this](std::size_t, const UString & ustr) {
        m_option_text.set_text(ustr);
    });

    m_text_area.set_text(U"Editable Text Box:");
    m_text_area.set_size(150.f, 32.f);

    m_editable_text.set_width(150.f);

    m_exit_button.set_press_event([this]() { m_request_close_flag = true; });
    m_exit_button.set_string(U"Close Application");

    auto styles = ksg::styles::construct_system_styles();
    styles[styles::k_global_font] = ksg::styles::load_font("font.ttf");

    begin_adding_widgets(styles).
        add(m_menu).
        add(m_option_text).
        add_line_seperator().

        add(m_text_area).
        add(m_editable_text).
        add_line_seperator().

        add(m_exit_button);
#   if 0


    add_widget(&m_menu);
    add_widget(&m_option_text);
    add_line_seperator();

    add_widget(&m_text_area);
    add_widget(&m_editable_text);
    add_line_seperator();
    add_widget(&m_exit_button);

    finalize_widgets(styles);
#   endif
}

} // end of <anonymous> namespace
