/****************************************************************************

    File: Frame.hpp
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

#include <SFML/Graphics.hpp>

#include <functional>
#include <map>
#include <memory>
#include <vector>

#include <common/DrawRectangle.hpp>

#include <ksg/Draggable.hpp>
#include <ksg/Widget.hpp>
#include <ksg/Text.hpp>

namespace ksg {

/** Much like the Frame class, only this one maybe instantiated, and added as
 *  a member of a class.
 */
class SimpleFrame;

/** @brief A frame is a collection of widgets. This class is meant to provide an
 *  interface which allows controling the entire collection as one group
 *  rather than individuals.
 *
 *  The Frame class controls widget's placement and
 *  resources. The exact size of the frame is mostly out of conrol and is
 *  subject to the constraints set forth by the individual widgets.
 *  By default a frame is at position (0, 0) and has no title. It will become
 *  draggable if a title is set. @n
 *  @n
 *  Widget organization: @n
 *  Widgets are placed into "lines" from left to right in the order that they
 *  are added (much like English text). @n
 *  There exists so far two special classes to handle formating:
 *  - LineSeperator, functions like a newline
 *  - HorizontalSpacer, blank spaces, expands to fill the line, but does not
 *    affect the size of other widgets
 *
 *  Ownership Model: @n
 *  Frame does not own any widget (except special types spacers and line
 *  seperators). Anything that adds a widget to the Frame is responsilble for
 *  it's deletion. This class is designed, so that inheritors may have widgets
 *  as concrete fields (like any field declared as part of a class). It is
 *  however, quite possible to widgets which are dynamically allocated, they
 *  are just not owned (only 'observed'). @n
 *  @n
 *  The following is example code showing how frame can be used compose widgets
 *  into a gui. @n
 *  @code
class DialogBox final : public ksg::Frame {
public:
    DialogBox() {
        add_widget(&m_face);
        add_widget(&m_dialog);
        add_line_seperator();
        // move the button to the right
        add_horizontal_spacer();
        add_widget(&m_ok);
        // ...
    }

    // ...

private:
    // ... some type aliases to get rid of needing ksg::
    ImageWidget m_face;
    TextArea m_dialog;
    TextButton m_ok;
};
    @endcode
 *  === COMPOSITE PATTERN === @n
 *  Frames uses a restricted version of the composite pattern. Components maybe
 *  added but they cannot be removed nor may a client "seek" one.
 *  @n
 *  DESIGN ISSUE: @n
 *  Thankfully this is isolated to this class and clients may ignore this
 *  issue, as the class functions just fine with it.
 *  LineSeperator/HorizontalSpacer
 *  requires switch on type for geometric computations to work for Frame
 *  old-solution used switch-on type with enums @n
 *  @n
 *  @todo allow the client to clear all components
 */
class Frame : public Widget, public Draggable {
public:
    using UString = std::u32string;

    static constexpr const char * const BACKGROUND_COLOR = "frame-background";
    static constexpr const char * const TITLE_BAR_COLOR =
        "frame-title-bar-color";
    static constexpr const char * const TITLE_SIZE = "frame-title-size";
    static constexpr const char * const TITLE_COLOR = "frame-title-color";
    static constexpr const char * const WIDGET_BODY_COLOR = "frame-body";

    static constexpr const char * const GLOBAL_PADDING = "global-padding";
    static constexpr const char * const GLOBAL_FONT = "global-font";

    // <---------------------- Frame as a component -------------------------->

    /** Sets the pixel location of the frame.
     *  @param x the x coordinate
     *  @param y the y coordinate
     */
    void set_location(float x, float y) override;

    /** Processes an event. If the frame is draggable and has a title it can
     *  move with the user's mouse cursor. This function also sends events to
     *  all its widgets.
     *  @param evnt
     */
    void process_event(const sf::Event &) override;

    void set_style(const StyleMap &) override;

    /** Gets the pixel location of the frame.
     *  @return returns a vector for location in pixels
     */
    VectorF location() const override
        { return VectorF(m_back.x(), m_back.y()); }

    float width() const override;

    float height() const override;

    void set_size(float w, float h);

    void accept(Visitor &) override;

    void accept(const Visitor &) const override;

    // <------------------ Frame specific functionality ---------------------->

    void add_widget(Widget *) override final;

    void add_line_seperator()
        { add_widget(&m_the_line_seperator); }

    void add_horizontal_spacer();

    void clear_all_widgets();

    /** Sets the title of the frame.
     *  @param str the new title of the frame
     */
    void set_title(const UString &);

    void set_title_visible(bool v);

    /** Enables/Disables the dragging behavior for this frame. Note that the
     *  frame must have a title in order for it to be dragged.
     *  @param v True to turn dragging behavior on, false for off.
     */
    void set_draggable(bool v)
        { m_draggable = v; }

    void set_padding(float amt)
        { m_padding = amt; }

    using Draggable::set_position_contraints;

    using Draggable::remove_position_contraints;

    /** Updates sizes and locations for all member widgets including this frame.
     *  Individual widgets
     */
    void update_geometry();

    /** Sets the function/functor to call in the event that the mouse is
     *  clicked inside the frame.
     *  @param f function that takes no arguments and returns a ClickResponse
     *  If function f returns SKIP_OTHER_EVENTS, then no other events which
     *  may occur with this frame will fire. If function f returns
     *  CONTINUE_OTHER_EVENTS, then event processing will continue normally
     *  after f returns.
     *  @see Frame::ClickResponse
     */
    template <typename Func>
    void set_register_click_event(Func && f);

    /** Resets the register click event function back to its default value. */
    void reset_register_click_event();

    /** Type signaling how whether other events should be considered.
     *  @see Frame::set_register_click_event(Func&&)
     */
    enum ClickResponse {
        SKIP_OTHER_EVENTS,
        CONTINUE_OTHER_EVENTS
    };

protected:
    Frame();

    /** Draws the frame and all it's constintuate widgets.
     *  @param target the SFML rendering target
     */
    void draw(sf::RenderTarget & target, sf::RenderStates) const override;

private:
    using WidgetItr = std::vector<Widget *>::iterator;

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

    protected:
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

    protected:
        void draw(sf::RenderTarget &, sf::RenderStates) const override {}

        VectorF m_location;
        float m_width;
    };

    WidgetItr set_horz_spacer_widths
        (WidgetItr beg, WidgetItr end, float left_over_space, float padding);

    void set_frame_location(float x, float y);

    void update_horizontal_spacers();

    void update_drag_position(int drect_x, int drect_y) override final;

    void update_head_and_back_geometry();

    VectorF compute_size_to_fit() const;

    bool is_horizontal_spacer(const Widget *) const;

    bool is_line_seperator(const Widget *) const;

    float get_widget_advance(const Widget * widget_ptr) const;

    void issue_auto_resize() override final;

    float title_height() const;

    std::vector<Widget *> m_widgets;
    float m_padding;
    bool m_draggable;

    DrawRectangle m_back;
    DrawRectangle m_title_bar;
    DrawRectangle m_widget_body;

    bool m_title_visible;
    Text m_title;

    //! unique per instance
    LineSeperator m_the_line_seperator;
    std::vector<HorizontalSpacer> m_horz_spacers;

    std::function<ClickResponse()> m_click_in_frame;
};

/** A Simple Frame allows creation of frames without being inherited. This can
 *  be useful if the frames being created will have content depending data
 *  rather than what's hardcoded.
 */
class SimpleFrame final : public Frame {
public:
    SimpleFrame() {}
    ~SimpleFrame() override;
};

// ----------------------------------------------------------------------------

template <typename Func>
void Frame::set_register_click_event(Func && f) {
    m_click_in_frame = std::move(f);
}

} // end of ksg namespace
