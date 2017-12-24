/****************************************************************************

    File: DrawCharacter.cpp
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

#include "DrawCharacter.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

namespace ksg {

namespace {

using VectorF = sf::Vector2f;

constexpr const int TOP_LEFT_VERT_OFFSET     = 0;
constexpr const int TOP_RIGHT_VERT_OFFSET    = 1;
constexpr const int BOTTOM_RIGHT_VERT_OFFSET = 2;
constexpr const int BOTTOM_LEFT_VERT_OFFSET  = 3;

template <typename T>
T mag(T t) { return (t < T(0)) ? -t : t; }

} // end of <anonymous> namespace

DrawCharacter::DrawCharacter(): m_advance(0.f) {}

DrawCharacter::DrawCharacter(const sf::Glyph & glyph, sf::Color clr):
    m_advance(0.f)
{
    using Vector2f = sf::Vector2f;

    float left   = glyph.bounds.left;
    float top    = glyph.bounds.top;
    float right  = glyph.bounds.left + glyph.bounds.width;
	float bottom = glyph.bounds.top  + glyph.bounds.height;

    // abbr: texture, left/top/right/bottom
    float trL = float(glyph.textureRect.left);
    float trT = float(glyph.textureRect.top);
    float trR = float(glyph.textureRect.left + glyph.textureRect.width);
    float trB = float(glyph.textureRect.top  + glyph.textureRect.height);

    // Add a quad for the current character
    m_verticies[0] = sf::Vertex(Vector2f(left , top   ), clr, Vector2f(trL, trT));
    m_verticies[1] = sf::Vertex(Vector2f(right, top   ), clr, Vector2f(trR, trT));
    m_verticies[2] = sf::Vertex(Vector2f(right, bottom), clr, Vector2f(trR, trB));
    m_verticies[3] = sf::Vertex(Vector2f(left , bottom), clr, Vector2f(trL, trB));

    m_advance = glyph.advance;
}

void DrawCharacter::set_color(sf::Color clr) {
    for (sf::Vertex & v : m_verticies)
        v.color = clr;
}

float DrawCharacter::width() const {
    return m_verticies[TOP_RIGHT_VERT_OFFSET].position.x -
           m_verticies[TOP_LEFT_VERT_OFFSET ].position.x;
}

float DrawCharacter::height() const {
    return m_verticies[BOTTOM_LEFT_VERT_OFFSET].position.y -
           m_verticies[TOP_RIGHT_VERT_OFFSET  ].position.y;
}

float DrawCharacter::advance() const {
    return m_advance;
}

void DrawCharacter::cut_on_right(float cut_line) {
    sf::Vertex & tr = m_verticies[TOP_RIGHT_VERT_OFFSET];
    sf::Vertex & br = m_verticies[BOTTOM_RIGHT_VERT_OFFSET];

    const sf::Vertex & any_left = m_verticies[TOP_LEFT_VERT_OFFSET];

    // if the cut line exceeds the draw character's bounds, reduce its width
    // to zero
    if (cut_line < any_left.position.x) {
        tr.position.x = br.position.x = any_left.position.x;
        return;
    }

    float cut_ratio = (cut_line - any_left.position.x)/
                      (br.position.x - any_left.position.x);
    // need to find proportion of the char's texture to cut off
    float tx_width = tr.texCoords.x - any_left.texCoords.x;
    tr.position.x = br.position.x = cut_line;
    tr.texCoords.x = br.texCoords.x = any_left.texCoords.x + tx_width*cut_ratio;
}

void DrawCharacter::cut_on_bottom(float cut_line) {
    sf::Vertex & bl = m_verticies[BOTTOM_LEFT_VERT_OFFSET];
    sf::Vertex & br = m_verticies[BOTTOM_RIGHT_VERT_OFFSET];
    const sf::Vertex & any_top = m_verticies[TOP_LEFT_VERT_OFFSET];

    // if the cut line exceeds the draw character's bounds, reduce its height
    // to zero
    if (cut_line < any_top.position.y) {
        bl.position.y = br.position.y = any_top.position.y;
        return;
    }

    float cut_ratio = (cut_line - any_top.position.y)/
                      (br.position.y - any_top.position.y);
    float tx_height = bl.texCoords.y - any_top.texCoords.y;
    br.position.y = bl.position.y = cut_line;
    br.texCoords.y = bl.texCoords.y = any_top.texCoords.y + tx_height*cut_ratio;
}

void DrawCharacter::set_location(float x, float y) {
    float w = width(), h = height();
    for (sf::Vertex & v : m_verticies)
        v.position = sf::Vector2f(x, y);
    m_verticies[TOP_RIGHT_VERT_OFFSET   ].position.x += w;
    m_verticies[BOTTOM_LEFT_VERT_OFFSET ].position.y += h;
    m_verticies[BOTTOM_RIGHT_VERT_OFFSET].position.y += h;
    m_verticies[BOTTOM_RIGHT_VERT_OFFSET].position.x += w;
}

void DrawCharacter::move(float x, float y) {
    for (sf::Vertex & v : m_verticies)
        v.position += sf::Vector2f(x, y);
}

VectorF DrawCharacter::location() const {
    return m_verticies[TOP_LEFT_VERT_OFFSET].position;
}

/* protected override */ void DrawCharacter::draw
    (sf::RenderTarget & target, sf::RenderStates states) const
{
    const sf::Vertex & tl = m_verticies[TOP_LEFT_VERT_OFFSET];
    const sf::Vertex & br = m_verticies[BOTTOM_RIGHT_VERT_OFFSET];
    if (   mag(tl.position.x - br.position.x) < 0.5f
        or mag(tl.position.y - br.position.y) < 0.5f)
        return;
    target.draw(&m_verticies[0], m_verticies.size(), sf::Quads, states);
}
} // end of ksg namespace
