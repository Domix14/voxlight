#include "VoxelObject.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

VoxelObject::VoxelObject()
    : voxelTexture(NO_TEXTURE), size(0.f), position(0.f), rotation(glm::vec3(0.f)), voxelSize(VOXEL_SIZE_12CM) {}

void VoxelObject::setData(std::vector<Voxel> const& newData, glm::vec3 newSize) {
    assert(newData.size() == (newSize.x * newSize.y * newSize.z));
    data = newData;
    size = newSize;
    if (voxelTexture != NO_TEXTURE) {
        glDeleteTextures(1, &voxelTexture);
        voxelTexture = NO_TEXTURE;
    }
    voxelTexture = createVoxelTexture(data, size);
}

void VoxelObject::draw(Program voxelProgram, glm::mat4 const& viewProjectionMatrix) const {
    auto rot = glm::toMat4(rotation);
    auto trans = glm::translate(glm::mat4(1.f), position);
    auto scale = glm::scale(glm::mat4(1.f), size * voxelSize);
    glm::mat4 modelMatrix = trans * rot * scale;
    auto voxScale = glm::scale(glm::mat4(1.f), size);
    auto mvp = viewProjectionMatrix * modelMatrix;
    auto voxelMatrix = trans * voxScale * glm::inverse(mvp);

    auto minBox = position;
    auto maxBox = minBox + size;
    glUniformMatrix4fv(glGetUniformLocation(voxelProgram, "uModelMatrix"), 1, GL_FALSE, &modelMatrix[0][0]);
    glUniform3f(glGetUniformLocation(voxelProgram, "uMinBox"), minBox.x, minBox.y, minBox.z);
    glUniform3f(glGetUniformLocation(voxelProgram, "uMaxBox"), maxBox.x, maxBox.y, maxBox.z);
    glUniform3f(glGetUniformLocation(voxelProgram, "uChunkSize"), size.x, size.y, size.z);
    glUniform1f(glGetUniformLocation(voxelProgram, "uVoxSize"), voxelSize);
    glUniformMatrix4fv(glGetUniformLocation(voxelProgram, "uMagicMatrix"), 1, GL_FALSE, &voxelMatrix[0][0]);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, voxelTexture);

    glDrawArrays(GL_TRIANGLES, 0, 36);  // 3 indices starting at 0 -> 1 triangle
}

void VoxelObject::setPosition(glm::vec3 newPosition) { position = newPosition; }

void VoxelObject::setRotation(glm::quat newRotation) { rotation = newRotation; }

void VoxelObject::setVoxelSize(VoxelSize newVoxelSize) { voxelSize = newVoxelSize; }
