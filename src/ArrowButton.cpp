/****************************************************************************

    File: ArrowButton.cpp
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

#include <ksg/ArrowButton.hpp>
#include <ksg/Visitor.hpp>

#include <cassert>

namespace ksg {

ArrowButton::ArrowButton(): m_dir(Direction::RIGHT) {}

void ArrowButton::set_direction(Direction dir_) {
    m_dir = dir_;
    update_points();
}

void ArrowButton::accept(Visitor & visitor)
    { visitor.visit(*this); }

void ArrowButton::accept(const Visitor & visitor) const
    { visitor.visit(*this); }

/* private */ void ArrowButton::draw
    (sf::RenderTarget & target, sf::RenderStates) const
{
    Button::draw(target, sf::RenderStates::Default);
    target.draw(m_draw_tri);
}

/* private */ void ArrowButton::on_size_changed(float, float) {
    update_points();
}

/* private */ void ArrowButton::on_location_changed(float, float) {
    update_points();
}

/* private */ void ArrowButton::update_points() {
    VectorF anchor = location() + VectorF(width()/2.f, height()/2.f);
    float offset = std::min(width() /2.f - padding()*2.f,
                            height()/2.f - padding()*2.f);
    switch (m_dir) {
    case Direction::DOWN:
        m_draw_tri.set_point_a(anchor + VectorF(    0.f,  offset));
        m_draw_tri.set_point_b(anchor + VectorF(-offset, -offset));
        m_draw_tri.set_point_c(anchor + VectorF( offset, -offset));
        break;
    case Direction::LEFT:
        m_draw_tri.set_point_a(anchor + VectorF(-offset,     0.f));
        m_draw_tri.set_point_b(anchor + VectorF( offset, -offset));
        m_draw_tri.set_point_c(anchor + VectorF( offset,  offset));
        break;
    case Direction::RIGHT:
        m_draw_tri.set_point_a(anchor + VectorF( offset,     0.f));
        m_draw_tri.set_point_b(anchor + VectorF(-offset, -offset));
        m_draw_tri.set_point_c(anchor + VectorF(-offset,  offset));
        break;
    case Direction::UP:
        m_draw_tri.set_point_a(anchor + VectorF(    0.f, -offset));
        m_draw_tri.set_point_b(anchor + VectorF(-offset,  offset));
        m_draw_tri.set_point_c(anchor + VectorF( offset,  offset));
        break;
    }
}

} // end of ksg namespace
