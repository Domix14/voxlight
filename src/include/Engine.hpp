#ifndef ENGINE_HPP
#define ENGINE_HPP

#include <cstdint>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <utils.hpp>
#include <vector>

class ICamera;

constexpr std::uint32_t WINDOW_WIDTH = 1280;
constexpr std::uint32_t WINDOW_HEIGHT = 720;

class VoxelWorld;

class Engine {
   public:
    Engine();
    void run();

    std::uint32_t createEntity();
    std::uint32_t createVoxelEntity(glm::vec3 pos, glm::vec3 rot, glm::vec3 size, float voxSize,
                                    std::vector<std::uint8_t> const& voxelData);

    void setCamera(ICamera* camera);

   private:
    VoxelWorld* voxelWorld;
    ICamera* camera;
};

#endif  // ENGINE_HPP