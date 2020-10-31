/****************************************************************************

    File: Widget.hpp
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

#include <SFML/Graphics/Drawable.hpp>

#include <ksg/StyleMap.hpp>

#include <vector>

namespace sf {
    class Font;
    class Event;
}

namespace ksg {

class FocusWidget;
class Widget;

/** @brief Child widget iterator enables a way to iterate all the child widgets
 *         for some given parent widget.
 *
 *  This class uses double dispatch between Widget's iterate_children_ and
 *  iterate_const_children_ virtual functions, and this classes on_child_ and
 *  on_const_child_ virtual functions.
 */
class ChildWidgetIterator {
public:
    virtual ~ChildWidgetIterator();
    void on_child(Widget & widget) { on_child_(widget); }
    void on_child(const Widget & widget) { on_const_child_(widget); }
protected:
    virtual void on_child_(Widget &) {}
    virtual void on_const_child_(const Widget &) {}
    ChildWidgetIterator() {}
};

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

    template <typename Func>
    void iterate_children_f(Func &&);

    template <typename Func>
    void iterate_const_children_f(Func &&) const;

    void iterate_children(ChildWidgetIterator &&);
    void iterate_children(ChildWidgetIterator &&) const;

    void iterate_children(ChildWidgetIterator &);
    void iterate_children(ChildWidgetIterator &) const;
    void set_visible(bool v) { m_visible = v; }

    bool is_visible() const { return m_visible; }
protected:
    virtual void iterate_children_(ChildWidgetIterator &);
    virtual void iterate_const_children_(ChildWidgetIterator &) const;
private:
    bool m_visible;
};

template <typename Func>
class ChildIteratorFunctor final : public ChildWidgetIterator {
public:
    explicit ChildIteratorFunctor(Func && f_): f(std::move(f_)) {}
    void on_child_(Widget & widget) override { f(widget); }
    void on_const_child_(const Widget &) {
        throw std::runtime_error(
            "ChildIteratorFunctor::on_const_child_: called constant "
            "on_const_child_, which is not desired by this type.");
    }
    Func f;
};

template <typename Func>
class ConstChildIteratorFunctor final : public ChildWidgetIterator {
public:
    explicit ConstChildIteratorFunctor(Func && f_): f(std::move(f_)) {}
    void on_child_(Widget &) override {
        throw std::runtime_error(
            "ConstChildIteratorFunctor::on_child_: called non-constant "
            "on_child_, which is not desired by this type.");
    }
    void on_const_child_(const Widget & widget) override { f(widget); }

    Func f;
};

template <typename Func>
void Widget::iterate_children_f(Func && f) {
    ChildIteratorFunctor<Func> itr(std::move(f));
    iterate_children(itr);
}

template <typename Func>
void Widget::iterate_const_children_f(Func && f) const {
    ConstChildIteratorFunctor<Func> itr(std::move(f));
    iterate_children(itr);
}

} // end of ksg namespace
