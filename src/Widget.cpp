/****************************************************************************

    File: Widget.cpp
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

#include <ksg/Widget.hpp>
#include <ksg/Visitor.hpp>

#include <stdexcept>

namespace {

using Error = std::runtime_error;

const char * const NOT_A_FRAME_MSG = "This widget is not a frame.";

} // end of <anonymous> namespace

namespace ksg {

Widget::Widget(): m_visible(true) {}

Widget::~Widget() {}

void Widget::add_widget(Widget *)
    { throw Error(NOT_A_FRAME_MSG); }

void Widget::accept(Visitor &) {}

void Widget::accept(const Visitor &) const {}

void Widget::issue_auto_resize() {}

Visitor::Visitor() {}

} // end of ksg namespace
