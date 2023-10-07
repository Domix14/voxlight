#ifndef VOXEL_WORLD_HPP
#define VOXEL_WORLD_HPP

#include <Camera.hpp>
#include <VoxelObject.hpp>
#include <utils.hpp>
#include <vector>

class Engine;

constexpr float VOXEL_WORLD_SIZE = 64;

class VoxelWorld {
   public:
    VoxelWorld(Engine* engine);

    void init();

    void update(float deltaTime);
    void render(glm::mat4 const& viewProjectionMatrix);

    VoxelObject* spawnVoxelObject();

    Engine* getEngine() const;

   private:
    Engine* engine;

    std::vector<VoxelObject> voxelObjects;
    Texture worldTexture;

    Camera camera;

    GLuint voxelProgram;
    GLuint sunlightProgram;
    GLuint vertexBuffer;
    GLuint quadVertexBuffer;
    GLuint paletteTexture;
    double sunPosition;

    GLuint depthFb;
    GLuint depthTexture;
    GLuint colorTexture;
    GLuint normalTexture;
};

#endif  // VOXEL_WORLD_HPP