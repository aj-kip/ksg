/****************************************************************************

    File: FrameBorder.cpp
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

#include <ksg/FrameBorder.hpp>
#include <ksg/Frame.hpp>
#include <ksg/TextArea.hpp>

#include <SFML/Graphics/RenderTarget.hpp>

#if 0
#include <iostream>
#endif

#include <cassert>

namespace {

using VectorF          = ksg::Widget::VectorF;
using Text             = ksg::Text;
using MouseButtonEvent = sf::Event::MouseButtonEvent;

void update_title_geometry(VectorF location,
                           const DrawRectangle & title_bar, Text * title);

bool mouse_is_inside(MouseButtonEvent mouse, const DrawRectangle &);

} // end of <anonymous> namespace

namespace ksg {

namespace detail {

// <----------------------- Private types for frame -------------------------->
// implemenation here, to keep vtable out of every translation unit
// (revealed by clang static analysis)

// method used as anchor for LineSeperator
LineSeperator::~LineSeperator() {}

void HorizontalSpacer::set_location(float x_, float y_)
    { m_location = VectorF(x_, y_); }

VectorF HorizontalSpacer::location() const
    { return m_location; }

float HorizontalSpacer::width() const
    { return m_width; }

void HorizontalSpacer::set_width(float w) {
    assert(w >= 0.f);
    m_width = w;
}

} // end of detail namespace

// ----------------------------------------------------------------------------
// ---                             FrameBorder                              ---

/* static */ constexpr const float FrameBorder::k_default_padding;

VectorF FrameBorder::widget_start() const noexcept {
    VectorF offset(m_outer_padding, m_outer_padding + title_height());
    if (!m_title.string().empty()) {
        offset.y += m_outer_padding;
    }
    return location() + offset;
}

VectorF FrameBorder::location() const noexcept { return m_back.position(); }

FrameBorder::EventResponseSignal FrameBorder::process_event
    (const sf::Event & event)
{
    check_should_update_drag(event);

    EventResponseSignal rv;
    rv.should_update_geometry = m_recently_dragged;

    if (   event.type == sf::Event::MouseButtonPressed
        && mouse_is_inside(event.mouseButton, m_back))
    {
#       if 0
        std::cout << "clicked inside; mouse (" << event.mouseButton.x << ", "
                  << event.mouseButton.y << ") bounds (" << m_back.x() << ", "
                  << m_back.y() << ", " << m_back.width() << ", " << m_back.height()
                  << ")" << std::endl;
#       endif
        assert(m_click_in_frame);
        rv.skip_other_events = (m_click_in_frame() == k_skip_other_events);
    }

    return rv;
}

void FrameBorder::set_location(float x, float y)
    { m_back.set_position(x, y); }

void FrameBorder::set_style(const StyleMap & smap) {
    using namespace styles;
    set_if_present(m_title, smap, k_global_font, Frame::k_title_size,
                   Frame::k_title_color);

    if (auto * color = find<sf::Color>(smap, Frame::k_background_color))
        m_back.set_color(*color);
    if (auto * color = find<sf::Color>(smap, Frame::k_title_bar_color))
        m_title_bar.set_color(*color);
    if (auto * color = find<sf::Color>(smap, Frame::k_widget_body_color))
        m_widget_body.set_color(*color);
    if (auto * pad = find<float>(smap, Frame::k_border_size)) {
        m_outer_padding = *pad;
    } else if (auto * pad = find<float>(smap, k_global_padding)) {
        m_outer_padding = *pad;
    }
}

void FrameBorder::set_size(float w, float h)
    { m_back.set_size(w, h); }

void FrameBorder::set_title(const UString & title_text) {
    m_title.set_string(title_text);
    if (title_text.empty()) {
        ignore_drag_events();
    } else {
        watch_for_drag_events();
    }
}

void FrameBorder::reset_register_click_event()
    { m_click_in_frame = do_default_click_event; }

void FrameBorder::update_geometry() {
    const auto loc = location();
    auto w = m_back.width();
    auto h = m_back.height();
    const float c_title_bar_height = title_height();
    const float c_title_bar_pad = m_title.is_visible() ? m_outer_padding : 0.f;
    if (m_title.is_visible()) {
        m_title_bar.set_position(loc.x + m_outer_padding, loc.y + m_outer_padding);
        m_title_bar.set_size(w - m_outer_padding*2.f, c_title_bar_height);
        update_title_geometry(loc, m_title_bar, &m_title);
    }
    m_widget_body.set_position
        (loc.x + m_outer_padding,
         loc.y + c_title_bar_height + m_outer_padding + c_title_bar_pad);
    m_widget_body.set_size
        (w - m_outer_padding*2.f,
         h - (c_title_bar_height + m_outer_padding*2.f + c_title_bar_pad));
}

float FrameBorder::title_width_accommodation() const noexcept
    { return m_title.string().empty() ? 0.f : m_title.width(); }

float FrameBorder::width_available_for_widgets() const noexcept
    { return m_widget_body.width(); }

/* private */ void FrameBorder::update_drag_position
    (int drect_x, int drect_y)
{
    set_location(float(drect_x), float(drect_y));
    // save and later send signal for geo update to frame, not the most clean
    // solution, just the least worst given the circumstances
    m_recently_dragged = true;
}

/* private */ void FrameBorder::draw
    (sf::RenderTarget & target, sf::RenderStates states) const
{
    target.draw(m_back, states);
    target.draw(m_title_bar, states);
    target.draw(m_widget_body, states);

    if (!m_title.string().empty())
        target.draw(m_title, states);
}

/* private */ float FrameBorder::title_height() const noexcept
    { return m_title.string().empty() ? 0.f : m_title.character_size()*2.f; }

/* private */ void FrameBorder::check_should_update_drag
    (const sf::Event & event)
{
    m_recently_dragged = false;
    switch (event.type) {
    case sf::Event::MouseButtonPressed:
        mouse_click(event.mouseButton.x, event.mouseButton.y, m_title_bar);
        break;
    case sf::Event::MouseButtonReleased:
        drag_release();
        break;
    case sf::Event::MouseMoved:
        mouse_move(event.mouseMove.x, event.mouseMove.y);
        break;
    default: break;
    }
}

/* private static */ FrameBorder::ClickResponse
    FrameBorder::do_default_click_event()
    { return k_continue_other_events; }

} // end of ksg namespace

namespace {

void update_title_geometry(VectorF location, const DrawRectangle & title_bar,
                           Text * title)
{
    title->set_limiting_dimensions(title_bar.width(), title_bar.height());
    VectorF title_offset(
        (title_bar.width () - title->width ()) / 2.f,
        (title_bar.height() - title->height()) / 2.f);
    title->set_location(location + title_offset);
}

bool mouse_is_inside(MouseButtonEvent mouse, const DrawRectangle & drect) {
    return mouse.x >= drect.x() && mouse.x <= drect.x() + drect.width () &&
           mouse.y >= drect.y() && mouse.y <= drect.y() + drect.height();
}

} // end of <anonymous> namespace
