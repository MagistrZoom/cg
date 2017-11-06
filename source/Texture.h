#pragma once

#include <GL/glew.h>
#include <ImageMagick-6/Magick++.h>

#include <memory>
#include <string>

class Texture
{
public:
    Texture(GLenum texture_type, const std::string & filename);

    void bind(GLenum texture_unit);

private:
    GLenum m_texture_type;
    GLuint m_texture_obj;
    std::unique_ptr<Magick::Image> m_image;
};
