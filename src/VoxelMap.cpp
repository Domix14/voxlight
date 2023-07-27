#include <VoxelMap.hpp>
#include <iostream>
#include "PerlinNoise.hpp"


static std::size_t getIdx(glm::vec3 pos, std::size_t worldSize) {
    return (pos.x + pos.y*worldSize + pos.z*worldSize*worldSize);
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

void VoxelMap::generateRandom() {
    siv::PerlinNoise perlin{ 1234 };
    for (std::uint32_t x = 0; x < worldSize; x++)
    {
        for (std::uint32_t z = 0; z < worldSize; z++)
        {
            float height = perlin.octave2D_01(x, z, 4)*16;
            for(std::size_t y = 0; y < height; y++) {
                auto id = getIdx({x,y,z}, worldSize);
                voxelData[id] = 1;
            }
           
        }
    }
    //std::cout << perlin.octave3D(0.1, 0.1, 0.1, 4);
}

VoxelMap::VoxelMap(std::size_t size) : worldSize(size)
{
    voxelData.resize(worldSize*worldSize*worldSize);
    std::fill(voxelData.begin(), voxelData.end(), 0);
}

void VoxelMap::setVoxel(glm::vec3 pos, VoxelType type)
{
    auto id = getIdx(pos, worldSize);
    voxelData[id] = 1;
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