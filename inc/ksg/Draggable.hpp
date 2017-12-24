/****************************************************************************

    File: Draggable.hpp
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

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Window/Event.hpp>

#include <common/DrawRectangle.hpp>

namespace ksg {

/** @brief Any on-screen object that can be dragged by a mouse.
 *
 *  Draggable provides the "draggable" feature for inheriting classes. In order
 *  for this to work the child class must have a rectangle object to define the
 *  bounds which can be dragged. Functions (as well as keeping track of a
 *  current state) are provided to start a drag (mouse_click), update a drag's
 *  position (mouse_click, update_drag_position) and end the drag
 *  (drag_release).
 */
class Draggable {
protected:
    using MouseButton = sf::Mouse::Button;

    Draggable(): m_dragged(false) {}
    virtual ~Draggable();

    /** @brief mouse_move updates the Draggable with the current mouse
     *  position.
     *
     *  If the Draggable is in the drag state, "update_drag_position" will be
     *  called, otherwise this funtion does nothing.
     *
     *  @param x X-coordinate of the current mouse position.
     *  @param y Y-coordinate of the current mouse position.
     */
    void mouse_move(int x, int y);

    /** @brief mouse_click Handles mouse click events.
     *
     *  If a mouse click were to take place within the given rectangle, this
     *  object enters a drag state. Any additional mouse movements will cause
     *  this object to follow the cursor until drag_release is called.
     *
     *  @param x X-coordinate of the current mouse position.
     *  @param y Y-coordinate of the current mouse position.
     *  @param drect The given rectangle.
     *  @return Returns true if the object has entered the drag state.
     */
    bool mouse_click(int x, int y, const DrawRectangle & drect);

    /** @brief drag_release Causes object to leave its drag state.
     */
    void drag_release()
        { m_dragged = false; }

    /** Constrains the draggable's positions, as it changes from mouse_move.
     *  @param area rectangle which this object's position may not go outside
     *              of
     *  @note Set constraints will not take size into account, so this will
     *        have to be subtracted from the area parameter.
     */
    void set_position_contraints(sf::IntRect area);

    /** Allows the draggable to be dragged to any position. */
    void remove_position_contraints();

    /** When this function is called, it is provided the new coordinates where
     *  the rectangle object that was used when calling mouse_click(), should
     *  be placed now.
     *  @see mouse_move()
     *  @param drect_x New "should-be" X-coordinate of the rectangle object.
     *  @param drect_y New "should-be" Y-coordinate of the rectangle object.
     */
    virtual void update_drag_position(int drect_x, int drect_y) = 0;

private:

    static bool is_in_rect(int x, int y, const DrawRectangle & drect);

    bool has_position_contraints() const;

    bool m_dragged;
    sf::Vector2i m_drag_offset;
    sf::IntRect m_position_contraints;
};

} // end of ksg namespace
