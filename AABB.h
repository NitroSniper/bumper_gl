#ifndef AABB_H
#define AABB_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


class AABB {
public:
    glm::vec3 min;
    glm::vec3 max;
    glm::float32 width;
    glm::float32 length;
    glm::float32 height;

    AABB(const glm::vec3& min_p, const glm::vec3& max_p) : min(min_p), max(max_p) {}

    AABB(const glm::float32& Width, const glm::float32& Length, const glm::float32& Height) : width(Width), length(Length), height(Height) {}

    void update_from_pos_and_dim(const glm::vec3& position) {
        min = { position.x - width / 2, position.y - height / 2, position.z - length / 2 };
        max = { position.x + width / 2, position.y + height / 2, position.z + length / 2 };
    }


    bool contains(const glm::vec3& point) const {
        return 
            (point.x >= min.x && point.x <= max.x) &&
            (point.y >= min.y && point.y <= max.y) &&
            (point.z >= min.z && point.z <= max.z);
    }

    bool intersects(const AABB& other) const {
        return 
            (min.x <= other.max.x && max.x >= other.min.x) &&
            (min.y <= other.max.y && max.y >= other.min.y) &&
            (min.z <= other.max.z && max.z >= other.min.z);
    }
};

#endif