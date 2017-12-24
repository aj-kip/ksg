/****************************************************************************

    File: ImageWidget.cpp
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

#include <ksg/ImageWidget.hpp>
#include <ksg/Visitor.hpp>

#include <SFML/Graphics/RenderTarget.hpp>

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
}

void ImageWidget::load_from_texture
    (const sf::Texture & texture_, const sf::IntRect & trect_)
{
    const auto & texture = m_texture_storage.reset<sf::Texture>(texture_);
    m_spt.setTexture    (texture);
    m_spt.setTextureRect(trect_ );
}

void ImageWidget::set_texture_shared_pointer
    (std::shared_ptr<const sf::Texture> shrd_ptr)
{ m_texture_storage = TextureMultiType(shrd_ptr); }

void ImageWidget::assign_texture(const sf::Texture * tptr)
    { m_texture_storage = TextureMultiType(tptr); }

void ImageWidget::reset_texture_rectangle(const sf::IntRect & trect_)
    { m_spt.setTextureRect(trect_); }

void ImageWidget::set_location(float x, float y)
    { m_spt.setPosition(x, y); }

VectorF ImageWidget::location() const { return m_spt.getPosition(); }

float ImageWidget::width() const
    { return float(m_spt.getTextureRect().width  * m_spt.getScale().x); }

float ImageWidget::height() const
    { return float(m_spt.getTextureRect().height * m_spt.getScale().y); }

void ImageWidget::set_size(float w, float h) {
    const auto & rect = m_spt.getTextureRect();
    m_spt.setScale(w / rect.width, h / rect.height);
}

void ImageWidget::accept(Visitor & visitor)
    { visitor.visit(*this); }

void ImageWidget::accept(const Visitor & visitor) const
    { visitor.visit(*this); }

void ImageWidget::draw(sf::RenderTarget & target, sf::RenderStates) const
    { target.draw(m_spt); }

} // end of ksg namespace
