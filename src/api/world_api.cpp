#include <core/voxlight.hpp>
#include <pugixml.hpp>
#include <voxlight_api.hpp>

WorldApi::WorldApi(Voxlight &voxlight) : voxlight(voxlight) {}

void WorldApi::loadWorldState(std::filesystem::path path) {}

void WorldApi::saveWorldState(std::filesystem::path path) {}
