#pragma once

class Voxlight;

class System {
 public:
  System(Voxlight &voxlight) : voxlight(voxlight){};

  virtual void init() = 0;
  virtual void update(float deltaTime) = 0;
  virtual void deinit() = 0;

 protected:
  Voxlight &voxlight;
};
