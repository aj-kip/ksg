/****************************************************************************

    File: ImageWidget.cpp
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

#include <ksg/ImageWidget.hpp>

#include <common/Util.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

#include <cassert>

namespace {

using TextureMultiType = ksg::ImageWidget::TextureMultiType;
using VectorF          = ksg::ImageWidget::VectorF         ;
using Error            = std::runtime_error                ;

const char * const CANNOT_UPLOAD_TEXTURE_MSG =
    "ImageWidget: cannot upload image to texture.";

} // end of <anonymous> namespace

namespace ksg {

bool ImageWidget::load_from_file(const char * filename) noexcept {
    auto & texture = m_texture_storage.reset<sf::Texture>();
    if (texture.loadFromFile(filename)) {
        m_spt.setTexture(texture);
        update_size_post_load();
        check_invarients();
        return true;
    } else {
        return false;
    }
}

void ImageWidget::load_from_image(const sf::Image & image) {
    auto & texture = m_texture_storage.reset<sf::Texture>();
    if (!texture.loadFromImage(image))
        throw Error(CANNOT_UPLOAD_TEXTURE_MSG);
    m_spt.setTexture(texture);
    update_size_post_load();
    check_invarients();
}

void ImageWidget::set_texture
    (const sf::Texture & texture_, const sf::IntRect & trect_)
{
    const auto & texture = m_texture_storage.reset<sf::Texture>(texture_);
    m_spt.setTexture(texture);
    if (trect_ != sf::IntRect())
        m_spt.setTextureRect(trect_ );
    update_size_post_load();
    check_invarients();
}

void ImageWidget::set_texture_shared_pointer
    (std::shared_ptr<const sf::Texture> shrd_ptr)
{
    m_texture_storage = TextureMultiType(shrd_ptr);
    m_spt.setTexture(*shrd_ptr);
    update_size_post_load();
    check_invarients();
}

void ImageWidget::assign_texture(const sf::Texture * tptr) {
    m_texture_storage = TextureMultiType(tptr);
    update_size_post_load();
    check_invarients();
}

void ImageWidget::reset_texture_rectangle(const sf::IntRect & trect_)
    { m_spt.setTextureRect(trect_); }

void ImageWidget::set_location(float x, float y)
    { m_spt.setPosition(x, y); }

VectorF ImageWidget::location() const { return m_spt.getPosition(); }

float ImageWidget::width() const { return m_size.x; }

float ImageWidget::height() const { return m_size.y; }

void ImageWidget::set_size(float w, float h) {
    m_size = sf::Vector2f(w, h);
    update_size_post_load();
    check_invarients();
}

/* private */ void ImageWidget::draw
    (sf::RenderTarget & target, sf::RenderStates) const
    { target.draw(m_spt); }

/* private */ void ImageWidget::check_invarients() const {
    if (m_texture_storage.is_valid()) {
        assert(m_spt.getTexture());
    } else {
        assert(!m_spt.getTexture());
    }
}

/* private */ void ImageWidget::update_size_post_load() {
    const auto & rect = m_spt.getTextureRect();
    if (rect.width != 0.f && rect.height != 0.f) {
        m_spt.setScale(m_size.x / rect.width, m_size.y / rect.height);
    }
}

} // end of ksg namespace
