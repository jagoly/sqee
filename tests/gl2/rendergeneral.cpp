#include "rendergeneral.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <libsqee/extra/gl.hpp>

namespace sqt {

Camera::Camera() {
    viewMatrix = glm::lookAt(glm::vec3(pos[0], pos[1], pos[2]), glm::vec3(), glm::vec3(0.f, 1.f, 0.f));
    projMatrix = glm::perspective(1.17f, 4.f/3.f, 0.1f, 100.f);
}

}
