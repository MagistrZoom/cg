#include "pipeline.h"

const glm::mat4& Pipeline::get_world()
{
    m_world = m_world_matrix * m_rotate_matrix * m_scale_matrix;
    return m_world;
}

const glm::mat4& Pipeline::get_wvp()
{
    const auto & world = get_world();
    m_wvp = m_perspective_matrix * m_camera * world;
    return m_wvp;
}
