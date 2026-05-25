#ifndef ENGINE_OSCILLATION_HPP
#define ENGINE_OSCILLATION_HPP

#include <glm/glm.hpp>
#include <cmath>

class Oscillation {
public:
    Oscillation(const glm::vec3& position, float radius, const glm::vec3& speed)
        : _position(position), _radius(radius), _speed(speed) {}

    glm::vec3 getPosition(float time) const {
        return _position + _radius * glm::vec3(
            _speed.x != 0.0f ? std::sin(time * _speed.x) : 0.0f,
            _speed.y != 0.0f ? std::sin(time * _speed.y) : 0.0f,
            _speed.z != 0.0f ? std::sin(time * _speed.z) : 0.0f
        );
    }

private:
    glm::vec3 _position;
    float _radius;
    glm::vec3 _speed;
};

#endif // ENGINE_OSCILLATION_HPP
