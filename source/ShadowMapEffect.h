#pragma once

#include "Effect.h"
#include <glm/glm.hpp>

class ShadowMapEffect : public Effect {

public:
    ShadowMapEffect() = default;
    virtual bool init();
    void set_wvp(const glm::mat4 & wvp);
    void set_texture_unit(unsigned int texture_unit);

private:

    GLuint m_wvp_location;
    GLuint m_texture_location;
};
