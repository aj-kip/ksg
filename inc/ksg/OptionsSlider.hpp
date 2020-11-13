/****************************************************************************

    File: OptionsSlider.hpp
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

    /** @brief Sets the size of the widget by setting the size of it's interior.
     *
     *  @note The size of the arrows is determined by the height. They are made
     *        into squares.
     *  @param w width of the interior of the widget, where the text lives.
     *  @param h height of the widget, this is also the height and width of
     *         each arrow.
     */
    void set_interior_size(float w, float h);

    void swap_options(std::vector<UString> & options);

    void set_options(const std::vector<UString> &);

    void set_options(std::vector<UString> &&);

    void select_option(std::size_t index);

    std::size_t selected_option_index() const;

    const UString & selected_option() const;

    std::size_t options_count() const
        { return m_options.size(); }

    void set_option_change_event(BlankFunctor && func);

    void set_wrap_enabled(bool);

private:
    void draw(sf::RenderTarget & target, sf::RenderStates) const override;

    void issue_auto_resize() override;

    void iterate_children_(ChildWidgetIterator &) override;

    void iterate_const_children_(ChildWidgetIterator &) const override;

    void recenter_text();

    bool is_horizontal() const
        { return width() >= height(); }

    bool is_vertical() const
        { return !is_horizontal(); }

    float padding() const noexcept;

    ArrowButton m_left_arrow;
    ArrowButton m_right_arrow;
    DrawRectangle m_back  = styles::make_rect_with_unset_color();
    DrawRectangle m_front = styles::make_rect_with_unset_color();
    Text m_text;
    std::vector<UString> m_options;
    std::size_t m_selected_index = 0;
    float m_padding = styles::get_unset_value<float>();
    sf::Vector2f m_size;
    BlankFunctor m_press_func = [](){};
    bool m_wrap_enabled = false;
};

} // end of ksg namespace
