/****************************************************************************

    File: Text.hpp
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

////////////////////////////////////////////////////////////
//
// SFML - Simple and Fast Multimedia Library
// Copyright (C) 2007-2015 Laurent Gomila (laurent@sfml-dev.org)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
////////////////////////////////////////////////////////////

#pragma once

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Vertex.hpp>

#include <vector>
#include <string>
#include <memory>
#include <limits>

#include <common/DrawRectangle.hpp>

namespace ksg {

class DrawCharacter;

/** The following is a rewrite/extention/retraction of Laurent Gomila's
 *  sf::Text class.
 *
 *  This class is responsible for most geometric work involving the rendering
 *  of text. Some things, like how the text fits in the bigger picture, cannot
 *  be handled by this class.
 *
 *  Features:
 *  - can restrict it's text rendering to a rectangle.
 *  - handles multi line text restricted by width.
 *  - automatic word wrapping (greedy) based on restricted width
 */
class Text final : public sf::Drawable {
public:
    using UString = std::u32string;
    using VectorF = sf::Vector2f;

    static constexpr const int MAX_STRING_LEN = std::numeric_limits<int>::max();

    enum { NO_SIZE_LIMIT };

    Text();

    Text(const Text &);
    Text(Text &&);

    Text & operator = (const Text &);
    Text & operator = (Text &&);

    ~Text() override;

    void set_string(const UString & str) {
        UString temp(str);
        swap_string(temp);
    }

    void swap_string(UString & str);

    void set_limiting_dimensions(float w, float h);

    void set_limiting_dimensions(decltype(NO_SIZE_LIMIT));

    void relieve_width_limit();

    void relieve_height_limit();

    void relieve_size_limit();

    void set_character_size(int size);

    void set_color(sf::Color color);

    void set_location(float x, float y);

    void set_location(VectorF r)
        { set_location(r.x, r.y); }

    void assign_font(const sf::Font * font_ptr);

    void set_color_for_character(int index, sf::Color clr);

    VectorF character_location(int index) const;

    float character_width (int index) const;

    float character_height(int index) const;

    VectorF location() const
        { return VectorF(m_bounds.left, m_bounds.top); }

    float width() const
        { return m_bounds.width; }

    float height() const
        { return m_bounds.height; }

    const UString & string() const
        { return m_string; }

    bool has_font_assigned() const
        { return m_font_ptr; }

    const sf::Font & assigned_font() const;

    int character_size() const
        { return m_character_size; }

    void swap(Text &);

private:

    using LineBreakList = std::vector<int>;
    using VertexContainer = std::vector<sf::Vertex>;
    using UChar = UString::value_type;
    using DrawCharacterArray = std::vector<DrawCharacter>;

    /** SFML draw, draws all verticies of the text.
     *  @param target Target of all draws.
     *  @param states States texture set, used to draw quads.
     */
    void draw(sf::RenderTarget & target, sf::RenderStates states) const override;

    /** Computes both Quad sizes as well as texture rectangles.
     *  @warning Should be called by update_geometry only.
     */
    void update_vertex_sizes();

    /** Sets positions of all Quads, trims/deletes Quads that fall out of
     *  bounds.
     *  More labor intensize than set_location, infact DO NOT call this
     *  function in set_location.
     *  @warning Should be called by update_geometry only.
     */
    void update_vertex_positions();

    /** Updates all position/sizing of the characters.
     *  This includes word wrapping and triming (what happens when 'space' runs
     *  out).
     */
    void update_geometry();

    /** Trims the given quad if necessary, also updates internal bounds
     *  information (not to be confused with the bounds maximum width/height).
     *  @warning Should be called by update_vertex_positions only.
     *  @param dc The DrawCharacter
     */
    void trim_char_quad_and_update_bounds(DrawCharacter & dc);

    bool is_ready_for_geometry_update() const;

    void check_invarients() const;

    UString m_string;

    DrawCharacterArray * m_draw_characters_uptr;
    int m_end_visible_char_index;

    // actual text rendering

    const sf::Font * m_font_ptr;
    int m_character_size;
    sf::Color m_color;

    // controls line breaks and boundries

    LineBreakList m_breaklist; // both hard and soft breaks
    float m_width_limit;
    float m_height_limit;
    sf::FloatRect m_bounds;

    DrawRectangle m_dbounds;
};

} // end of ksg namespace
