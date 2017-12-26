#include <ksg/Frame.hpp>
#include <ksg/TextArea.hpp>
#include <ksg/TextButton.hpp>
#include <ksg/OptionsSlider.hpp>
#include <ksg/ImageWidget.hpp>

#include <SFML/Window.hpp>
#include <SFML/Graphics/Font.hpp>

#include <iostream>

using UString = ksg::Text::UString;

namespace {

class FruitFrame final : public ksg::Frame {
public:
    void setup_frame();
private:
    ksg::ImageWidget   m_image_widget;
    ksg::OptionsSlider m_slider;

    std::vector<sf::Texture> m_fruit_images;
};

class DemoText final : public ksg::Frame {
public:
    DemoText(): m_close_flag(false) {}
    void setup_frame(const sf::Font &);
    bool requesting_to_close() const { return m_close_flag; }
private:
    ksg::TextArea      m_text_area  ;
    ksg::TextButton    m_text_button;
    FruitFrame         m_embeded_frame;

    bool m_close_flag;
};

} // end of <anonymous>

int main() {
    DemoText dialog;
    sf::Font font;
    font.loadFromFile("font.ttf");
    dialog.setup_frame(font);

    sf::RenderWindow window(sf::VideoMode(unsigned(dialog.width()),
                                          unsigned(dialog.height())), "Window Title");
    window.setFramerateLimit(30);
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            dialog.process_event(event);
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

namespace {

void FruitFrame::setup_frame() {
    add_widget(&m_image_widget);
    add_line_seperator();
    add_horizontal_spacer();
    add_widget(&m_slider);
    add_horizontal_spacer();

    const auto image_files_c = 
        { "images/orange.jpg", "images/apple.jpg", "images/bananas.jpg" };
    std::vector<UString> options_list = { U"Orange", U"Apple", U"Bananas" };

    m_slider.swap_options(options_list);

    for (const auto & image_file : image_files_c) {
        m_fruit_images.emplace_back();
        m_fruit_images.back().loadFromFile(image_file);
    }
    m_image_widget.set_texture(m_fruit_images.front());

    m_slider.set_option_change_event([this]() {
        m_image_widget.set_texture
            (m_fruit_images[m_slider.selected_option_index()]);
    });
    m_image_widget.set_size(200.f, 150.f);
    set_title_visible(false);
}

void DemoText::setup_frame(const sf::Font & font) {
    auto styles = ksg::construct_system_styles();
    styles[Frame::GLOBAL_FONT] = ksg::StylesField(&font);

    m_embeded_frame.setup_frame();

    add_widget(&m_text_area);
    add_widget(&m_embeded_frame);
    add_line_seperator();
    add_horizontal_spacer();
    add_widget(&m_text_button);
    add_horizontal_spacer();

    // careful not to use/capture tempory objects here
    // this may result in a segmentation fault, and is undefined behavior
    m_text_button.set_press_event([this]() { m_close_flag = true; });

    set_title_visible(false);
    m_text_area.set_text(U"Hello World.\n"
        "Images of fruit were graciously provided by\n"
        "\"freefoodphotos.com\" each of which are\n"
        "released under the creative commons\n"
        "attribution (3.0) license.");
    m_text_button.set_string(U"Close Application");

    set_style(styles);
    update_geometry();
}

} // end of <anonymous> namespace
