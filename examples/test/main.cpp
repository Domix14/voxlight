#include <spdlog/spdlog.h>

#include <core/voxel_data.hpp>
#include <voxel_engine.hpp>
#include <world/world_system.hpp>

int main() {
    spdlog::set_level(spdlog::level::info);
    VoxelEngine engine;
    engine.init();
    spdlog::info("Engine initialized");

    // Create voxel texture
    VoxelData<std::uint8_t> voxelData;
    voxelData.resize(8, 8, 8);
    for (int i = 0; i < 64; i++) {
        if (!voxelData.setVoxel(i, 0, 0, static_cast<std::uint8_t>(50))) {
            spdlog::error("Failed to set voxel");
        }
    }
    engine.getSystem<WorldSystem>()->createVoxelEntity(voxelData, glm::vec3(0.0f, 0.0f, 0.0f));

    for (int i = 0; i < 64; i++) {
        if (!voxelData.setVoxel(i, 0, 0, static_cast<std::uint8_t>(60))) {
            spdlog::error("Failed to set voxel");
        }
    }
    engine.getSystem<WorldSystem>()->createVoxelEntity(voxelData, glm::vec3(0.0f, 0.0f, 10.0f));

    for (int i = 0; i < 64; i++) {
        if (!voxelData.setVoxel(i, 0, 0, static_cast<std::uint8_t>(70))) {
            spdlog::error("Failed to set voxel");
        }
    }
    engine.getSystem<WorldSystem>()->createVoxelEntity(voxelData, glm::vec3(0.0f, 0.0f, 20.0f));

    engine.run();
    return 0;
}