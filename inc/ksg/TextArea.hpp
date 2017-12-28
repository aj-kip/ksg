/****************************************************************************

    File: TextArea.hpp
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

#include <ksg/Text.hpp>
#include <ksg/Frame.hpp>

namespace ksg {

void set_if_present(Text &, const StyleMap &, const char * font_field,
                    const char * char_size_field, const char * text_color);

/** @brief A TextArea is an invisible rectangle wrapped around some blob of
 *         text.
 */
class TextArea final : public Widget {
public:
    using UString = Text::UString;

    static constexpr const char * const TEXT_COLOR = "text-area-text-color";
    static constexpr const char * const TEXT_SIZE  = "text-area-text-size" ;

    TextArea();

    // <-------------------------- Basic Widget ------------------------------>

    void process_event(const sf::Event & evnt) override;

    void set_location(float x, float y) override;

    VectorF location() const override;

    float width() const override;

    float height() const override;

    void set_style(const StyleMap &) override;

    void accept(Visitor &) override;

    void accept(const Visitor &) const override;

    // <----------------------------- TextWidget ----------------------------->

    void set_text(const UString & str);

    const UString & text() const { return m_draw_text.string(); }

    void set_color_for_index(int index, sf::Color c)
        { m_draw_text.set_color_for_character(index, c); }

    void set_color(sf::Color c)
        { m_draw_text.set_color(c); }

    void set_character_size(int size_);

    // as command
    // too large -> auto center
    // too small -> cut off rendering/visible characters (even if there are no
    // visible characters)
    void set_width(float w);

    void set_height(float h);

    void set_size(float w, float h);

    void assign_font(const sf::Font & font)
        { return m_draw_text.assign_font(&font); }

    const sf::Font & assigned_font() const
        { return m_draw_text.assigned_font(); }

    bool has_font_assigned() const
        { return m_draw_text.has_font_assigned(); }

    int character_size() const
        { return m_draw_text.character_size(); }

protected:
    void draw(sf::RenderTarget & target, sf::RenderStates) const override;

    void issue_auto_resize() override;

private:
    void recompute_geometry();

    Text m_draw_text;
};

} // end of ksg namespace
