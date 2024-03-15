#include <spdlog/spdlog.h>

#include <core/voxlight.hpp>
#include <pugixml.hpp>
#include <voxlight_api.hpp>

WorldApi::WorldApi(Voxlight& voxlight) : voxlight(voxlight) {}

void WorldApi::loadWorldState(std::filesystem::path path) {
  pugi::xml_document doc;
  pugi::xml_parse_result result = doc.load_file(path.c_str());
  if(!result) {
    spdlog::error("Failed to load world state: {}", result.description());
    return;
  }

  pugi::xml_node worldNode = doc.child("world");
  if(!worldNode) {
    spdlog::error("Failed to load world state: No world node found");
    return;
  }

  for(auto& vox : worldNode.children("vox")) {
    glm::vec3 pos;
    sscanf(vox.attribute("pos").value(), "%f %f %f", &pos.x, &pos.y, &pos.z);

    VoxelData voxelData;
    voxelData.loadFromFile(vox.attribute("filepath").value(), vox.attribute("name").value());
    TransformComponent transform;
    transform.position = pos;
    transform.rotation = glm::quat(glm::vec3(0.f));
    auto entity = EntityApi(voxlight).createEntity(vox.attribute("name").value(), transform);
    VoxelComponentApi(voxlight).addComponent(entity, voxelData);
  }
}

void WorldApi::saveWorldState(std::filesystem::path path) {
  // pugi::xml_document doc;
  // pugi::xml_node worldNode = doc.append_child("world");

  // auto view = voxlight.registry.view<VoxelComponent, TransformComponent>();
  // for(auto entity : view) {
  //   auto& voxelComponent = view.get<VoxelComponent>(entity);
  //   auto& transformComponent = view.get<TransformComponent>(entity);

  //   pugi::xml_node voxNode = worldNode.append_child("vox");
  //   voxNode.append_attribute("name") = voxlight.registry.get<std::string>(entity).c_str();
  //   voxNode.append_attribute("pos") = fmt::format("{} {} {}", transformComponent.position.x,
  //                                                 transformComponent.position.y, transformComponent.position.z)
  //                                         .c_str();
  //   voxNode.append_attribute("filepath") = fmt::format("{}.vox", voxlight.registry.get<std::string>(entity)).c_str();
  // }

  // doc.save_file(path.c_str());
}

glm::ivec3 WorldApi::getWorldSize() const { return voxlight.worldSize; }
