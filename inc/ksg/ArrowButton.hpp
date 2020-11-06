/****************************************************************************

    File: ArrowButton.hpp
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

#include <ksg/Button.hpp>
#include <ksg/DrawTriangle.hpp>

namespace ksg {

/** An Arrow Button is a clickable button with an arrow facing in one of four
 *  possible directions.
 */
class ArrowButton final : public Button {
public:
    enum class Direction {
        k_up, k_down, k_right, k_left,
        k_none
    };

    ArrowButton();

    void set_direction(Direction dir_);

    void set_arrow_color(sf::Color color_)
        { m_draw_tri.set_color(color_); }

    Direction direction() const { return m_dir; }

    void process_event(const sf::Event & evnt) override;

private:
    void draw(sf::RenderTarget & target, sf::RenderStates) const override;

    void on_size_changed(float old_width, float old_height) override;

    void on_location_changed(float old_x, float old_y) override;

    void update_points();

    DrawTriangle m_draw_tri;
    Direction m_dir;
};

} // end of ksg namespace
