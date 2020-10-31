/****************************************************************************

    File: Widget.cpp
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

#include <ksg/Widget.hpp>

#include <stdexcept>

namespace {

using Error = std::runtime_error;
#if 0
const char * const NOT_A_FRAME_MSG = "This widget is not a frame.";
#endif
} // end of <anonymous> namespace

namespace ksg {

ChildWidgetIterator::~ChildWidgetIterator() {}

Widget::Widget(): m_visible(true) {}

Widget::~Widget() {}

/* experimental */ void Widget::iterate_children(ChildWidgetIterator && itr)
    { iterate_children_(itr); }

/* experimental */ void Widget::iterate_children(ChildWidgetIterator && itr) const
    { iterate_const_children_(itr); }

/* experimental */ void Widget::iterate_children(ChildWidgetIterator & itr)
    { iterate_children_(itr); }

/* experimental */ void Widget::iterate_children(ChildWidgetIterator & itr) const
    { iterate_const_children_(itr); }
#if 0
void Widget::add_widget(Widget *)
    { throw Error(NOT_A_FRAME_MSG); }
#endif
/* protected experimental */ void Widget::iterate_children_(ChildWidgetIterator &)
    {}

/* protected experimental */ void Widget::iterate_const_children_(ChildWidgetIterator &) const
    {}

#if 0
void Widget::add_children_to(ChildWidgetIterator &) {}

void Widget::add_focus_widgets_to(std::vector<FocusWidget *> &)
    {}
#endif

void Widget::issue_auto_resize() {}

} // end of ksg namespace
