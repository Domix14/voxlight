#include "Chunk.hpp"


Chunk::Chunk(glm::vec3 origin, glm::vec3 size, std::vector<GLubyte> const& data) {
    chunkSize = size;
    voxelData = data;
    minBox = origin;
    maxBox = origin + size;
    // glGenTextures(1, &texture);
    // glBindTexture(GL_TEXTURE_3D, texture);
    // glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    // glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    // glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    // glTexImage3D(GL_TEXTURE_3D, 0, GL_R8, size.x, size.y, size.z, 0, GL_RED, GL_UNSIGNED_BYTE, voxelData.data());

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_3D, texture);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_R8, chunkSize.x, chunkSize.y, chunkSize.z, 0, GL_RED, GL_UNSIGNED_BYTE, voxelData.data());
}