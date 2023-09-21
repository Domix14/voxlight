#ifndef VOXEL_SYSTEM_HPP
#define VOXEL_SYSTEM_HPP

#include <cstdint>
#include <vector>
#include <Camera.hpp>

#include <glad/gl.h>

constexpr std::size_t MAP_SIZE = 256;

class VoxelSystem {
public:
    VoxelSystem(class Engine* engine);
    void initialise();
    void update(float deltaTime, Camera& camera);
    void addEntity(std::uint32_t entity);
    void setVoxel(glm::ivec3 pos);

    void createWorldVoxelTexture();

private:
    class Engine* engine;
    GLuint voxelProgram;
    GLuint sunlightProgram;
    GLuint vertexBuffer;
    GLuint quadVertexBuffer;
    std::vector<std::uint32_t> entities;
    GLuint worldTexture;
    GLuint paletteTexture;
    double sunRotation;
    std::vector<std::uint8_t> mapData;

    GLuint depthFb;
    GLuint depthTexture;
    GLuint colorTexture;
    GLuint normalTexture;
};

#endif // VOXEL_SYSTEM_HPP