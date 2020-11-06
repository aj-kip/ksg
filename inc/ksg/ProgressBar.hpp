/****************************************************************************

    File: ProgressBar.hpp
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

namespace ksg {

class ProgressBar final : public Widget {
public:
    static constexpr const char * const k_outer_color       = "progress-bar-outer-color";
    static constexpr const char * const k_inner_front_color = "progress-bar-inner-front-color";
    static constexpr const char * const k_inner_back_color  = "progress-bar-inner-back-color";
    static constexpr const char * const k_padding           = "progress-bar-padding";

    void process_event(const sf::Event &) override;

    void set_location(float x, float y) override;

    VectorF location() const override;

    void set_size(float w, float h);

    float width() const override;

    float height() const override;

    void set_style(const StyleMap &) override;

    void set_outer_color(sf::Color color_);

    void set_inner_front_color(sf::Color color_);

    void set_inner_back_color(sf::Color color_);

    void set_fill_amount(float fill_amount);

    float fill_amount() const;

    void set_padding(float p);

    float padding() const
        { return m_padding; }

protected:
    void draw(sf::RenderTarget & target, sf::RenderStates) const override;

private:
    float active_padding() const;

    void update_positions_using_outer();

    void update_sizes_using_outer();

    DrawRectangle m_outer       = styles::make_rect_with_unset_color();
    DrawRectangle m_inner_front = styles::make_rect_with_unset_color();
    DrawRectangle m_inner_back  = styles::make_rect_with_unset_color();

    float m_fill_amount = 0.f;
    float m_padding = styles::get_unset_value<float>();
};

} // end of ksg namespace
