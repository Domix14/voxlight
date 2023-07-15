#ifndef VOXELMAP_HPP
#define VOXELMAP_HPP

#include <vector>
#include <cstdint>
#include <glm/glm.hpp>
#include <glad/gl.h>


enum VoxelType : std::uint8_t {
    None = 0,
    Stone = 1,
    Grass = 2,
    Dirt = 3,
    Water = 4,
};

struct VoxelNode {
    VoxelNode() {
        for (int i = 0; i < 8; i++) {
            children[i] = nullptr;
            type = VoxelType::Stone;
        }
    }

    VoxelNode* children[8];
    VoxelType type;
};

class VoxelMap
{ 
    std::vector<GLubyte> voxelData;
    std::size_t worldSize;

    void insert(VoxelNode* node, glm::vec3 pos, glm::vec3 origin, std::size_t blockSize, VoxelType type);
public:
    VoxelMap(std::size_t size);

    void setVoxel(glm::vec3 pos, VoxelType type);
    std::size_t getWorldSize() const { return worldSize; }
    GLubyte* getVoxelData() { return voxelData.data(); }

    void addSpehere(glm::vec3 center, float radius, VoxelType type);
    void addPlane(glm::vec3 corner, float width, float length, VoxelType type);


};

#endif // VOXELMAP_HPP