/****************************************************************************

    File: ImageWidget.hpp
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

#include <memory>

#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>

#include <common/MultiType.hpp>

#include <ksg/Widget.hpp>

namespace ksg {

class ImageWidget : public Widget {
public:
    using TextureMultiType =
        MultiType<const sf::Texture *, std::shared_ptr<const sf::Texture>, sf::Texture>;

    bool load_from_file(const char * filename) noexcept;

    void load_from_image(const sf::Image & image);

    void load_from_texture(const sf::Texture & texture_,
                           const sf::IntRect & trect_ = sf::IntRect());

    void set_texture_shared_pointer(std::shared_ptr<const sf::Texture>);

    /** Assigns a pointer to the texture, with the client being responsible for
     *  ownership.
     *  @note the given pointed to object must live at least as long as this
     *        widget, or bad things will happen!
     */
    void assign_texture(const sf::Texture *);

    void reset_texture_rectangle(const sf::IntRect & trect_);

    void process_event(const sf::Event &) override {}

    void set_location(float x, float y) override;

    VectorF location() const override;

    float width() const override;

    float height() const override;

    void set_size(float w, float h);

    void set_style(const StyleMap &) override {}

    void accept(Visitor &) override;

    void accept(const Visitor &) const override;

private:

    void draw(sf::RenderTarget & target, sf::RenderStates states) const override;

    TextureMultiType m_texture_storage;
    sf::Sprite m_spt;
};

} // end of ksg namespace
