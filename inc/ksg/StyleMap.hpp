/****************************************************************************

    File: StyleMap.hpp
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

#include <common/StorageUnion.hpp>
#include <common/MultiType.hpp>

#include <SFML/Graphics/Color.hpp>

#include <map>
#include <string>
#include <functional>

namespace sf { class Font; }

namespace ksg {

using StylesField = MultiType<sf::Color, float, const sf::Font *>;

using StyleMap = std::map<std::string, StylesField>;

StyleMap construct_system_styles();

class Text;

class StyleFinder {
public:
    template <typename T>
    struct PointerTo {
        using Type = T *;
        using OriginalType = T;
    };

    StyleFinder(const StyleMap & styles): m_styles_ptr(&styles) {}

    bool set_if_found(const char * field_name, sf::Color *);

    bool set_if_found(const char * field_name, float *);

    bool set_if_found(const char * field_name, PointerTo<const sf::Font *>::Type);

    template <typename T, typename Func>
    typename std::enable_if<StylesField::HasType<T>::VALUE, void>::
    type call_if_found(const char * field_name, Func && f);

    void assign_font_if_found(const char * field_name, Text & text);

private:

    const StylesField & find_field(const char * field_name) const;

    StylesField find_field_nothrow(const char * field_name) const noexcept;

    const StyleMap * m_styles_ptr;
};

template <typename T, typename Func>
typename std::enable_if<StylesField::HasType<T>::VALUE, void>::
type StyleFinder::call_if_found(const char * field_name, Func && f) {
    T obj;
    if (set_if_found(field_name, &obj))
        f(std::cref(obj));
}

} // end of namespace ksg

