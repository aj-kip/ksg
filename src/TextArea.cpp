/****************************************************************************

    File: TextArea.cpp
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

#include <ksg/TextArea.hpp>
#include <ksg/TextButton.hpp>
#include <ksg/Visitor.hpp>

#include <array>

#include <cassert>

using VectorF = ksg::Widget::VectorF;

namespace ksg {

/* static */ const char * const TextArea::TEXT_COLOR = "text-area-text-color";
/* static */ const char * const TextArea::TEXT_SIZE  = "text-area-text-size" ;

void set_if_present(Text & text, const StyleMap & smap, const char * font_field,
                    const char * char_size_field, const char * text_color)
{
    StyleFinder sfinder(smap);
    sfinder.call_if_found<const sf::Font *>
        (font_field, [&](const sf::Font * ptr) { text.assign_font(ptr); });
    sfinder.call_if_found<sf::Color>
        (text_color, [&](sf::Color color){ text.set_color(color); });
    sfinder.call_if_found<float>
        (char_size_field, [&](float num) { text.set_character_size(int(num)); });
}

// ----------------------------------------------------------------------------

TextArea::TextArea() {}

void TextArea::process_event(const sf::Event &){}

void TextArea::set_location(float x, float y) {
    m_bounds.left = x;
    m_bounds.top  = y;
    m_draw_text.set_location(x, y);
    recompute_geometry();
}

VectorF TextArea::location() const
    { return VectorF(m_bounds.left, m_bounds.top); }

float TextArea::width() const {
    return (m_bounds.width == 0.f) ? m_draw_text.width() : m_bounds.width;
}

float TextArea::height() const {
    return (m_bounds.height == 0.f) ? m_draw_text.height() : m_bounds.height;
}

void TextArea::set_style(const StyleMap & smap) {
    set_if_present(m_draw_text, smap,
                   Frame::GLOBAL_FONT, TEXT_SIZE, TEXT_COLOR);
    recompute_geometry();
}

void TextArea::accept(Visitor & visitor)
    { visitor.visit(*this); }

void TextArea::accept(const Visitor & visitor) const
    { visitor.visit(*this); }

void TextArea::issue_auto_resize() {
    m_draw_text.relieve_size_limit();
    recompute_geometry();
}

void TextArea::set_text(const UString & str) {
    UString temp(str);
    m_draw_text.swap_string(temp);
    recompute_geometry();
}

void TextArea::set_character_size(int size_) {
    m_draw_text.set_character_size(size_);
    recompute_geometry();
}

void TextArea::set_width(float w) {
    m_bounds.width = w;
    m_draw_text.set_limiting_dimensions(m_bounds.width, m_bounds.height);
    recompute_geometry();
}

void TextArea::set_height(float h) {
    m_bounds.height = h;
    m_draw_text.set_limiting_dimensions(m_bounds.width, m_bounds.height);
    recompute_geometry();
}

/* protected */ void TextArea::draw
    (sf::RenderTarget & target, sf::RenderStates) const
{
    target.draw(m_draw_text);
}

/* private */ void TextArea::recompute_geometry() {}

} // end of ksg namespace
