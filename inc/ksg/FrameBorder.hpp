/****************************************************************************

    File: FrameBorder.hpp
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

#include <ksg/Widget.hpp>
#include <ksg/Draggable.hpp>
#include <ksg/Text.hpp>

#include <functional>

namespace ksg {

// ----------------------------------------------------------------------------
// ---                  Frame Helpers (FrameBorder Below)                   ---

class Frame;
#   if 0
class FrameHelperPriv {
    friend class Frame;
#   endif
namespace detail {

/** Sentinal Widget type used to force the parent frame to move a new line
 *  (prempts the "overflow" that too many widgets produce are put onto a line).
 *
 *  Features:
 *  - maintian a uniform interface for widgets and frame construction
 *  - forces the frame to move all following widgets to be added on the next
 *    line (see Frame declaration).
 */
class LineSeperator final : public Widget {
public:
    ~LineSeperator() override;

    void process_event(const sf::Event &) override {}

    void set_location(float, float) override {}

    VectorF location() const override { return VectorF(); }

    float width() const override { return 0.f; }

    float height() const override { return 0.f; }

    void set_style(const StyleMap &) override {}

private:
    void draw(sf::RenderTarget &, sf::RenderStates) const override {}
};

class HorizontalSpacer final : public Widget {
public:
    HorizontalSpacer(): m_width(0.f) {}

    void process_event(const sf::Event &) override { }

    void set_location(float x_, float y_) override;

    VectorF location() const override;

    float width() const override;

    float height() const override { return 0.f; }

    void set_width(float w);

    void set_style(const StyleMap &) override {}

private:
    void draw(sf::RenderTarget &, sf::RenderStates) const override {}

    VectorF m_location;
    float m_width;
};

} // end of detail namespace
#   if 0
}; // end of FrameHelperPriv class
#   endif
// ----------------------------------------------------------------------------

/** A helper class for Frame. The object manages the border graphics, events,
 *  and provides information for widget placement.
 *  Naturally the client only needs to concern themselves mostly with the
 *  title, styling, and click event function.
 */
class FrameBorder final : public Draggable, public sf::Drawable {
public:
    using VectorF = Widget::VectorF;
    using UString = Text::UString;
    enum ClickResponse {
        k_skip_other_events,
        k_continue_other_events
    };
    struct EventResponseSignal {
        bool skip_other_events      = false;
        bool should_update_geometry = false;
    };
    static constexpr const float k_default_padding = 2.f;

    VectorF widget_start() const noexcept;
    VectorF location() const noexcept;
    float width() const noexcept { return m_back.width(); }
    float height() const noexcept { return m_back.height(); }
    EventResponseSignal process_event(const sf::Event &);
    void set_location(float x, float y);
    void set_style(const StyleMap &);
    void set_size(float w, float h);

    /** Sets the title of the border.
     *  @note Title bar becomes invisible if empty string is given. If a blank
     *        bar is desired for some reason, giving " " as the title will have
     *        the desired effect.
     *  @note An empty string will also disable dragging.
     */
    void set_title(const UString &);

    /** Sets the function/functor to call in the event that the mouse is
     *  clicked inside the frame.
     *  @param f function that takes no arguments and returns a ClickResponse
     *  If function f returns k_skip_other_events, then no other events which
     *  may occur with this frame will fire. If function f returns
     *  k_continue_other_events, then event processing will continue normally
     *  after f returns.
     *  @see Frame::ClickResponse
     */
    template <typename Func>
    void set_register_click_event(Func && f);

    /** Resets the register click event function back to its default value. */
    void reset_register_click_event();

    void update_geometry();

    /** @return the pixels needed to be set aside for rendering the title's
     *          width
     *  @note   accommodations for the title's height can be made from the
     *          difference between widget_start and location vertical distance.
     */
    float title_width_accommodation() const noexcept;

    /** @return the pixel width available for widgets */
    float width_available_for_widgets() const noexcept;

private:
    void update_drag_position(int drect_x, int drect_y) override;
    void draw(sf::RenderTarget &, sf::RenderStates) const override;

    float title_height() const noexcept;

    void check_should_update_drag(const sf::Event &);

    static ClickResponse do_default_click_event();

    float m_outer_padding = k_default_padding;
    bool m_recently_dragged = false;
    DrawRectangle m_back;
    DrawRectangle m_title_bar;
    DrawRectangle m_widget_body;

    Text m_title;

    std::function<ClickResponse()> m_click_in_frame = do_default_click_event;
};

// ----------------------------------------------------------------------------

template <typename Func>
void FrameBorder::set_register_click_event(Func && f) {
    m_click_in_frame = std::move(f);
}

} // end of ksg namespace
