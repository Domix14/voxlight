#include <VoxelMap.hpp>
#include <iostream>


static std::size_t getIdx(glm::vec3 pos, std::size_t worldSize) {
    return (pos.x + pos.y*worldSize + pos.z*worldSize*worldSize)*3;
}

static glm::vec3 getColor(VoxelType type) {
    switch (type)
    {
    case VoxelType::None:
        return {0,0,0};
    case VoxelType::Dirt:
        return {0.5,0.5,0.5};
    case VoxelType::Grass:
        return {0,1,0};
    case VoxelType::Stone:
        return {0.5,0.5,0.5};
    case VoxelType::Water:
        return {0,0,1};
    default:
        return {0,0,0};
    }
}

VoxelMap::VoxelMap(std::size_t size) : worldSize(size)
{
    voxelData.resize(worldSize*worldSize*worldSize*3);
    std::fill(voxelData.begin(), voxelData.end(), 0);
}

void VoxelMap::setVoxel(glm::vec3 pos, VoxelType type)
{
    auto id = getIdx(pos, worldSize);
    auto color = getColor(type);
    voxelData[id] = color.x * 255;
    voxelData[id+1] = color.y * 255;
    voxelData[id+2] = color.z * 255;
}

void VoxelMap::addSpehere(glm::vec3 center, float radius, VoxelType type)
{
    for (std::uint32_t x = 0; x < worldSize; x++)
    {
        for (std::uint32_t y = 0; y < worldSize; y++)
        {
            for (std::uint32_t z = 0; z < worldSize; z++)
            {
                glm::vec3 voxelPos = {x,y,z};
                if (glm::distance(voxelPos, center) < radius)
                {
                    setVoxel(voxelPos, type);
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
            setVoxel({x,corner.y,z}, type);
        }
    }
}