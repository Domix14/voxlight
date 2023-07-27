#ifndef CHUNK_HPP
#define CHUNK_HPP

#include <vector>

#include <glad/gl.h>
#include <glm/glm.hpp>

class Chunk {
public:
    Chunk(glm::vec3 origin, glm::vec3 size, std::vector<GLubyte> const& data);

    glm::vec3 getMinBox() { return minBox; }
    glm::vec3 getMaxBox() { return maxBox; }
    GLuint getTexture() { return texture; }
    glm::vec3 getSize() { return chunkSize; }
private:
    std::vector<GLubyte> voxelData;
    GLuint texture;

    glm::vec3 minBox;
    glm::vec3 maxBox;
    glm::vec3 chunkSize;
};

#endif // CHUNK_HPP