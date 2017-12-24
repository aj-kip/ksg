/****************************************************************************

    File: StyleMap.cpp
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

#include <ksg/StyleMap.hpp>
#include <ksg/Frame.hpp>
#include <ksg/Button.hpp>
#include <ksg/TextButton.hpp>
#include <ksg/TextArea.hpp>
#include <ksg/ProgressBar.hpp>

#include <stdexcept>

#include <cassert>

using Error = std::runtime_error;

namespace {

std::invalid_argument make_nullptr_arg_error();

template <typename T>
void add_style(ksg::StyleMap & smap, const char * key, const T & val)
    { smap[key] = ksg::StylesField(val); }

} // end of <anonymous> namespace

namespace ksg {

StyleMap construct_system_styles() {
    using sf::Color;
    StyleMap smap;

    // Global styles
    add_style(smap, Frame::GLOBAL_PADDING, 5.f);

    // Frame's default styles
    add_style(smap, Frame::BACKGROUND_COLOR , Color(0x51, 0x51, 0x76));
    add_style(smap, Frame::TITLE_BAR_COLOR  , Color(0x08, 0x08, 0x22));
    add_style(smap, Frame::TITLE_COLOR      , Color::White           );
    add_style(smap, Frame::TITLE_SIZE       , 20.f                   );
    add_style(smap, Frame::WIDGET_BODY_COLOR, Color(0x18, 0x18, 0x40));

    // Button's default styles
    add_style(smap, Button::HOVER_BACK_COLOR , Color(0x4B, 0x46, 0x15));
    add_style(smap, Button::HOVER_FRONT_COLOR, Color(0x77, 0x6A, 0x45));
    add_style(smap, Button::REG_BACK_COLOR   , Color(0x4B, 0x46, 0x15));
    add_style(smap, Button::REG_FRONT_COLOR  , Color(0x30, 0x2C, 0x05));

    // Text Button's default styles
    add_style(smap, TextButton::TEXT_COLOR, Color::White);
    add_style(smap, TextButton::TEXT_SIZE , 20.f         );

    // Text Area's default styles
    add_style(smap, TextArea::TEXT_COLOR, Color::White);
    add_style(smap, TextArea::TEXT_SIZE , 18.f        );

    // Progress Bar's default styles
    add_style(smap, ProgressBar::INNER_BACK_COLOR , Color(0x40,    0,    0));
    add_style(smap, ProgressBar::INNER_FRONT_COLOR, Color(0xA0, 0xA0, 0x00));
    add_style(smap, ProgressBar::OUTER_COLOR      , Color(0x10, 0x10, 0x10));
    add_style(smap, ProgressBar::PADDING          , 2.f                    );
    return smap;
}

// <----------------------------- StyleFinder -------------------------------->

bool StyleFinder::set_if_found(const char * field_name, sf::Color * color_ptr) {
    if (!color_ptr)
        throw make_nullptr_arg_error();
    const auto & field = find_field_nothrow(field_name);
    if (field.is_type<sf::Color>()) {
        *color_ptr = field.as<sf::Color>();
        return true;
    }
    return false;
}

bool StyleFinder::set_if_found(const char * field_name, float * number_ptr) {
    if (!number_ptr)
        throw make_nullptr_arg_error();
    const auto & field = find_field_nothrow(field_name);
    if (field.is_type<float>()) {
        *number_ptr = field.as<float>();
        return true;
    }
    return false;
}

bool StyleFinder::set_if_found
    (const char * field_name, PointerTo<const sf::Font *>::Type font_ptr)
{
    if (!font_ptr)
        throw make_nullptr_arg_error();
    const auto & field = find_field_nothrow(field_name);
    if (field.is_type<const sf::Font *>()) {
        *font_ptr = field.as<const sf::Font *>();
        return true;
    }
    return false;
}

void StyleFinder::assign_font_if_found(const char * field_name, Text & text) {
    const sf::Font * font_ptr = nullptr;
    set_if_found(field_name, &font_ptr);
    if (font_ptr)
        text.assign_font(font_ptr);
}

/* private */ const StylesField & StyleFinder::find_field
    (const char * field_name) const
{
    assert(m_styles_ptr);
    auto itr = m_styles_ptr->find(field_name);
    if (itr == m_styles_ptr->end()) {
        throw Error(std::string("Cannot find styles field named: \"") +
                    field_name + "\"."                                 );
    }
    return itr->second;
}

/* private */ StylesField StyleFinder::find_field_nothrow
    (const char * field_name) const noexcept
{
    assert(m_styles_ptr);
    auto itr = m_styles_ptr->find(field_name);
    if (itr == m_styles_ptr->end()) {
        return StylesField();
    }
    return itr->second;
}

} // end of ksg namespace

namespace {

std::invalid_argument make_nullptr_arg_error() {
    return std::invalid_argument
        ("StyleFinder::set_if_found: This function only accepts non-null "
         "pointers.");
}

} // end of <anonymous> namespace
