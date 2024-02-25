#include <gtest/gtest.h>

#include <voxlight/core/components.hpp>
#include <voxlight/core/voxlight_api.hpp>
#include <voxlight/voxlight.hpp>

// Demonstrate some basic assertions.
TEST(EntityApiTest, BasicTest) {
  Voxlight engine;
  auto firstEntity = EntityApi(engine).createEntity("FirstEntity", TransformComponent());
  auto secondEntity = EntityApi(engine).createEntity("SecondEntity", TransformComponent());
  auto thirdEntity = EntityApi(engine).createEntity("ThirdEntity", TransformComponent());

  EXPECT_EQ("FirstEntity", EntityApi(engine).getName(firstEntity));
  EXPECT_EQ("SecondEntity", EntityApi(engine).getName(secondEntity));
  EXPECT_EQ("ThirdEntity", EntityApi(engine).getName(thirdEntity));

  glm::vec3 position = {1, 2, 3};
  glm::vec3 scale = {3, 2, 1};
  glm::quat rotation = {0.5, 0.5, 0.1, 0.5};
  EntityApi(engine).setPosition(firstEntity, position);
  EntityApi(engine).setScale(secondEntity, scale);
  EntityApi(engine).setRotation(thirdEntity, rotation);

  EXPECT_EQ(position, EntityApi(engine).getTransform(firstEntity).position);
  EXPECT_EQ(scale, EntityApi(engine).getTransform(secondEntity).scale);
  EXPECT_EQ(rotation, EntityApi(engine).getTransform(thirdEntity).rotation);

  TransformComponent transform;
  transform.position.x = 5;
  EntityApi(engine).setTransform(firstEntity, transform);
  EXPECT_EQ(5, EntityApi(engine).getTransform(firstEntity).position.x);
}
