/****************************************************************************

    File: StyleMap.hpp
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

#include <common/MultiType.hpp>

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Font.hpp>

#include <map>
#include <string>
#include <memory>

namespace ksg {

using StylesField = MultiType<
    sf::Color, float, const sf::Font *, std::shared_ptr<const sf::Font>
>;

using StyleMap = std::map<std::string, StylesField>;

namespace styles {

constexpr const char * const k_global_padding = "global-padding";
constexpr const char * const k_global_font    = "global-font";

StyleMap construct_system_styles();

/** @brief  Attempts to load a font, add store it into a styles field directly.
 *  @param  filename of the font to load
 *  @return a style field, which stores a shared_ptr to the loaded font,
 *          if loading the font failed, an empty styles field is returned
 *          instead
 */
StylesField load_font(const std::string & filename);

template <typename T, typename KeyType, typename std::enable_if<StylesField::HasType<T>::k_value && std::is_pointer<T>::value, T>::type...>
auto find(const StyleMap & smap, const KeyType & key) {
    auto itr = smap.find(key);
    if (itr != smap.end()) {
        return itr->second.template as<T>();
    }
    return static_cast<T>(nullptr);
}

template <typename T, typename KeyType, typename std::enable_if<StylesField::HasType<T>::k_value && !std::is_pointer<T>::value, T>::type...>
auto find(const StyleMap & smap, const KeyType & key) {
    auto itr = smap.find(key);
    if (itr != smap.end()) {
        return &(itr->second.template as<T>());
    }
    return static_cast<const T *>(nullptr);
}

template <typename T, typename KeyType>
typename std::enable_if<StylesField::HasType<T>::k_value, bool>::
type set_if_found(const StyleMap & smap, const KeyType & key, T & obj) {
    const T * gv = find<T>(smap, key);
    if (gv) { obj = *gv; }
    return gv;
}

} // end of styles namespace

} // end of namespace ksg
