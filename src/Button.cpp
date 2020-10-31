/****************************************************************************

    File: Button.cpp
    Author: Aria Janke
    License: GPLv3

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*****************************************************************************/

#include <ksg/Button.hpp>
#include <ksg/Frame.hpp>

#include <SFML/Graphics/RenderTarget.hpp>

#include <cassert>

namespace {

using StyleMap = ksg::StyleMap;
using InvalidArg = std::invalid_argument;

bool is_in_drect(int x, int y, const DrawRectangle & drect) {
    return (x >= drect.x() && y >= drect.y()
            && x <= drect.x() + drect.width()
            && y <= drect.y() + drect.height());
}

bool is_click_in(const sf::Event::MouseButtonEvent & mouse,
              const DrawRectangle & drect)
{
    return is_in_drect(mouse.x, mouse.y, drect);
}

bool is_mouse_in(const sf::Event::MouseMoveEvent & mouse,
              const DrawRectangle & drect)
{
    return is_in_drect(mouse.x, mouse.y, drect);
}
} // end of <anonymous> namespace

namespace ksg {

/* static */ constexpr const char * const Button::k_hover_back_color;
/* static */ constexpr const char * const Button::k_hover_front_color;
/* static */ constexpr const char * const Button::k_regular_back_color;
/* static */ constexpr const char * const Button::k_regular_front_color;

void Button::process_event(const sf::Event & evnt) {
    switch (evnt.type) {
    case sf::Event::MouseButtonReleased:
        if (m_is_highlighted && is_click_in(evnt.mouseButton, m_outer)) {
            press();
        }
        break;
    case sf::Event::MouseMoved:
        if (is_mouse_in(evnt.mouseMove, m_outer))
            highlight();
        else
            deselect();
        break;
    case sf::Event::MouseLeft: case sf::Event::LostFocus:
    case sf::Event::Resized:
        deselect();
        break;
    default: break;
    }
}

void Button::set_location(float x, float y) {
    float old_x = location().x, old_y = location().y;
    m_outer.set_position(x, y);
    m_inner.set_position(x + m_padding, y + m_padding);

    set_button_frame_size(width(), height());
    on_location_changed(old_x, old_y);
}

void Button::set_style(const StyleMap & smap) {
    using namespace styles;

    set_if_found(smap, k_hover_back_color     , m_hover.back );
    set_if_found(smap, k_hover_front_color    , m_hover.front);
    set_if_found(smap, k_regular_back_color   , m_reg.back   );
    set_if_found(smap, k_regular_front_color  , m_reg.front  );
    set_if_found(smap, k_global_padding, m_padding    );

    m_outer.set_color(m_reg.back);
    m_inner.set_color(m_reg.front);
}

void Button::set_press_event(BlankFunctor && func) {
    m_press_functor = std::move(func);
}

void Button::press() {
    if (m_press_functor)
        m_press_functor();
}

void Button::set_size(float width_, float height_) {
    if (width_ <= 0.f || height_ <= 0.f) {
        throw InvalidArg("ksg::Button::set_size: width and height must be "
                         "positive real numbers (which excludes zero)."    );
    }

    float old_width = width(), old_height = height();

    set_button_frame_size(width_, height_);
    set_size_back(width_, height_);

    on_size_changed(old_width, old_height);
}

/* protected */ Button::Button() {}

/* protected */ void Button::draw
    (sf::RenderTarget & target, sf::RenderStates) const
{
    target.draw(m_outer);
    target.draw(m_inner);
}

/* protected */ void Button::on_size_changed(float, float) { }

/* protected */ void Button::on_location_changed(float, float) { }

/* protected */ void Button::set_size_back(float, float) { }

/* protected */ void Button::set_button_frame_size
    (float width_, float height_)
{
    m_outer.set_size(width_, height_);
    m_inner.set_size(std::max(width_  - m_padding*2.f, 0.f),
                     std::max(height_ - m_padding*2.f, 0.f));
}

/* private */ void Button::deselect() {
    m_is_highlighted = false;
    m_inner.set_color(m_reg.front);
    if (has_focus()) {
        m_outer.set_color(m_hover.front);
    } else {
        m_outer.set_color(m_reg.back);
    }
}

/* private */ void Button::highlight() {
    m_is_highlighted = true;
    m_inner.set_color(m_hover.front);
    if (has_focus()) {
        m_outer.set_color(m_hover.front);
    } else {
        m_outer.set_color(m_hover.back);
    }
}
#if 0
/* private */ void Button::add_focus_widgets_to
    (std::vector<FocusWidget *> & cont)
{
    cont.push_back(this);
}
#endif
/* private */ void Button::process_focus_event(const sf::Event & event) {
    if (event.type == sf::Event::KeyReleased) {
        if (event.key.code == sf::Keyboard::Return) {
            m_press_functor();
        }
    }
}

/* private */ void Button::notify_focus_gained() {
    m_outer.set_color(m_hover.front);
}

/* private */ void Button::notify_focus_lost() {
    m_outer.set_color(m_reg.back);
}

} // end of ksg namespace
