/****************************************************************************

    File: StyleMap.cpp
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

#include <ksg/StyleMap.hpp>
#include <ksg/Frame.hpp>
#include <ksg/Button.hpp>
#include <ksg/TextButton.hpp>
#include <ksg/TextArea.hpp>
#include <ksg/ProgressBar.hpp>
#include <ksg/SelectionMenu.hpp>

#include <stdexcept>

#include <cassert>

using Error = std::runtime_error;

namespace {

template <typename T>
void add_style(ksg::StyleMap & smap, const char * key, const T & val)
    { smap[key] = ksg::StylesField(val); }

} // end of <anonymous> namespace

namespace ksg {

namespace styles {

DrawRectangle make_rect_with_unset_color() {
    DrawRectangle rect;
    rect.set_color(get_unset_value<sf::Color>());
    return rect;
}

StyleMap construct_system_styles() {
    using sf::Color;
    StyleMap smap;

    // Global styles
    add_style(smap, k_global_padding, 5.f);

    // Frame's default styles
    add_style(smap, Frame::k_background_color , Color(0x51, 0x51, 0x76));
    add_style(smap, Frame::k_title_bar_color  , Color(0x08, 0x08, 0x22));
    add_style(smap, Frame::k_title_color      , Color::White           );
    add_style(smap, Frame::k_title_size       , 20.f                   );
    add_style(smap, Frame::k_widget_body_color, Color(0x18, 0x18, 0x40));

    // Button's default styles
    add_style(smap, Button::k_hover_back_color , Color(0x4B, 0x46, 0x15));
    add_style(smap, Button::k_hover_front_color, Color(0x77, 0x6A, 0x45));
    add_style(smap, Button::k_regular_back_color   , Color(0x4B, 0x46, 0x15));
    add_style(smap, Button::k_regular_front_color  , Color(0x30, 0x2C, 0x05));

    // Text Button's default styles
    add_style(smap, TextButton::k_text_color, Color::White);
    add_style(smap, TextButton::k_text_size , 20.f         );

    // Text Area's default styles
    add_style(smap, TextArea::k_text_color, Color::White);
    add_style(smap, TextArea::k_text_size , 18.f        );

    // Progress Bar's default styles
    add_style(smap, ProgressBar::k_inner_back_color , Color(0x40,    0,    0));
    add_style(smap, ProgressBar::k_inner_front_color, Color(0xA0, 0xA0, 0x00));
    add_style(smap, ProgressBar::k_outer_color      , Color(0x10, 0x10, 0x10));
    add_style(smap, ProgressBar::k_padding          , 2.f                    );

    detail::add_selection_menu_default_styles(smap);
    return smap;
}

StylesField load_font(const std::string & filename) {
    auto sptr = std::make_shared<sf::Font>();
    if (sptr->loadFromFile(filename)) {
        return StylesField(std::shared_ptr<const sf::Font>(sptr));
    } else {
        return StylesField();
    }
}

/* private static */ void SetIfNumericFoundPriv::set_char_size
    (Text & text, int sz)
{ text.set_character_size(sz); }

} // end of styles namespace

} // end of ksg namespace
