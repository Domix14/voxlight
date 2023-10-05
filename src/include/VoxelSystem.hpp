#ifndef VOXEL_SYSTEM_HPP
#define VOXEL_SYSTEM_HPP

#include <glad/gl.h>

#include <Camera.hpp>
#include <VoxelObject.hpp>
#include <cstdint>
#include <vector>

constexpr std::size_t MAP_SIZE = 256;

class VoxelSystem {
   public:
    VoxelSystem(class Engine* engine);
    void initialise();
    void update(float deltaTime, Camera& camera);
    void addEntity(std::uint32_t entity);
    void setVoxel(glm::ivec3 pos);

    void createWorldVoxelTexture();
    VoxelObject* createVoxelObject();

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
    std::vector<VoxelObject> voxelObjects;

    GLuint depthFb;
    GLuint depthTexture;
    GLuint colorTexture;
    GLuint normalTexture;
};

#endif  // VOXEL_SYSTEM_HPP