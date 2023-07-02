#include "CPURenderer.hpp"
#include "VoxelMap.hpp"
#include "Camera.hpp"

#include <thread>
#include <iostream>

static glm::vec3 lighting(glm::vec3 norm, glm::vec3 pos, glm::vec3 rd, glm::vec3 col) {
    glm::vec3 lightDir = glm::normalize(glm::vec3(3.0, 3.0, -1.0));
    float diffuseAttn = glm::max(glm::dot(norm, lightDir), 0.0f);
    glm::vec3 light = glm::vec3(1.0,1.f,1.f);
    
    glm::vec3 ambient = glm::vec3(0.4, 0.4, 0.4);
    
    glm::vec3 reflected = glm::reflect(rd, norm);
    float specularAttn = glm::max(glm::dot(reflected, lightDir), 0.0f);
    
    return col*(diffuseAttn*light*1.0f + ambient);
}

static glm::vec3 raycast(glm::vec3 ro, glm::vec3 rd, VoxelMap const* voxelMap) {
    //Todo: find out why this is so slow
    glm::vec3 pos = glm::floor(ro);
    
    glm::vec3 step = glm::sign(rd);

    glm::vec3 tDelta = step / rd;
    tDelta.x = glm::isnan(tDelta.x) ? 1e30 : tDelta.x;
    tDelta.y = glm::isnan(tDelta.y) ? 1e30 : tDelta.y;
    tDelta.z = glm::isnan(tDelta.z) ? 1e30 : tDelta.z;
    
    float tMaxX, tMaxY, tMaxZ;
    
    glm::vec3 fr = glm::fract(ro);
    
    tMaxX = tDelta.x * ((rd.x>0.0) ? (1.0 - fr.x) : fr.x);
    tMaxY = tDelta.y * ((rd.y>0.0) ? (1.0 - fr.y) : fr.y);
    tMaxZ = tDelta.z * ((rd.z>0.0) ? (1.0 - fr.z) : fr.z);

    glm::vec3 norm;
    const int maxTrace = 150;
    
    for (int i = 0; i < maxTrace; i++) {
        auto [type, color] = voxelMap->getVoxel(pos);
        if (type != VoxelType::None) {
            return lighting(norm, pos, rd, color)*255.f;
            // return color;
        }

        if (tMaxX < tMaxY) {
            if (tMaxZ < tMaxX) {
                tMaxZ += tDelta.z;
                pos.z += step.z;
                norm = glm::vec3(0, 0,-step.z);
            } else {
                tMaxX += tDelta.x;
            	pos.x += step.x;
                norm = glm::vec3(-step.x, 0, 0);
            }
        } else {
            if (tMaxZ < tMaxY) {
                tMaxZ += tDelta.z;
                pos.z += step.z;
                norm = glm::vec3(0, 0, -step.z);
            } else {
            	tMaxY += tDelta.y;
            	pos.y += step.y;
                norm = glm::vec3(0, -step.y, 0);
            }
        }
    }

 	return glm::vec3(0,0,0);
}

CPURenderer::CPURenderer(std::size_t width, std::size_t height) : screenWidth(width), screenHeight(height)
{
    pixels.resize(width*height*3);
    std::fill(pixels.begin(), pixels.end(), 0);
}

void CPURenderer::processSplit(std::size_t start, std::size_t end, glm::vec3 planeLocation, Camera* camera) {
    for(std::size_t x = start; x < end; ++x) {
        for(std::size_t y = 0; y < screenHeight; ++y) {
            float v = ((float)x / screenWidth)*2.f -1.f;
            float u = ((float)y / screenHeight)*2.f -1.f;
            
            auto d = planeLocation + u*camera->getRight() + v*camera->getUp();
            auto dir = glm::normalize(d - camera->getPosition());
            auto color = raycast(camera->getPosition(), dir, voxelMap);

            pixels[(x*screenHeight+y)*3] = static_cast<GLubyte>(color.x);
            pixels[(x*screenHeight+y)*3+1] = static_cast<GLubyte>(color.y);
            pixels[(x*screenHeight+y)*3+2] = static_cast<GLubyte>(color.z);
        }
    }
}

void CPURenderer::process(Camera* camera)
{
    glm::vec3 planeLocation = camera->getPosition() + 0.6f * camera->getDirection();


    if(screenWidth % THREAD_COUNT == 0) {
        std::vector<std::thread> threads;
        for(std::size_t i = 0; i < THREAD_COUNT; ++i) {
            threads.emplace_back(&CPURenderer::processSplit, this, i*screenWidth/THREAD_COUNT, (i+1)*screenWidth/THREAD_COUNT, planeLocation, camera);
        }
        for(auto& thread : threads) {
            thread.join();
        }
    } else {
        std::cout << "Warning: Screen width is not divisible by thread count, using single thread\n";
        processSplit(0, screenWidth, planeLocation, camera);
    }
}

void CPURenderer::setVoxelMap(VoxelMap* map)
{
    voxelMap = map;
}

void CPURenderer::draw()
{
    glDrawPixels(screenWidth, screenHeight, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());
}