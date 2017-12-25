/****************************************************************************

    File: OptionsSlider.hpp
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

#include <common/DrawRectangle.hpp>

#include <ksg/Widget.hpp>
#include <ksg/ArrowButton.hpp>
#include <ksg/Text.hpp>

namespace ksg {

/**
 *  For now, assumes horizontal oreintation...
 */
class OptionsSlider final : public Widget {
public:
    using UString = Text::UString;
    using BlankFunctor = ArrowButton::BlankFunctor;

    OptionsSlider();

    void process_event(const sf::Event & evnt) override;

    void set_location(float x, float y) override;

    VectorF location() const override;

    float width() const override;

    float height() const override;

    void set_style(const StyleMap &) override;

    void accept(Visitor &) override;

    void accept(const Visitor &) const override;

    void set_size(float w, float h);

    void swap_options(std::vector<UString> & options);

    void select_option(std::size_t index);

    std::size_t selected_option_index() const;

    const UString & selected_option() const;

    std::size_t options_count() const
        { return m_options.size(); }

    void set_option_change_event(BlankFunctor && func);

private:
    void draw(sf::RenderTarget & target, sf::RenderStates) const override;

    void issue_auto_resize() override;

    void recenter_text();

    bool is_horizontal() const
        { return width() >= height(); }

    bool is_vertical() const
        { return !is_horizontal(); }

    ArrowButton m_left_arrow;
    ArrowButton m_right_arrow;
    DrawRectangle m_back;
    DrawRectangle m_front;
    Text m_text;
    std::vector<UString> m_options;
    std::size_t m_selected_index;
    float m_padding;
    sf::Vector2f m_size;
    BlankFunctor m_press_func;
};

} // end of ksg namespace
