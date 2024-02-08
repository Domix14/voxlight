#pragma once

class Voxlight;

class ISystem {
public:
  ISystem() = default;
  virtual ~ISystem() = default;

  virtual void init(Voxlight *voxlight) = 0;
  virtual void update(float deltaTime) = 0;
  virtual void deinit() = 0;
};