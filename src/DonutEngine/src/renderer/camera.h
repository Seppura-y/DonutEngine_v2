#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>

namespace Donut
{
    class Camera
    {
    public:
        Camera() = default;
        Camera(const glm::mat4& projection) : projection_(projection) {}
    
        const glm::mat4& getProjection() const { return projection_; }
    protected:
        glm::mat4 projection_{ glm::mat4(1.0f) };
    };
}
#endif