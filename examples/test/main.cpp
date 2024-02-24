#include <spdlog/spdlog.h>

#include <core/components.hpp>
#include <core/voxel_data.hpp>
#include <core/voxlight.hpp>
#include <voxlight_api.hpp>

#include "core/voxel_data.hpp"
#include "test_system.hpp"

int main() {
  spdlog::set_level(spdlog::level::info);
  Voxlight engine(1280, 720, "Voxlight Test");
  EngineApi(engine).addSystem<TestSystem>();

  EngineApi(engine).start();

  // engine.init();
  // spdlog::info("Engine initialized");

  // // Create voxel texture
  // VoxelData voxelData;
  // voxelData.resize({8, 8, 8});
  // for (int i = 0; i < 64; i++) {
  //   voxelData.setVoxel({i, 0, 0}, 50);
  // }
  // engine.getWorldSystem().createVoxelEntity(voxelData,
  //                                           glm::vec3(0.0f, 0.0f, 0.0f));

  // for (int i = 0; i < 64; i++) {
  //   voxelData.setVoxel({i, 0, 0}, 60);
  // }
  // engine.getWorldSystem().createVoxelEntity(voxelData,
  //                                           glm::vec3(0.0f, 0.0f, 10.0f));

  // for (int i = 0; i < 64; i++) {
  //   voxelData.setVoxel({i, 0, 0}, 70);
  // }
  // engine.getWorldSystem().createVoxelEntity(voxelData,
  //                                           glm::vec3(0.0f, 0.0f, 20.0f));

  // engine.run();
  return 0;
}
