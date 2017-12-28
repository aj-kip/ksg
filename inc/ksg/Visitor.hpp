/****************************************************************************

    File: Visitor.hpp
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

#include "ForwardWidgets.hpp"
#include <common/TypeList.hpp>

namespace ksg {

// ----------------------------------------------------------------------------

template <typename ... Types>
class VisitorLayer {
public:
    virtual ~VisitorLayer() {}
    // just makes the compiler happy
    virtual void visit() = 0;
};

template <typename Head, typename ... Types>
class VisitorLayer<Head, Types...> : public VisitorLayer<Types...> {
public:
    virtual ~VisitorLayer<Head, Types...>(){}
    using VisitorLayer<Types...>::visit;

    virtual void visit(Head &);
    virtual void visit(const Head &) const;
};

// ---------------------------- PUBLIC INTERFACE ------------------------------

/** Defines the Visitor class specific for all widget types available in this
 *  library.
 *  It can be easily expanded to include other user defined widget types as per
 *  the visitor design pattern.
 *  This class uses some template magic to make sure the complete list of
 *  library defined widgets matches that with the virtual methods present in
 *  this class.
 *
 *  @note writable visit methods are defined automatically, however the
 *        constant reference version of the method will need to be defined
 */
class Visitor :
    public BuiltinWidgetTypes::DefineWithListTypes<VisitorLayer>::Type
{
public:
    // be aware: pure virtual visit methods here
    using VisitorLayer::visit;

protected:
    Visitor(); // defined in Widgets.cpp
    // no one is going to call this
    void visit() final {}
};

// ----------------------------------------------------------------------------

template <typename Head, typename ... Types>
void VisitorLayer<Head, Types...>::visit(Head & obj) {
    const auto & const_obj = obj;
    visit(const_obj);
}

template <typename Head, typename ... Types>
void VisitorLayer<Head, Types...>::visit(const Head &) const {}

} // end of ksg namespace
