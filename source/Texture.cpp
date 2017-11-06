#include "Texture.h"

Texture::Texture(GLenum texture_type, const std::string & filename)
    : m_texture_type(texture_type)
{
    m_image = std::make_unique<Magick::Image>(filename);

    Magick::Blob blob;
    m_image->write(&blob, "RGBA");

    glGenTextures(1, &m_texture_obj);
    glBindTexture(m_texture_type, m_texture_obj);
    glTexImage2D(m_texture_type, 0, GL_RGB,
            m_image->columns(),
            m_image->rows(),
            0,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            blob.data());
    glTexParameterf(m_texture_type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(m_texture_type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Texture::bind(GLenum texture_unit)
{
    glActiveTexture(texture_unit);
    glBindTexture(m_texture_type, m_texture_obj);
}
