/****************************************************************************

    File: Widget.hpp
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
#include <SFML/Graphics/Color.hpp>
#include <SFML/Window/Event.hpp>

#include <map>
#include <string>
#include <memory>

#include <ksg/StyleMap.hpp>

namespace sf { class Font; }

namespace ksg {

class Visitor;

/** A frame needs four things from a widget, in order to position the widget
 *  and setup the frame.
 *
 *  === COMPOSITE PATTERN ===
 *  A Widget is a component, by default add_component (and future composite
 *  functions throws exceptions).
 */
class Widget : public sf::Drawable {
public:
    using VectorF = sf::Vector2f;

    Widget();

    virtual ~Widget();

    virtual void process_event(const sf::Event &) = 0;

    virtual void set_location(float x, float y) = 0;

    virtual VectorF location() const = 0;

    virtual float width() const = 0;

    virtual float height() const = 0;

    virtual void set_style(const StyleMap &) = 0;

    /** @brief Called by frame for automatic widget sizing.
     *  This in effect is telling the widget, go ahead, there are no
     *  restrictions on widget size
     *  @note The default behavior is for the widget to do nothing (no resize)
     *  @note Presently Frame and TextArea both use this function.
     */
    virtual void issue_auto_resize();

    /** @brief Adds another widget pointer
     *  Intended for pointers to aggregate class members.
     *  @warning Frame does not own the widget pointers, it's the callers
     *  responsiblity to delete their widgets. If you to dynamically create
     *  your widgets on the fly, it is recommended to use smart pointers
     *  (std::unique_ptr & std::shared_ptr).
     *  @note Other widgets may of course override this function themselves,
     *        add have member widgets of thier own.
     *  @note Perhaps as a future feature, removing a widget pointer will be
     *        added to this interface. For now "clear_widgets" and re-adding
     *        them to the Frame is necessary.
     */
    virtual void add_widget(Widget *);

    virtual void accept(Visitor &);

    virtual void accept(const Visitor &) const;

    void set_visible(bool v) { m_visible = v; }

    bool is_visible() const { return m_visible; }

private:
    bool m_visible;
};

} // end of ksg namespace
