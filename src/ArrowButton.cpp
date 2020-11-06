/****************************************************************************

    File: ArrowButton.cpp
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

#include <ksg/ArrowButton.hpp>

#include <SFML/Graphics/RenderTarget.hpp>

#include <cassert>

namespace ksg {

ArrowButton::ArrowButton(): m_dir(Direction::k_none) {}

void ArrowButton::set_direction(Direction dir_) {
    if (m_dir == dir_) return;
    m_dir = dir_;
    update_points();
}

void ArrowButton::process_event(const sf::Event & evnt) {
    if (m_dir == Direction::k_none) return;
    Button::process_event(evnt);
}

/* private */ void ArrowButton::draw
    (sf::RenderTarget & target, sf::RenderStates) const
{
    Button::draw(target, sf::RenderStates::Default);
    if (m_dir == Direction::k_none) return;
    target.draw(m_draw_tri);
}

/* private */ void ArrowButton::on_size_changed(float, float)
    { update_points(); }

/* private */ void ArrowButton::on_location_changed(float, float)
    { update_points(); }

/* private */ void ArrowButton::update_points() {
    VectorF anchor = location() + VectorF(width()/2.f, height()/2.f);
    float offset = std::min(width() /2.f - padding()*2.f,
                            height()/2.f - padding()*2.f);
    switch (m_dir) {
    case Direction::k_down:
        m_draw_tri.set_point_a(anchor + VectorF(    0.f,  offset));
        m_draw_tri.set_point_b(anchor + VectorF(-offset, -offset));
        m_draw_tri.set_point_c(anchor + VectorF( offset, -offset));
        break;
    case Direction::k_left:
        m_draw_tri.set_point_a(anchor + VectorF(-offset,     0.f));
        m_draw_tri.set_point_b(anchor + VectorF( offset, -offset));
        m_draw_tri.set_point_c(anchor + VectorF( offset,  offset));
        break;
    case Direction::k_right:
        m_draw_tri.set_point_a(anchor + VectorF( offset,     0.f));
        m_draw_tri.set_point_b(anchor + VectorF(-offset, -offset));
        m_draw_tri.set_point_c(anchor + VectorF(-offset,  offset));
        break;
    case Direction::k_up:
        m_draw_tri.set_point_a(anchor + VectorF(    0.f, -offset));
        m_draw_tri.set_point_b(anchor + VectorF(-offset,  offset));
        m_draw_tri.set_point_c(anchor + VectorF( offset,  offset));
        break;
    case Direction::k_none:
        deselect();
        break;
    }
}

} // end of ksg namespace
