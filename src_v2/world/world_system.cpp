#include "world_system.hpp"

#include <entt/entt.hpp>

#include "core/components.hpp"
#include "core/voxel_data.hpp"
#include "rendering/render_system.hpp"
#include "voxel_engine.hpp"

WorldSystem::WorldSystem(VoxelEngine* engine) : System(engine) {}

void WorldSystem::init() {}

void WorldSystem::update(float) {}

void WorldSystem::deinit() {}

int WorldSystem::createVoxelObject(VoxelData<std::uint8_t> const& data, glm::vec3 position) {
    auto texId = getEngine()->getSystem<RenderSystem>()->createVoxelTexture(data);
    auto voxelEntity = getEngine()->getRegistry().create();
    getEngine()->getRegistry().emplace<VoxelComponent>(voxelEntity, texId);
    getEngine()->getRegistry().emplace<TransformComponent>(
        voxelEntity, position, glm::vec3(data.getWidth(), data.getHeight(), data.getDepth()));
}