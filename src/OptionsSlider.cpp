/****************************************************************************

    File: OptionsSlider.cpp
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

#include <ksg/OptionsSlider.hpp>
#include <ksg/Frame.hpp>
#include <ksg/TextButton.hpp>
#include <ksg/TextArea.hpp>

#include <SFML/Graphics/RenderTarget.hpp>

#include <iostream>

#include <cassert>

namespace ksg {

using VectorF = OptionsSlider::VectorF;
using UString = OptionsSlider::UString;

OptionsSlider::OptionsSlider(): m_selected_index(0), m_padding(0.f) {
    m_left_arrow.set_direction(ArrowButton::Direction::k_left);
    m_right_arrow.set_direction(ArrowButton::Direction::k_right);

    m_right_arrow.set_press_event([this]() {
        if (selected_option_index() == options_count() - 1) return;
        select_option(selected_option_index() + 1);
        if (m_press_func)
            m_press_func();
    });
    m_left_arrow.set_press_event([this]()  {
        if (selected_option_index() == 0) return;
        select_option(selected_option_index() - 1);
        if (m_press_func)
            m_press_func();
    });
#   if 0
    set_next(&m_left_arrow);

    m_left_arrow.set_next(&m_right_arrow);
#   endif
}

void OptionsSlider::process_event(const sf::Event & evnt) {
    m_left_arrow.process_event(evnt);
    m_right_arrow.process_event(evnt);
}

void OptionsSlider::set_location(float x, float y) {
    m_left_arrow.set_location(x, y);
    if (is_horizontal()) {
        m_back.set_position(x + m_left_arrow.width(), y);
        m_front.set_position(x + m_left_arrow.width(), y + m_padding);
        m_right_arrow.set_location(x + m_left_arrow.width() + m_back.width(),
                                   y);
    } else {
        assert(is_vertical());
        m_back.set_position(x, y + m_left_arrow.height());
        m_front.set_position(x + m_padding, y + m_left_arrow.height());
        m_right_arrow.set_location(x,
                                   y + m_left_arrow.height() + m_back.height());
    }
    recenter_text();
}

VectorF OptionsSlider::location() const {
    return m_left_arrow.location();
}

float OptionsSlider::width() const {
    return m_size.x;
}

float OptionsSlider::height() const {
    return m_size.y;
}

void OptionsSlider::set_style(const StyleMap & smap) {
    using namespace styles;
    m_left_arrow.set_style(smap);
    m_right_arrow.set_style(smap);

    set_if_present(m_text, smap, k_global_font,
                   TextButton::k_text_size, TextButton::k_text_color);
    if (auto * pad = find<float>(smap, k_global_padding))
        { m_padding = *pad; }

    if (auto * color = find<sf::Color>(smap, Button::k_regular_front_color))
        m_front.set_color(*color);
    if (auto * color = find<sf::Color>(smap, Button::k_regular_back_color))
        m_back.set_color(*color);
    // setting style should not invoke any kind of geometry update
}

void OptionsSlider::set_size(float w, float h) {
    if (w == 0.f || h == 0.f) return;
    if (m_padding > w || m_padding > h) {
        // have to get right of padding, constraints too tight!
        m_padding = 0.f;
    }
    m_size = VectorF(w, h);

    float arrow_size = size_for_arrows(w, h);
    m_left_arrow .set_size(arrow_size, arrow_size);
    m_right_arrow.set_size(arrow_size, arrow_size);

    m_back .set_size(w - arrow_size*2.f, h);
    m_front.set_size(w - arrow_size*2.f, h - m_padding*2.f);
#   if 0
    static constexpr const float k_force_min = 1.f;
    if (h < w) {
        // horizontal
        float long_dim = std::max(k_force_min, w - h*2.f);
        m_back.set_size(long_dim, h);
        m_front.set_size(long_dim, std::max(k_force_min, h - m_padding*2.f));
    } else {
        // vertical
        float long_dim = std::max(k_force_min, h - w*2.f);
        m_back.set_size(w, long_dim);
        m_front.set_size(std::max(k_force_min, w - m_padding*2.f), long_dim);
    }
#   endif
    set_location(location().x, location().y);
    m_text.set_limiting_dimensions(m_front.width(), m_front.height());
    recenter_text();
}

void OptionsSlider::swap_options(std::vector<UString> & options) {
    m_options.swap(options);
    select_option(0);
}

void OptionsSlider::set_options(const std::vector<UString> & options) {
    auto t = options;
    set_options(std::move(t));
}

void OptionsSlider::set_options(std::vector<UString> && options) {
    m_options.swap(options);
    select_option(0);
}

void OptionsSlider::select_option(std::size_t index) {
    if (index >= m_options.size()) {
        throw std::out_of_range("OptionsSlider::select_option: Index is out "
                                "of range");
    }
    m_selected_index = index;
    m_text.set_string(m_options[m_selected_index]);
    recenter_text();
}

std::size_t OptionsSlider::selected_option_index() const {
    return m_selected_index;
}

const UString & OptionsSlider::selected_option() const {
    return m_options[m_selected_index];
}

void OptionsSlider::set_option_change_event(BlankFunctor && func) {
    m_press_func = std::move(func);
}

/* private */ void OptionsSlider::draw
    (sf::RenderTarget & target, sf::RenderStates) const
{
    target.draw(m_back);
    target.draw(m_front);
    target.draw(m_text);
    target.draw(m_left_arrow);
    target.draw(m_right_arrow);
}

/* private */ void OptionsSlider::issue_auto_resize() {
    if (width() != 0.f || height() != 0.f) return;
    float width_ = 0.f, height_ = 0.f;
    for (const auto & str : m_options) {
        auto gv = m_text.measure_text(str);
        width_  = std::max(width_ , gv.width );
        height_ = std::max(height_, gv.height);
    }

    height_ += 2.f*m_padding;
    auto arrow_size = size_for_arrows(width_, height_);
    width_  += 2.f*(m_padding + arrow_size);

    set_size(width_, height_);
}

#if 0
/* private */ void OptionsSlider::add_focus_widgets_to
    (std::vector<FocusWidget *> & cont)
{
    cont.push_back(&m_left_arrow );
    cont.push_back(&m_right_arrow);
}
#endif

/* private */ void OptionsSlider::iterate_children_
    (ChildWidgetIterator & itr)
{
    itr.on_child(m_left_arrow );
    itr.on_child(m_right_arrow);
}

/* private */ void OptionsSlider::iterate_const_children_
    (ChildWidgetIterator & itr) const
{
    itr.on_child(m_left_arrow );
    itr.on_child(m_right_arrow);
}

/* private */ void OptionsSlider::recenter_text() {
    // assume size has been decided, use front
    float width_diff  = m_front.width () - m_text.width ();
    float height_diff = m_front.height() - m_text.height();
    m_text.set_location(m_front.x() + std::max(0.f, width_diff  / 2.f),
                        m_front.y() + std::max(0.f, height_diff / 2.f));
}

} // end of ksg namespace
