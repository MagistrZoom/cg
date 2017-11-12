#include "ShadowMapFBO.h"

#include <iostream>

ShadowMapFBO::ShadowMapFBO()
    : m_fbo(0)
    , m_shadow_map(0)
{
}

ShadowMapFBO::~ShadowMapFBO()
{
    if (m_fbo != 0){
        glDeleteFramebuffers(1, &m_fbo);
    }

    if (m_shadow_map != 0){
        glDeleteFramebuffers(1, &m_shadow_map);
    }
}

bool ShadowMapFBO::init(unsigned int window_width, unsigned int window_height)
{
    // Создаем FBO
    glGenFramebuffers(1, &m_fbo);

    // Создаем буфер глубины
    glGenTextures(1, &m_shadow_map);
    glBindTexture(GL_TEXTURE_2D, m_shadow_map);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, window_width, window_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
                           m_shadow_map, 0);

    // Отключаем запись в буфер цвета
    glDrawBuffer(GL_NONE);

    GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if (Status != GL_FRAMEBUFFER_COMPLETE){
        std::cerr << "FB error, status: 0x%x\n" << Status;
        return false;
    }

    return true;
}

void ShadowMapFBO::read_bind(GLenum texture_unit)
{
    glActiveTexture(texture_unit);
    glBindTexture(GL_TEXTURE_2D, m_shadow_map);
}

void ShadowMapFBO::write_bind()
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
}
