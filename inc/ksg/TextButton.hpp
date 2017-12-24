/****************************************************************************

    File: TextButton.hpp
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

#pragma once

#include <ksg/Button.hpp>
#include <ksg/Text.hpp>

namespace ksg {

class TextButton final : public Button {
public:
    using UString = Text::UString;

    static constexpr const char * const TEXT_COLOR = "text-button-text-color";
    static constexpr const char * const TEXT_SIZE = "text-button-text-size";

    TextButton();

    void swap_string(UString & str);

    void set_string(const UString & str);

    void set_style(const StyleMap &) override;

    void set_location(float x, float y) override;

    void accept(Visitor &) override;

    void accept(const Visitor &) const override;

    void issue_auto_resize() override;

private:

    /** Sets the maximum size of the text button.
     *  @param w width in pixels
     *  @param h height in pixels
     */
    void set_size_back(float w, float h) override;

    void update_text_geometry(float w, float h);

    void draw(sf::RenderTarget & target, sf::RenderStates) const override;

    void on_size_changed(float, float) override;

    void update_string_position();

    Text m_text;
};

} // end of ksg namespace
