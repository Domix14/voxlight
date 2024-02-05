#pragma once

class VoxelEngine;

class ISystem {
public:
  ISystem() = default;
  virtual ~ISystem() = default;

  virtual void init(VoxelEngine *voxelEngine) = 0;
  virtual void update(float deltaTime) = 0;
  virtual void deinit() = 0;
};