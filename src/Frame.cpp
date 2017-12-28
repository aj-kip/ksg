/****************************************************************************

    File: Frame.cpp
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

#include <common/Util.hpp>

#include <ksg/Frame.hpp>
#include <ksg/Visitor.hpp>
#include <ksg/TextArea.hpp>

#include <stdexcept>
#include <iostream>
#include <cassert>

namespace {

using Error            = std::runtime_error;
using VectorF          = ksg::Frame::VectorF;
using Widget           = ksg::Widget;
using Text             = ksg::Text;
using MouseButtonEvent = sf::Event::MouseButtonEvent;
using Frame            = ksg::Frame;

void update_title_geometry(VectorF location,
                           const DrawRectangle & title_bar, Text * title);

bool mouse_is_in(MouseButtonEvent mouse, const DrawRectangle &);

Frame::ClickResponse default_click_func()
    { return Frame::CONTINUE_OTHER_EVENTS; }

} // end of <anonymous> namespace

namespace ksg {

Frame::Frame():
    m_padding(2.f), m_outer_padding(2.f),
    m_draggable(false), m_title_visible(true),
    m_click_in_frame(default_click_func)
{
    check_invarients();
}

void Frame::set_location(float x, float y) {
    set_frame_location(x, y);
    check_invarients();
}

void Frame::process_event(const sf::Event & event) {
    assert(m_click_in_frame);
    bool should_do_sub_events = true;
    if (event.type == sf::Event::MouseButtonPressed &&
        mouse_is_in(event.mouseButton, m_back))
    {
        should_do_sub_events = (m_click_in_frame() == CONTINUE_OTHER_EVENTS);
    }
    if (should_do_sub_events) {
        for (Widget * widget_ptr : m_widgets) {
            if (widget_ptr->is_visible())
                widget_ptr->process_event(event);
        }
    }
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
    check_invarients();
}

void Frame::set_style(const StyleMap & smap) {
    StyleFinder sfinder(smap);
    using namespace std::placeholders;
    using std::bind;
    sfinder.call_if_found<sf::Color>
        (BACKGROUND_COLOR, bind(&DrawRectangle::set_color, &m_back, _1));
    set_if_present(m_title, smap, GLOBAL_FONT, TITLE_SIZE, TITLE_COLOR);

    sfinder.call_if_found<sf::Color>
        (TITLE_BAR_COLOR, bind(&DrawRectangle::set_color, &m_title_bar, _1));
    sfinder.call_if_found<sf::Color>
        (WIDGET_BODY_COLOR, bind(&DrawRectangle::set_color, &m_widget_body, _1));
    sfinder.set_if_found(GLOBAL_PADDING, &m_padding);
    auto itr = smap.find(BORDER_SIZE);
    if (itr == smap.end()) {
        sfinder.set_if_found(GLOBAL_PADDING, &m_outer_padding);
    } else if (itr->second.is_type<float>()) {
        m_outer_padding = itr->second.as<float>();
    }
    for (Widget * widget_ptr : m_widgets)
        widget_ptr->set_style(smap);
    check_invarients();
}

float Frame::width() const {
    return m_back.width();
}

float Frame::height() const {
    return m_back.height();
}

void Frame::set_size(float w, float h) {
    m_back.set_size(w, h);
    check_invarients();
}

void Frame::accept(Visitor & visitor) {
    visitor.visit(*this);
    check_invarients();
}

void Frame::accept(const Visitor & visitor) const
    { visitor.visit(*this); }

void Frame::add_widget(Widget * comp_ptr) {
    if (!comp_ptr) {
        throw std::invalid_argument("Frame::add_widget requires a non-nullptr "
                                    "argument.");
    }
    m_widgets.push_back(comp_ptr);
    check_invarients();
}

void Frame::add_horizontal_spacer() {
    // A little word on the C++ standard:
    // According to: http://stackoverflow.com/questions/5410035/when-does-a-stdvector-reallocate-its-memory-array
    // From C++ standard 23.2.4.2
    // <quote>
    // It is guaranteed that no reallocation takes place during insertions that
    // happen after a call to reserve() until the time when an insertion would
    // make the size of the vector greater than the size specified in the most
    // recent call to reserve().
    // </quote>

    // update and revalidate all pointers to spacers as necessary
    if (   m_horz_spacers.size() == m_horz_spacers.capacity()
        && !m_horz_spacers.empty())
    {
        std::vector<HorizontalSpacer> new_spacer_cont;
        // reserve and copy
        new_spacer_cont.reserve(1 + m_horz_spacers.capacity()*2);
        (void)std::copy(m_horz_spacers.begin(), m_horz_spacers.end(),
                        std::back_inserter(new_spacer_cont));
        // revalidate pointers in the widget container
        HorizontalSpacer * new_itr = &new_spacer_cont[0];
        const HorizontalSpacer * old_itr = &m_horz_spacers[0];
        for (Widget *& widget_ptr : m_widgets) {
            if (widget_ptr == old_itr) {
                widget_ptr = new_itr++;
                ++old_itr;
            }
        }
        // safely get the end() pointer
        assert(new_itr == (new_spacer_cont.data() + new_spacer_cont.size()));
        m_horz_spacers.swap(new_spacer_cont);
    }

    // otherwise add a spacer as normal
    m_horz_spacers.push_back(HorizontalSpacer());
    m_widgets.push_back(&m_horz_spacers.back());
    check_invarients();
}

void Frame::clear_all_widgets() {
    m_widgets.clear();
    m_horz_spacers.clear();
    check_invarients();
}

void Frame::set_title(const UString & str) {
    m_title.set_string(str);
    if (m_title.string().empty())
        set_title_visible(false);
    check_invarients();
}

void Frame::set_title_visible(bool v) {
    bool old_title_visible = m_title_visible;
    m_draggable = m_title_visible = v;
    if (old_title_visible != m_title_visible)
        set_size(width(), height());
    check_invarients();
}

void Frame::draw(sf::RenderTarget & target, sf::RenderStates) const {
    if (!is_visible()) return;

    target.draw(m_back);
    if (m_title_visible) {
        target.draw(m_title_bar);
        target.draw(m_title);
    }
    target.draw(m_widget_body);
    for (Widget * widget_ptr : m_widgets) {
        if (widget_ptr->is_visible())
            target.draw(*widget_ptr);
    }
#   if 1
    DrawRectangle dr;
    dr.set_color(sf::Color::Red);
    for (Widget * widget_ptr : m_widgets) {
        if (!is_horizontal_spacer(widget_ptr)) continue;
        dr.set_position(widget_ptr->location().x, widget_ptr->location().y);
        dr.set_size    (widget_ptr->width()     , 20.f);
        target.draw(dr);
    }
#   endif
}

/* private */ void Frame::set_frame_location(float x, float y) {
    m_back.set_position(x, y);
}

void Frame::update_geometry() {
    // auto sizing
    issue_auto_resize();

    // must come before horizontal spacer updates
    update_head_and_back_geometry();

    // update horizontal spacer sizes
    update_horizontal_spacers();

    const float start_x = m_widget_body.x() + m_padding;
    float x = start_x;
    float y = m_widget_body.y() + m_padding;

    float line_height = 0.f;
    float pad_fix     = 0.f;
    auto advance_locals_to_next_line = [&]() {
        y += line_height + m_padding;
        x = start_x;
        line_height = 0.f;
        pad_fix = 0.f;
    };
    const float right_limit_c = location().x + width();
    for (Widget * widget_ptr : m_widgets) {
        assert(widget_ptr);
        // horizontal overflow
        if (is_line_seperator(widget_ptr)) {
            advance_locals_to_next_line();
            continue;
        }
        if (x + get_widget_advance(widget_ptr) > right_limit_c) {
            advance_locals_to_next_line();
            // this widget_ptr is placed as the first element of the line
        }
        if (is_horizontal_spacer(widget_ptr))
            x += pad_fix;

        widget_ptr->set_location(x, y);

        line_height = std::max(widget_ptr->height(), line_height);

        // horizontal advance
        x += get_widget_advance(widget_ptr);
        pad_fix = -m_padding;
    }
    // note: there is no consideration given to "vertical overflow"
    // not considering if additional widgets overflow the frame's
    // height

    for (Widget * widget_ptr : m_widgets) {
        auto * frame_ptr = dynamic_cast<Frame *>(widget_ptr);
        if (frame_ptr)
            frame_ptr->update_geometry();
    }
    check_invarients();
}

void Frame::reset_register_click_event() {
    m_click_in_frame = default_click_func;
    check_invarients();
}

/* private */ VectorF Frame::compute_size_to_fit() const {
    float total_width  = 0.f;
    float line_width   = 0.f;
    float total_height = 0.f;
    float line_height  = 0.f;
    float pad_fix      = 0.f;

    for (Widget * widget_ptr : m_widgets) {
        assert(widget_ptr);
        if (is_horizontal_spacer(widget_ptr)) {
            pad_fix = -m_padding;
            continue;
        }
        if (is_line_seperator(widget_ptr)) {
            total_width   = std::max(line_width, total_width);
            assert(!util::is_nan(total_width));
            line_width    = 0.f;
            total_height += line_height + m_padding;
            line_height   = 0.f;
            pad_fix       = 0.f;
            continue;
        }
        float width  = widget_ptr->width ();
        float height = widget_ptr->height();
        Frame * widget_as_frame = nullptr;
        // should I issue auto-resize here?
        if (width == 0.f && height == 0.f
            && (widget_as_frame = dynamic_cast<Frame *>(widget_ptr)))
        {
            auto gv = widget_as_frame->compute_size_to_fit();
            width  = gv.x;
            height = gv.y;
        }
        line_width  += get_widget_advance(widget_ptr) + pad_fix;
        line_height  = std::max(line_height, height);
        pad_fix = 0.f;
    }

    if (line_width != 0.f) {
        total_width   = std::max(total_width, line_width);
        total_height += line_height + m_padding;
        assert(!util::is_nan(total_width));
    }
    if (m_title_visible) {
        total_height += title_height() + m_padding;
        total_width = std::max(total_width, m_title.width() + m_padding*2.f);
        assert(!util::is_nan(total_width));
    }
    if (!m_widgets.empty()) {
        // padding for borders + padding on end
        // during normal iteration we include only one
        total_width  += m_padding*3.f;
        total_height += m_padding*3.f;
    }
    return VectorF(total_width, total_height);
}

/* private */ bool Frame::is_horizontal_spacer
    (const Widget * widget) const
{
    if (m_horz_spacers.empty()) return false;
    return !(widget < &m_horz_spacers.front() || widget > &m_horz_spacers.back());
}

/* private */ bool Frame::is_line_seperator(const Widget * widget) const
    { return widget == &m_the_line_seperator; }

/* private */ float Frame::get_widget_advance(const Widget * widget_ptr) const {
    bool is_special_widget = is_line_seperator(widget_ptr) ||
                             is_horizontal_spacer(widget_ptr);
    //bool next_is_special_widget
    return widget_ptr->width() + (is_special_widget ? 0.f : m_padding);
}

/* private */ void Frame::issue_auto_resize() {
    // ignore auto resize if the frame as a width/height already set
    for (Widget * widget_ptr : m_widgets)
        widget_ptr->issue_auto_resize();
    if (width() == 0.f || height() == 0.f) {
        auto size_ = compute_size_to_fit();
        set_size(size_.x, size_.y);
    }
}

/* private */ float Frame::title_height() const
    { return float(m_title.character_size()*2); }

/* private */ void Frame::check_invarients() const {
    assert(!util::is_nan(width ()) && width () >= 0.f);
    assert(!util::is_nan(height()) && height() >= 0.f);
}

/* private */ void Frame::update_horizontal_spacers() {
    const float horz_space_c = m_widget_body.width();
    const float start_x_c    = 0.f;
    assert(horz_space_c >= 0.f);
    // horizontal spacers:
    // will have to find out how much horizontal space is available per line
    // first. Next, if there are any horizontal spacers, each of them carries
    // an equal amount of the left over space.
    float x = start_x_c;
    float pad_fix = 0.f;
    auto line_begin = m_widgets.begin();
    for (auto itr = m_widgets.begin(); itr != m_widgets.end(); ++itr) {
        Widget * widget_ptr = *itr;
        assert(widget_ptr);
        // if the widget follow another non spacer is a spacer than no
        // padding is added
        if (is_horizontal_spacer(widget_ptr)) {
            x += pad_fix;
            pad_fix = 0.f;
            continue;
        }
        pad_fix = -m_padding;
        float horz_step = get_widget_advance(widget_ptr);

        // horizontal overflow or end of widgets
        if (x + horz_step > horz_space_c || is_line_seperator(widget_ptr)) {
            // at the end of the line, set the widths for the horizontal
            // spacers
            line_begin = set_horz_spacer_widths
                (line_begin, itr, std::max(horz_space_c - x, 0.f), m_padding);

            // advance to new line
            x = start_x_c;
            pad_fix = 0.f;
        } // end of horizontal overflow handling

        // horizontal advance
        x += horz_step;
    } // looping through widgets

    if (line_begin == m_widgets.end()) return;

    set_horz_spacer_widths
        (line_begin, m_widgets.end(), std::max(horz_space_c - x, 0.f), m_padding);
}

/* private */ void Frame::update_drag_position(int drect_x, int drect_y) {
    set_location(drect_x - m_padding, drect_y - m_padding);
    update_geometry();
}

// called by update_geometry
/* private */ void Frame::update_head_and_back_geometry() {
    const auto loc = location();
    auto w = m_back.width();
    auto h = m_back.height();
    const float title_bar_height_c =
        m_title_visible ? title_height() : 0.f;
    const float title_bar_pad_c = m_title_visible ? m_outer_padding : 0.f;
    if (m_title_visible) {
        m_title_bar.set_position(loc.x + m_outer_padding, loc.y + m_outer_padding);
        m_title_bar.set_size(w - m_outer_padding*2.f, title_bar_height_c);
        update_title_geometry(loc, m_title_bar, &m_title);
    }
    m_widget_body.set_position
        (loc.x + m_outer_padding,
         loc.y + title_bar_height_c + m_outer_padding + title_bar_pad_c);
    m_widget_body.set_size
        (w - m_outer_padding*2.f,
         h - (title_bar_height_c + m_outer_padding*2.f + title_bar_pad_c));
}

/* private */ Frame::WidgetItr Frame::set_horz_spacer_widths
    (WidgetItr beg, WidgetItr end, float left_over_space, float padding)
{
    assert(left_over_space >= 0.f);
    int horz_spacer_count = 0;
    for (auto jtr = beg; jtr != end; ++jtr) {
        if (is_horizontal_spacer(*jtr))
            ++horz_spacer_count;
    }

    // if no horizontal spacers, space cannot be split up
    if (horz_spacer_count == 0) return end;

    // distribute left over space to spacers
    float width_per_spacer = (left_over_space / horz_spacer_count) - padding;
    width_per_spacer = std::max(0.f, width_per_spacer);
    for (auto jtr = beg; jtr != end; ++jtr) {
        if (!is_horizontal_spacer(*jtr)) continue;
        auto horz_spacer = dynamic_cast<HorizontalSpacer *>(*jtr);
        assert(horz_spacer);
        // always move on the next state
        horz_spacer->set_width(width_per_spacer);
    }

    return end;
}

// <----------------------- Private types for frame -------------------------->
// implemenation here, to keep vtable out of every translation unit
// (revealed by clang static analysis)

// method used as anchor for LineSeperator
Frame::LineSeperator::~LineSeperator() {}

void Frame::HorizontalSpacer::set_location(float x_, float y_)
    { m_location = VectorF(x_, y_); }

VectorF Frame::HorizontalSpacer::location() const
    { return m_location; }

float Frame::HorizontalSpacer::width() const
    { return m_width; }

void Frame::HorizontalSpacer::set_width(float w) {
    assert(w >= 0.f);
    m_width = w;
}

// ----------------------------------------------------------------------------

// anchor vtable for clang
SimpleFrame::~SimpleFrame() {}

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

bool mouse_is_in(MouseButtonEvent mouse, const DrawRectangle & drect) {
    return mouse.x >= drect.x() && mouse.x <= drect.x() + drect.width () &&
           mouse.y >= drect.y() && mouse.y <= drect.y() + drect.height();
}

} // end of <anonymous> namespace
