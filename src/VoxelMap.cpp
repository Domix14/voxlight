#include <VoxelMap.hpp>

VoxelMap::VoxelMap()
{
    mapSize = 400;
    m_voxels.resize(mapSize*mapSize*mapSize);
    std::fill(m_voxels.begin(), m_voxels.end(), VoxelType::None);
}

std::tuple<VoxelType, glm::vec3> VoxelMap::getVoxel(glm::vec3 pos) const
{
    std::uint32_t x = pos.x;
    std::uint32_t y = pos.y;
    std::uint32_t z = pos.z;

    if (x < 0 || x >= mapSize || y < 0 || y >= mapSize || z < 0 || z >= mapSize)
    {
        return {VoxelType::None, glm::vec3(0,0,0)};
    }

    auto voxelType = m_voxels[x + y*mapSize + z*mapSize*mapSize];
    switch(voxelType) {
        case VoxelType::None:
            return {VoxelType::None, glm::vec3(0,0,0)};
        case VoxelType::Stone:
            return {VoxelType::Stone, glm::vec3(128,128,128)};
        case VoxelType::Grass:
            return {VoxelType::Grass, glm::vec3(0,128,0)};
        case VoxelType::Dirt:
            return {VoxelType::Dirt, glm::vec3(139,69,19)};
        case VoxelType::Water:
            return {VoxelType::Water, glm::vec3(0,0,255)};
        default:
            return {VoxelType::None, glm::vec3(0,0,0)};
    }
}

void VoxelMap::addSpehere(glm::vec3 center, float radius, VoxelType type)
{
    for (std::uint32_t x = 0; x < mapSize; x++)
    {
        for (std::uint32_t y = 0; y < mapSize; y++)
        {
            for (std::uint32_t z = 0; z < mapSize; z++)
            {
                glm::vec3 voxelPos = {x,y,z};
                if (glm::distance(voxelPos, center) < radius)
                {
                    m_voxels[x + y*mapSize + z*mapSize*mapSize] = type;
                }
            }
        }
    }
} 

void VoxelMap::addPlane(glm::vec3 corner, float width, float length, VoxelType type)
{
    for (std::uint32_t x = corner.x; x < corner.x + width; x++)
    {
        for (std::uint32_t z = corner.z; z < corner.z + length; z++)
        {
            m_voxels[x + corner.y*mapSize + z*mapSize*mapSize] = type;
        }
    }
}