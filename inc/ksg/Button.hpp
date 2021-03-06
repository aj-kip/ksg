/****************************************************************************

    File: Button.hpp
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

#pragma once

#include <functional>

#include <common/DrawRectangle.hpp>

#include <ksg/FocusWidget.hpp>

namespace ksg {

/** A button is any widget which ha3)i7[p]:B&d!s a click event. It may also be highlighted,
 *  which is nothing more than a visual tell that the user may trigger the
 *  click event by clicking or by pressing the Return key.
 *
 *  === NON VIRTUAL INTERFACE ===
 *
 *  This class uses a non-virtual interface for changes applied to it whether
 *  its size, highlight, deselect ("anti-highlight").
 */
class Button : public FocusWidget {
public:
    using BlankFunctor = std::function<void()>;

    //! background color of button, when mouse hovers over the button
    static constexpr const char * const k_hover_back_color = "button-hover-back";
    //! foreground color of button, when mouse hovers over the button
    static constexpr const char * const k_hover_front_color = "button-hover-front";
    //! background color of button
    static constexpr const char * const k_regular_back_color = "button-back";
    //! foreground color of button
    static constexpr const char * const k_regular_front_color = "button-front";

    void set_location(float x, float y) override;

    VectorF location() const final
        { return VectorF(m_outer.x(), m_outer.y()); }

    /** Allows the setting of the width and height of Button
     *  @note the virtual on_size_changed method is available for any
     *        resize events if inheriting classes wishes to resize their
     *        internals
     *  @param w width  in pixels
     *  @param h height in pixels
     */
    void set_size(float w, float h);

    //! @return This returns width of the button in pixels.
    float width() const final
        { return m_outer.width(); }

    //! @return This returns height of the button in pixels.
    float height() const final
        { return m_outer.height(); }

    void process_event(const sf::Event & evnt) override;

    /** Sets the press event which is called whenever the button is pressed.
     *  That is when the user clicks/presses the Return key when the button is
     *  selected.
     *  @param func the callback function to call when the button is pressed
     */
    void set_press_event(BlankFunctor && func);

    /** Explicity fires the press event. (rather than having the user click it
     *  or press enter when active.)
     */
    void press();

    /** @brief Sets button's styles.
     *
     *  Sets the following styles:
     *  - hover background color
     *  - hover foreground color
     *  - regular background color
     *  - regular foreground color
     *  @note when overriding, please don't forget to make this call
     */
    void set_style(const StyleMap &) override;

    /** Padding, which is applied both horizontally and vertically. Maybe
     *  useful with geometry updates.
     *  @note added to public interface, some composite widgets may need to
     *        know this widget's padding for consistency
     *  @return padding amount in pixels
     */
    float padding() const noexcept { return m_padding; }

protected:
    /** Creates a zero-sized, white colored button. Pending setting of styles.
     */
    Button();

    /** Draws the button's background. Override to add your own button
     *  markings.
     *  @param target the target, where the button is drawn
     */
    void draw(sf::RenderTarget & target, sf::RenderStates) const override;
#   if 0
    /** Padding, which is applied both horizontally and vertically. Maybe
     *  useful with geometry updates.
     *  @return padding amount in pixels
     */
    float padding() const
        { return m_padding; }
#   endif
    /** This function is called @em after the button's size changes.
     *  Override to add your own geometry updates with location changes.
     *  @param old_width old width of the button in pixels
     *  @param old_height old height of the button in pixels
     */
    virtual void on_size_changed(float old_width, float old_height);

    /** This function is called @em after the button's location changes.
     *  Override to add your own geometry updates with location changes.
     *  @param old_x old x coordinate, left boundry of the button
     *  @param old_y old y coordinate, top boundry of the button
     */
    virtual void on_location_changed(float old_x, float old_y);

    /** Called by set_size, allowing inheriting classes to resize their
     *  internals.
     */
    virtual void set_size_back(float width, float height);

    /** Sets the size of the button's frame.
     *  @note Make sure to adjust for padding if necessary so that the button
     *        frame will not be too small.
     *  @param width  in pixels including padding
     *  @param height in pixels including padding
     */
    void set_button_frame_size(float width, float height);

    /** Change button aesthetics to denote a deselected button. */
    void deselect();

    /** Change button aesthetics to denote a selected button. */
    void highlight();

private:
    void process_focus_event(const sf::Event &) override;

    void notify_focus_gained() override;

    void notify_focus_lost() override;

    struct ColorPair {
        ColorPair(){}
        ColorPair(sf::Color b_, sf::Color f_): back(b_), front(f_) {}

        sf::Color back  = styles::get_unset_value<sf::Color>();
        sf::Color front = styles::get_unset_value<sf::Color>();
    };

    // strangley ok for default color value
    DrawRectangle m_outer;
    DrawRectangle m_inner;
    float m_padding = styles::get_unset_value<float>();
    bool m_is_highlighted = false;
    BlankFunctor m_press_functor = [](){};

    ColorPair m_reg;
    ColorPair m_hover;
};

} // end of ksg namespace
