/****************************************************************************

    File: Text.cpp
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

#include <ksg/Text.hpp>
#include <common/Util.hpp>

#include "DrawCharacter.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

#include <iostream>
#include <array>
#include <memory>

#include <cassert>

using UChar                  = ksg::Text::UString::value_type;
using LineBreakList          = std::vector<int>;
using VertexContainer        = std::vector<sf::Vertex>;
using DrawCharacterContainer = std::vector<ksg::DrawCharacter>;
using UString                = ksg::Text::UString;

namespace ksg {

namespace {

/** The purpose of this class is to provide an interface to the word
 *  wrapping algorithm, defined as a private static function.
 *  This is a means to simplify that algorithm by restricting access via
 *  encapsulation.
 */
class WrapInfo {
public:

    WrapInfo(const DrawCharacterContainer & draw_characters_,
             const UString & str_, float line_width_):
        m_draw_characters_ptr(&draw_characters_), m_str_ptr(&str_),
        m_line_width(line_width_) {}

    UChar char_identity(int index) const
        { return (*m_str_ptr)[std::size_t(index)]; }

    float char_advance(int index) const
        { return (*m_draw_characters_ptr)[std::size_t(index)].advance(); }

    float line_width() const
        { return m_line_width; }

    int char_count() const
        { return int(m_str_ptr->length()); }

    void swap_line_breaks(LineBreakList & breaks)
        { m_line_breaks.swap(breaks); }

private:
    LineBreakList m_line_breaks;
    const DrawCharacterContainer * m_draw_characters_ptr;
    const UString * m_str_ptr;
    float m_line_width;
};

std::out_of_range make_invalid_index_error(const char * preface);

bool relief_should_update(float * dim);

#ifdef MACRO_COMPILER_CLANG
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wfloat-equal"
#endif

bool is_sentinel(float a, float b) { return a == b; }

#ifdef MACRO_COMPILER_CLANG
#   pragma clang diagnostic pop
#endif

DrawCharacter * get_draw_character
    (std::vector<DrawCharacter> * chars_uptr, int index);

} // end of <anonymous> namespace

Text::Text():
    m_string(),
    m_draw_characters_uptr(nullptr),
    m_end_visible_char_index(0),
    m_font_ptr(nullptr),
    m_character_size(0),
    m_width_limit (std::numeric_limits<float>::infinity()),
    m_height_limit(std::numeric_limits<float>::infinity()),
    m_bounds()
{
    m_dbounds.set_color(sf::Color(64, 0, 0));
}

Text::Text(const Text & lhs):
    m_string                (lhs.m_string),
    m_draw_characters_uptr  (nullptr),
    m_end_visible_char_index(lhs.m_end_visible_char_index),
    m_font_ptr              (lhs.m_font_ptr),
    m_character_size        (lhs.m_character_size),
    m_color                 (lhs.m_color),
    m_breaklist             (lhs.m_breaklist),
    m_width_limit           (lhs.m_width_limit),
    m_height_limit          (lhs.m_height_limit),
    m_bounds                (lhs.m_bounds),
    m_dbounds               (lhs.m_dbounds)
{
    if (lhs.m_draw_characters_uptr) {
        m_draw_characters_uptr = new DrawCharacterArray(*lhs.m_draw_characters_uptr);
    }
    check_invarients();
}

Text::Text(Text && lhs):
    Text()
    { swap(lhs); }

Text & Text::operator = (const Text & lhs) {
    if (&lhs != this) {
        Text temp(lhs);
        swap(temp);
    }
    return *this;
}

Text & Text::operator = (Text && lhs) {
    swap(lhs);
    return *this;
}

Text::~Text() {
    if (m_draw_characters_uptr)
        delete m_draw_characters_uptr;
}

void Text::swap_string(UString & str) {
    m_string.swap(str);
    if (!m_string.empty()) {
        if (!m_draw_characters_uptr)
            m_draw_characters_uptr = new DrawCharacterArray;
        m_draw_characters_uptr->clear();
        m_draw_characters_uptr->resize(m_string.size(), DrawCharacter());
    }
    m_end_visible_char_index = 0;
    update_geometry();
    check_invarients();
}

void Text::set_limiting_dimensions(float w, float h) {
    static constexpr const char * const INVALID_VALUES_MSG =
        "Text::set_limiting_dimensions: Width and height must be positive "
        "real numbers or infinity (for no limit).";

    auto is_valid = [](float x) {
        return x > 0.f && !util::is_nan(x) &&
               !is_sentinel(x, -std::numeric_limits<float>::infinity());
    };

    if (!is_valid(w) || !is_valid(w))
        throw std::invalid_argument(INVALID_VALUES_MSG);

    m_width_limit  = w;
    m_height_limit = h;
    m_dbounds.set_size(w, h);
    update_geometry();
    check_invarients();
}

void Text::set_limiting_dimensions(decltype(NO_SIZE_LIMIT)) {
    set_limiting_dimensions
        (std::numeric_limits<float>::infinity(),
         std::numeric_limits<float>::infinity());
}

void Text::relieve_width_limit() {
    if (relief_should_update(&m_width_limit))
        update_geometry();
}

void Text::relieve_height_limit() {
    if (relief_should_update(&m_height_limit))
        update_geometry();
}

void Text::relieve_size_limit() {
    if (relief_should_update(&m_height_limit) or
        relief_should_update(&m_width_limit )   )
    {
        update_geometry();
    }
}

void Text::set_character_size(int size) {
    m_character_size = size;
    update_geometry();
    check_invarients();
}

void Text::set_color(sf::Color color_) {
    m_color = color_;
    if (!m_draw_characters_uptr) return;
    for (DrawCharacter & dc : *m_draw_characters_uptr)
        dc.set_color(color_);
    check_invarients();
}

void Text::set_location(float x, float y) {
    VectorF offset = VectorF(x, y) - location();
    if (!m_draw_characters_uptr) return;
    for (DrawCharacter & dc : *m_draw_characters_uptr)
        dc.move(offset.x, offset.y);
    m_dbounds.set_position(m_bounds.left = x, m_bounds.top = y);
    check_invarients();
}

void Text::assign_font(const sf::Font * font_ptr) {
    m_font_ptr = font_ptr;
    update_geometry();
    check_invarients();
}

void Text::set_color_for_character(int index, sf::Color clr) {
    check_invarients();
    if (index >= int(m_string.length()))
        throw make_invalid_index_error("Text::set_color_for_index");
    (*m_draw_characters_uptr)[std::size_t(index)].set_color(clr);
    check_invarients();
}

Text::VectorF Text::character_location(int index) const {
    check_invarients();
    const auto * dc = get_draw_character(m_draw_characters_uptr, index);
    if (dc) return dc->location();
    throw make_invalid_index_error("Text::character_location");
}

float Text::character_width (int index) const {
    check_invarients();
    const auto * dc = get_draw_character(m_draw_characters_uptr, index);
    if (dc) return dc->width() + dc->advance();
    throw make_invalid_index_error("Text::character_width");
}

float Text::character_height(int index) const {
    check_invarients();
    const auto * dc = get_draw_character(m_draw_characters_uptr, index);
    if (dc) return dc->width() + dc->height();
    throw make_invalid_index_error("Text::character_height");
}

const sf::Font & Text::assigned_font() const {
    check_invarients();
    if (!m_font_ptr) {
        throw std::runtime_error("Text::assigned_font: Cannot access font, "
                                 "Text does not have a font assigned to it.");
    }
    return *m_font_ptr;
}

void Text::swap(Text & lhs) {
    m_string.swap(lhs.m_string);
    std::swap(m_draw_characters_uptr  , lhs.m_draw_characters_uptr);
    std::swap(m_end_visible_char_index, lhs.m_end_visible_char_index);
    std::swap(m_font_ptr              , lhs.m_font_ptr);
    std::swap(m_character_size        , lhs.m_character_size);
    std::swap(m_color                 , lhs.m_color);
    m_breaklist.swap(lhs.m_breaklist);
    std::swap(m_width_limit           , lhs.m_width_limit);
    std::swap(m_height_limit          , lhs.m_height_limit);
    std::swap(m_bounds                , lhs.m_bounds);
    std::swap(m_dbounds               , lhs.m_dbounds);
    check_invarients();
    lhs.check_invarients();
}

/* private */ void Text::draw
    (sf::RenderTarget & target, sf::RenderStates states) const
{
    // no font set
    if (!m_font_ptr) return;

    // no string to render
    if (m_string.empty()) return;
    assert(m_draw_characters_uptr);

    // draw the m_dbounds to get a visual of the text bounds
    // target.draw(m_dbounds);

    states.texture = &m_font_ptr->getTexture(unsigned(m_character_size));
    assert(m_draw_characters_uptr->size() < MAX_STRING_LEN);
    assert(m_end_visible_char_index > 0);
    int end_vis_char = m_end_visible_char_index;
    // enforce const correctness
    const DrawCharacterArray & draw_characters = *m_draw_characters_uptr;
    for (std::size_t i = 0; true; ++i) {
        // loop condition
        if (i == draw_characters.size() or int(i) == end_vis_char)
            break;
        if (m_string[i] == U'\n')
            continue;
        target.draw(draw_characters[i], states);

    }
}

/* private */ void Text::update_vertex_sizes() {
    assert(m_font_ptr);
    assert(m_character_size != 0);
    assert(!m_string.empty());
    assert(m_draw_characters_uptr);

    for (std::size_t i = 0; i != m_string.size(); ++i) {
        // bolding not supported (yet :3)
        sf::Glyph glyph = m_font_ptr->getGlyph
            (sf::Uint32(m_string[i]), unsigned(m_character_size), false);

        (*m_draw_characters_uptr)[i] = DrawCharacter(glyph, m_color);
    }
}

static bool line_break_list_in_order(const std::vector<int> & line_breaks);

using ConstStringItr = std::u32string::const_iterator;

/** Calculates the maximum height of any character found in [beg end).
 *  @param beg The first character in the sequence
 *  @param end One past the end of the character sequence (consistent with the STL)
 *  @param font SFML Font being used to render the characters.
 *  @param char_size The size of the characters
 *  @return Returns in pixels the height of the tallest character.
 */
static float max_char_height
    (ConstStringItr beg, ConstStringItr end, const sf::Font & font,
     int char_size);

/* private */ void Text::update_vertex_positions()
{
    assert(line_break_list_in_order(m_breaklist));

    assert(m_font_ptr);
    assert(m_character_size != 0);
    assert(!m_string.empty());

    // make sure breaklist requirements met for assumptions needed by this
    // function
    if (m_breaklist.empty())
        m_breaklist.push_back(int(m_string.length()));
    if (m_breaklist.back() != int(m_string.length()))
        m_breaklist.push_back(int(m_string.length()));

    assert(!m_breaklist.empty());
    assert(m_breaklist.back() == int(m_string.size()));

    VectorF write_pos = location();
    const VectorF BOUNDRY = location() + VectorF(m_width_limit, m_height_limit);
    const float LINE_HEIGHT = m_font_ptr->getLineSpacing(unsigned(m_character_size));

    // we will destroy the line breaks list, reverse it so back comes first
    std::reverse(m_breaklist.begin(), m_breaklist.end());

    // initial hieght...
    write_pos.y += max_char_height
        (m_string.begin(), m_string.begin() + m_breaklist.back(), *m_font_ptr,
         m_character_size);

    assert(m_string.length() < std::numeric_limits<int>::max());
    for (int i = 0; i != int(m_string.length()); ++i) {
        // draw will render [0 m_end...) characters
        m_end_visible_char_index = i + 1;
        // regular character positioning, verticies for this quad
        DrawCharacter & dc = *get_draw_character(m_draw_characters_uptr, i);

        // conditions for skipping/not rendering characters
        if (!m_breaklist.empty())
        {
            if (i == m_breaklist.back()) {
                // start writing next line
                write_pos = VectorF(location().x, write_pos.y + LINE_HEIGHT);
                if (write_pos.y > BOUNDRY.y)
                    break; // out of char index loop
                m_breaklist.pop_back();
                dc.set_color(sf::Color(0, 0, 0, 0));
                continue;
            }
        }
        else if (   write_pos.x > BOUNDRY.x
                 || dc.location().x + dc.width() >= BOUNDRY.x)
        {
            dc.set_color(sf::Color(0, 0, 0, 0));
            write_pos = VectorF(location().x, write_pos.y + LINE_HEIGHT);
            continue;
        }

        // === in char indexing loop ===

        // move to proper position
        dc.move(write_pos.x, write_pos.y);
        trim_char_quad_and_update_bounds(dc);

        write_pos.x += dc.advance();
    } // end of char indexing loop
    m_breaklist.clear();
}

void Text::trim_char_quad_and_update_bounds(DrawCharacter & dc) {

    const VectorF BOUNDRY = location() + VectorF(m_width_limit, m_height_limit);
    const float dc_right = dc.location().x + dc.width();
    if (dc_right - m_bounds.left > BOUNDRY.x) {
        m_bounds.width = m_width_limit;
        dc.cut_on_right(BOUNDRY.x);
    } else if (dc_right - m_bounds.left > m_bounds.width) {
        m_bounds.width = dc_right - m_bounds.left;
    }

    // trim bottom off
    const float dc_bottom = dc.location().y + dc.height();
    if (dc_bottom - m_bounds.top > BOUNDRY.y) {
        m_bounds.height = m_height_limit;
        dc.cut_on_bottom(BOUNDRY.y);
    } else if (dc_bottom - m_bounds.top > m_bounds.height) {
        m_bounds.height = dc_bottom - m_bounds.top;
    }
}

static void do_greedy_wrapping(WrapInfo & nfo);

/* private */ void Text::update_geometry()
{
    // can our verticies have sizes, do we even have verticies?
    if (!is_ready_for_geometry_update())
        return;

    m_bounds.width = m_bounds.height = 0.f;

    update_vertex_sizes();
    WrapInfo cnfo(*m_draw_characters_uptr, m_string, m_width_limit);
    m_breaklist.clear();
    cnfo.swap_line_breaks(m_breaklist);
    do_greedy_wrapping(cnfo); // wrapping algorithm
    cnfo.swap_line_breaks(m_breaklist);

    update_vertex_positions();

    m_dbounds.set_position(m_bounds.left, m_bounds.top);
    m_dbounds.set_size(m_bounds.width, m_bounds.height);
}

bool Text::is_ready_for_geometry_update() const {
    return (m_font_ptr && m_character_size != 0 && !m_string.empty() &&
            m_draw_characters_uptr);
}

void Text::check_invarients() const {
    if (m_draw_characters_uptr)
        assert(m_string.length() == m_draw_characters_uptr->size());
    else
        assert(m_string.empty());
    assert(m_string.length() <= MAX_STRING_LEN);
}

// <------------------------ rj::gui::Text Helper ---------------------------->

static bool line_break_list_in_order(const std::vector<int> & line_breaks) {
    if (line_breaks.empty()) return true;
    for (std::size_t i = 1; i != line_breaks.size(); ++i) {
        if (line_breaks[i] < line_breaks[i - 1])
            return false;
    }
    return true;
}

static bool is_space(UChar chr)
    { return (chr == ' ' || chr == '\t' || chr == '\r'); }

static void do_greedy_wrapping(WrapInfo & nfo) {

    LineBreakList line_breaks;
    bool in_word = false;
    int word_end = -1;
    float current_width_accum = 0.f;

    for (int i = 0; i != nfo.char_count(); ++i) {

        if (nfo.char_identity(i) == UChar('\n')) {
            // is hard line break
            line_breaks.push_back(i);
            current_width_accum = 0.f;
        } else if (current_width_accum > nfo.line_width()) {
            // is soft line break
            if (word_end == -1) {
                // no other word on this line
                line_breaks.push_back(i);
            } else {
                line_breaks.push_back(word_end);
                i = word_end;
            }
            current_width_accum = 0.f;
            word_end = -1;
        } else if (is_space(nfo.char_identity(i))) {
            // is whitespace
            if (in_word) {
                word_end = i;
            }
            in_word = false;
        } else {
            // is printable character
            in_word = true;
        }
        current_width_accum += nfo.char_advance(i);
    }
    nfo.swap_line_breaks(line_breaks);
}

static float max_char_height
    (ConstStringItr beg, ConstStringItr end, const sf::Font & font,
     int char_size)
{
    assert(char_size > 0);
    // adjust for max possible height for characters
    float max_height = 0.f;
    for (auto itr = beg; itr != end; ++itr) {
        // get glyph
        sf::Glyph glyph = font.getGlyph
            (sf::Uint32(*itr), unsigned(char_size), false);
        if (glyph.bounds.height > max_height)
            max_height = glyph.bounds.height;
    }
    return max_height;
}

namespace {

std::out_of_range make_invalid_index_error(const char * preface) {
    return std::out_of_range(std::string(preface) +
                             ": index exceeds length of the string.");
}

bool relief_should_update(float * dim) {
    const float max_ = std::numeric_limits<float>::max();
    if (is_sentinel(*dim, max_)) {
        return false;
    }
    *dim = max_;
    return true;
}

DrawCharacter * get_draw_character
    (std::vector<DrawCharacter> * chars_uptr, int index)
{
    if (chars_uptr) {
        if (index < int(chars_uptr->size()) and index >= 0)
            return &(*chars_uptr)[std::size_t(index)];
    }
    return nullptr;
}

} // end of <anonymous> namespace

} // end of ksg namespace
