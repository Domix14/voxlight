#pragma once

class VoxelEngine;

class System {
   public:
    System(VoxelEngine* engine) : engine(engine) {}
    virtual ~System() = default;

    virtual void init() = 0;
    virtual void update(float deltaTime) = 0;
    virtual void deinit() = 0;

   protected:
    [[nodiscard]] VoxelEngine* getEngine() const { return engine; }

   private:
    VoxelEngine* engine;
};