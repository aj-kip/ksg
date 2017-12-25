/****************************************************************************

    File: OptionsSlider.cpp
    Author: Andrew Janke
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
#include <ksg/Visitor.hpp>
#include <ksg/TextArea.hpp>

#include <iostream>

#include <cassert>

namespace {

template <typename T>
const T * min_ptr_of(const T & u, const T & v)
    { return (u < v) ? &u : &v; }

} // end of <anonymous> namspace

namespace ksg {

using VectorF = OptionsSlider::VectorF;
using UString = OptionsSlider::UString;

OptionsSlider::OptionsSlider(): m_selected_index(0), m_padding(0.f) {
    m_left_arrow.set_direction(ArrowButton::Direction::LEFT);
    m_right_arrow.set_direction(ArrowButton::Direction::RIGHT);

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
    using std::bind;
    using namespace std::placeholders;
    StyleFinder sfinder(smap);
    m_left_arrow.set_style(smap);
    m_right_arrow.set_style(smap);

    set_if_present(m_text, smap, Frame::GLOBAL_FONT,
                   TextButton::TEXT_SIZE, TextButton::TEXT_COLOR);
    sfinder.set_if_found(Frame::GLOBAL_PADDING, &m_padding);

    sfinder.call_if_found<sf::Color>
        (Button::REG_FRONT_COLOR, bind(&DrawRectangle::set_color, &m_front, _1));
    sfinder.call_if_found<sf::Color>
        (Button::REG_BACK_COLOR, bind(&DrawRectangle::set_color, &m_back, _1));
    // setting style should not invoke any kind of geometry update
}

void OptionsSlider::accept(Visitor & visitor)
    { visitor.visit(*this); }

void OptionsSlider::accept(const Visitor & visitor) const
    { visitor.visit(*this); }

void OptionsSlider::set_size(float w, float h) {
    if (w == 0.f || h == 0.f) return;
    m_size = VectorF(w, h);
    {
    float min_dim = std::min(w, h);
    m_left_arrow .set_size(min_dim, min_dim);
    m_right_arrow.set_size(min_dim, min_dim);
    }

    if (min_ptr_of(w, h) == &h) {
        // horizontal
        float long_dim = std::max(0.f, w - h*2.f);
        m_back.set_size(long_dim, h);
        m_front.set_size(long_dim, std::max(0.f, h - m_padding*2.f));
    } else {
        // vertical
        float long_dim = std::max(0.f, h - w*2.f);
        m_back.set_size(w, long_dim);
        m_front.set_size(std::max(0.f, w - m_padding*2.f), long_dim);
    }
    set_location(location().x, location().y);
    m_text.set_limiting_dimensions(m_front.width(), m_front.height());
    recenter_text();
}

void OptionsSlider::swap_options(std::vector<UString> & options) {
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
    float width_ = 0.f, height_ = 0.f;
    for (const auto & str : m_options) {
        auto gv = m_text.measure_text(str);
        width_  = std::max(width_ , gv.width );
        height_ = std::max(height_, gv.height);
    }
    m_text.set_limiting_dimensions(Text::NO_SIZE_LIMIT);
    set_size(width_ + 6.f*m_padding, height_ + 2.f*m_padding);
}

/* private */ void OptionsSlider::recenter_text() {
    // assume size has been decided, use front
    float width_diff  = m_front.width () - m_text.width ();
    float height_diff = m_front.height() - m_text.height();
    m_text.set_location(m_front.x() + std::max(0.f, width_diff  / 2.f),
                        m_front.y() + std::max(0.f, height_diff / 2.f));
}

} // end of ksg namespace
