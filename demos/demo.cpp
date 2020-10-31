// a possible way to compile this application:
// g++ -std=c++17 demo.cpp -lcommon-d -lksg -lsfml-graphics -lsfml-window -lsfml-system -lz -L/media/ramdisk/ksg/demos -I../inc -I../lib/cul/inc -o demo
#include <ksg/Frame.hpp>
#include <ksg/TextArea.hpp>
#include <ksg/TextButton.hpp>
#include <ksg/OptionsSlider.hpp>
#include <ksg/ImageWidget.hpp>
#include <ksg/EditableText.hpp>

#include <SFML/Window.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Font.hpp>

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
    void setup_frame();
    bool requesting_to_close() const { return m_close_flag; }
private:
    ksg::TextArea   m_text_area    ;
    ksg::TextButton m_text_button  ;
    FruitFrame      m_embeded_frame;

    bool m_close_flag;
};

} // end of <anonymous>

int main() {
    ksg::Text::run_tests();
    DemoText dialog;
    dialog.setup_frame();

    sf::RenderWindow window;
    window.create(sf::VideoMode(unsigned(dialog.width()), unsigned(dialog.height())),
                  "Window Title", sf::Style::Close);
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
            sf::sleep(sf::microseconds(16667));
        }
    }
}

namespace {

void FruitFrame::setup_frame() {
    begin_adding_widgets().
        add(m_image_widget).
        add_line_seperator().
        // --------------------
        add_horizontal_spacer().
        add(m_slider).
        add_horizontal_spacer();

    const auto image_files_c =
        { "images/orange.jpg", "images/apple.jpg", "images/bananas.jpg" };

    m_slider.set_options({ U"Orange", U"Apple", U"Bananas" });

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
}

void DemoText::setup_frame() {
    // careful not to use/capture tempory objects here
    // this may result in a segmentation fault, and is undefined behavior
    m_text_button.set_press_event([this]() { m_close_flag = true; });

    m_text_area.set_text(U"Hello World");
    m_text_area.set_width(200.f);

    m_text_area.set_text(U"Hello World.\n"
        "Images of fruit were graciously "
        "provided by \"freefoodphotos.com\" "
        "each of which are released under "
        "the creative commons attribution "
        "(3.0) license.");

    m_text_button.set_string(U"Close Application");

    auto styles_ = ksg::styles::construct_system_styles();
    styles_[ksg::styles::k_global_font] = ksg::styles::load_font("font.ttf");
    styles_[Frame::k_border_size] = ksg::StylesField(0.f);

    m_embeded_frame.setup_frame();

    begin_adding_widgets(styles_).
        add(m_text_area).
        add_horizontal_spacer().

        add(m_embeded_frame).
        add_line_seperator().
        add_horizontal_spacer().
        add(m_text_button).
        add_horizontal_spacer();
}

} // end of <anonymous> namespace
