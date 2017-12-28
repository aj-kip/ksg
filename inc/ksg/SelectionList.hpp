/****************************************************************************

    File: ProgressBar.hpp
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
#if 0
#pragma once

#include <ksg/Widget.hpp>

namespace ksg {

/**
 * @brief The SelectionList class
 *  A vertically arranged list of scrollable texts.
 *  Overflow:
 *  In the event that the lists of texts cannot fit the assigned size, this
 *  widget will go into vertical overflow mode. In this mode the text will
 *  scroll to position n% of the entire list where n%
 *
 *  Features:
 */
template <typename T>
class SelectionList final : public Widget {
public:
    enum { VERTICAL, HORIZONTAL };

    void process_event(const sf::Event &) override;

    void set_location(float x, float y) override;

    VectorF location() const override;

    float width() const override;

    float height() const override;

    void set_style(const StyleMap &) override;

    void set_orientation(decltype(VERTICAL));
};

} // end of ksg namespace
#endif
