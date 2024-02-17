#pragma once

#include <cstdint>
#include <glm/glm.hpp>

unsigned int CreateVoxelTexture(std::uint8_t const *data, glm::ivec3 size);
void DeleteVoxelTexture(unsigned int textureId);