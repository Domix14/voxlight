#ifndef VOXEL_OBJECT_HPP
#define VOXEL_OBJECT_HPP

#include <cstdint>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

#include "utils.hpp"

class VoxelObject {
   public:
    glm::vec3 getSize() const { return size; }
    std::vector<Voxel> getData() const { return data; }
    glm::vec3 getPosition() const { return position; }
    glm::quat getRotation() const { return rotation; }
    VoxelSize getVoxelSize() const { return voxelSize; }

    VoxelObject();

    void setData(std::vector<Voxel> const& newData, glm::vec3 newSize);
    void draw(Program voxelProgram, glm::mat4 const& viewProjectionMatrix) const;

    void setPosition(glm::vec3 newPosition);
    void setRotation(glm::quat newRotation);
    void setVoxelSize(VoxelSize newVoxelSize);

   private:
    Texture voxelTexture;
    std::vector<Voxel> data;
    glm::vec3 size;
    glm::vec3 position;
    glm::quat rotation;
    VoxelSize voxelSize;
};

#endif  // VOXEL_OBJECT_HPP