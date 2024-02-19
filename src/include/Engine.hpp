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
struct GLFWwindow;

class VoxelWorld;

class Engine {
   public:
    Engine();

    void run();

    std::uint32_t createEntity();
    std::uint32_t createVoxelEntity(glm::vec3 pos, glm::vec3 rot, glm::vec3 size, float voxSize,
                                    std::vector<std::uint8_t> const& voxelData);

    void setCamera(ICamera* camera);
    void setWindowSize(std::uint32_t width, std::uint32_t height);

    std::pair<std::uint32_t, std::uint32_t> getWindowSize() const;

   private:
    GLFWwindow* window;
    VoxelWorld* voxelWorld;
    ICamera* camera;

    std::uint32_t windowWidth;
    std::uint32_t windowHeight;
};

#endif  // ENGINE_HPP