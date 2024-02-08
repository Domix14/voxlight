#pragma once

#include <cinttypes>
#include <string_view>

class Voxlight;

class EngineApi {
public:
  void start();
  void stop();

  EngineApi(Voxlight &voxlight);

private:
  Voxlight &voxlight;
};