#include <ksg/Frame.hpp>
#include <ksg/TextArea.hpp>
#include <ksg/TextButton.hpp>

#include <SFML/Window.hpp>
#include <SFML/Graphics/Font.hpp>

class DemoText final : public ksg::Frame {
public:
    DemoText(): m_close_flag(false) {}
    void setup_frame(const sf::Font &);
    bool requesting_to_close() const { return m_close_flag; }
private:
    ksg::TextArea   m_text_area  ;
    ksg::TextButton m_text_button;
    bool m_close_flag;
};

int main() {
    DemoText dialog;
    sf::Font font;
    font.loadFromFile("font.ttf");
    dialog.setup_frame(font);
    
    sf::RenderWindow window(sf::VideoMode(dialog.width(), dialog.height()), "Window Title");
    window.setFramerateLimit(30);
    while (window.isOpen()) {
        
        sf::Event event;
        while (window.pollEvent(event)) {
            dialog. process_event(event);
            if (event.type == sf::Event::Closed)
                window.close();
        }
        if (dialog.requesting_to_close())
            window.close();
        window.clear();
        window.draw(dialog);
        window.display();
        
    }
}

void DemoText::setup_frame(const sf::Font & font) {
    auto styles = ksg::construct_system_styles();
    styles[Frame::GLOBAL_FONT] = ksg::StylesField(&font);

    add_widget(&m_text_area);
    add_line_seperator();
    add_horizontal_spacer();
    add_widget(&m_text_button);
    add_horizontal_spacer();
    
    // careful not to use/capture tempory objects here
    // this may result in a segmentation fault, and is undefined behavior
    m_text_button.set_press_event([this]() { m_close_flag = true; });

    set_title_visible(false);
    m_text_area.set_text(U"Hello World.\n"
        "You may click and drag this dialog via the title bar.\n"
        "Dialogs are not contrained to the parent window, so if\n"
        "you were to drag the dialog outside the window, release\n"
        "the mouse, you might not be able to recover it.\n"
        "Click the button to exit the application.");
    m_text_button.set_string(U"Exit");

    set_style(styles);
    update_geometry();
}
