#ifndef CPURENDERER_HPP
#define CPURENDERER_HPP

#include <vector>
#include <cstdint>
#include <glm/glm.hpp>
#include <glad/gl.h>

const std::uint8_t THREAD_COUNT = 40;

class CPURenderer
{
    std::size_t screenWidth;
    std::size_t screenHeight;
    std::vector<GLubyte> pixels;
    class VoxelMap* voxelMap;

public:
    CPURenderer(std::size_t screenWidth, std::size_t screenHeight);

    void process(class Camera* camera);
    void processSplit(std::size_t start, std::size_t end, glm::vec3 planeLocation, class Camera* camera);
    void draw();
    void setVoxelMap(class VoxelMap* map);
};


#endif // CPURENDERER_HPP