/****************************************************************************

    File: DrawTriangle.cpp
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

#include <ksg/DrawTriangle.hpp>

#include <SFML/Graphics/RenderTarget.hpp>

namespace {

using VectorF = DrawTriangle::VectorF;

const int POINT_A = 0;
const int POINT_B = 1;
const int POINT_C = 2;

} // end of <anonymous> namesoace

void DrawTriangle::set_point_a(float x, float y)
    { m_verticies[POINT_A].position = VectorF(x, y); }

void DrawTriangle::set_point_b(float x, float y)
    { m_verticies[POINT_B].position = VectorF(x, y); }

void DrawTriangle::set_point_c(float x, float y)
    { m_verticies[POINT_C].position = VectorF(x, y); }

void DrawTriangle::move(VectorF r) {
    for (sf::Vertex & v : m_verticies)
        v.position += r;
}

void DrawTriangle::set_location(VectorF r) { move(r - location()); }

void DrawTriangle::set_location(float x, float y)
    { move(VectorF(x, y) - location()); }

VectorF DrawTriangle::point_a() const
    { return m_verticies[POINT_A].position; }

VectorF DrawTriangle::point_b() const
    { return m_verticies[POINT_C].position; }

VectorF DrawTriangle::point_c() const
    { return m_verticies[POINT_C].position; }

VectorF DrawTriangle::location() const {
    VectorF loc;
    for (const sf::Vertex & v : m_verticies)
        loc += v.position;
    return loc;
}

sf::Color DrawTriangle::color() const { return m_verticies[POINT_A].color; }

void DrawTriangle::set_color(sf::Color color_) {
    for (sf::Vertex & v : m_verticies)
        v.color = color_;
}

/* private */ void DrawTriangle::draw
    (sf::RenderTarget & target, sf::RenderStates) const
    { target.draw(&*m_verticies.begin(), m_verticies.size(), sf::Triangles); }
