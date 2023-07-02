#ifndef VOXELMAP_HPP
#define VOXELMAP_HPP

#include <vector>
#include <cstdint>
#include <glm/glm.hpp>


enum VoxelType : std::uint8_t {
    None = 0,
    Stone = 1,
    Grass = 2,
    Dirt = 3,
    Water = 4,
};

class VoxelMap
{
    std::vector<VoxelType> m_voxels;
    std::uint32_t mapSize;

    public:
        VoxelMap();

        std::tuple<VoxelType, glm::vec3> getVoxel(glm::vec3 pos) const;

        void addSpehere(glm::vec3 center, float radius, VoxelType type);
        void addPlane(glm::vec3 corner, float width, float length, VoxelType type);


};

#endif // VOXELMAP_HPP