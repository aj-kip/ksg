/****************************************************************************

    File: DrawCharacter.hpp
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

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Glyph.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics/Texture.hpp>

#include <common/FixedLengthArray.hpp>

namespace ksg {

class Text;

/** @brief IMPLEMENTATION DETAIL <not for client use>
 *
 */
class DrawCharacter final : public sf::Drawable {
public:
    using VectorF = sf::Vector2f;

    DrawCharacter();

    DrawCharacter(const sf::Glyph & glyph, sf::Color clr);

    void set_color(sf::Color clr);

    float width() const;

    float height() const;

    float advance() const;

    /** Cuts off part of a quad by changing it's right verticies. A cut is made by
     *  changing positions and the texture rectangle's coordinates to give a
     *  "cut-off" effect.
     *  @param tr Top-right coordinate of the quad.
     *  @param br Bottom-right coordinate of the quad.
     *  @param any_left Top or Bottom left of the quad.
     *  @param cut_line X-coordinate where the cut is made.
     */
    void cut_on_right(float cut_line);

    void cut_on_bottom(float cut_line);

    void set_location(float x, float y);

    VectorF location() const;

    void move(float x, float y);

protected:
    void draw(sf::RenderTarget & target, sf::RenderStates states) const override;

private:
    FixedLengthArray<sf::Vertex, 4> m_verticies;
    float m_advance;
};

} // end of ksg namespace
