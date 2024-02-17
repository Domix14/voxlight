#include "rendering/render_utils.hpp"

#include <glad/gl.h>


unsigned int CreateVoxelTexture(std::uint8_t const *data, glm::ivec3 size) {
  unsigned int texname;
  glGenTextures(1, &texname);
  glBindTexture(GL_TEXTURE_3D, texname);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  glTexImage3D(GL_TEXTURE_3D, 0, GL_R8, size.x, size.y, size.z, 0, GL_RED, GL_UNSIGNED_BYTE, data);
  glBindTexture(GL_TEXTURE_3D, 0);
  return texname;
}

void DeleteVoxelTexture(unsigned int textureId) { glDeleteTextures(1, &textureId); }