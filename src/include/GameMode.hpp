#ifndef GAME_MODE_HPP
#define GAME_MODE_HPP

class Engine;
class VoxelWorld;

class GameMode {
   public:
    GameMode(Engine* engine);

    void update(float deltaTime);

    void setVoxelWord(VoxelWorld* voxelWorld);

    Engine* getEngine() const;

   private:
    Engine* engine;
    VoxelWorld* voxelWorld;
}

#endif  // GAME_MODE_HPP