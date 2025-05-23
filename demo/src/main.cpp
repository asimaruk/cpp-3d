#include "component/Camera.h"
#include "component/Mesh.h"
#include "component/Shader.h"
#include "component/Transform.h"
#include "ecs/ComponentRegistry.hpp"
#include "ecs/DefaultWorld.h"
#include "ecs/System.h"
#include "glad.h"
#include "math/Quat.h"
#include "system/render/RenderSystem.h"
#include "system/render/defaultshaders.h"
#include "system/tween/Tween.hpp"
#include "system/tween/TweenSystem.hpp"
#include "window/EngineWindow.h"
#include "window/GlfwEngineWindow.h"
#include <GLFW/glfw3.h>
#include <__ostream/print.h>
#include <iostream>
#include <memory>

#define GL_SILENCE_DEPRECATION

int main() {

#ifdef NDEBUG
  std::println("Release configuration!");
#else
  std::println("Debug configuration!");
#endif

  EngineWindow *window = new GlfwEngineWindow();
  auto windowWidth = 800;
  auto windowHeight = 600;
  window->initialize(windowWidth, windowHeight, "GGenie");

  // Инициализация GLAD
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::println(std::cerr, "GLAD error!");
    return -1;
  }

  // ECS world setup
  auto registry = std::make_shared<ComponentRegistry>();
  auto world = std::make_shared<DefaultWorld>(registry);

  // Systems setup
  auto renderSystem = std::make_shared<RenderSystem>(
      RenderSystem::ID, System::PRIORITY_LOW, registry);
  auto tweenSystem = std::make_shared<TweenSystem>(
      TweenSystem::ID, System::PRIORITY_MEDIUM, registry);
  world->registerSystem(renderSystem);
  world->registerSystem(tweenSystem);

  // Entities and component setup
  auto cube = world->createEntity();
  registry->add(cube, Mesh::cube());
  registry->add(cube, Shader(ShaderSource::DEFAULT_VERTEX,
                             ShaderSource::DEFAULT_FRAGMENT));
  registry->add(cube, Transform());
  auto camera = world->createEntity();
  auto cameraTransform = Transform{.translation{0, -1, -5}};
  registry->add(camera, cameraTransform);
  registry->add(camera, Camera(windowWidth, windowHeight, 45, 0.1, 100));

  std::println("World setup");

  auto cubeTransform = registry->get<Transform>(cube);
  if (cubeTransform.has_value()) {
    auto translateCube = std::make_shared<Tween<Transform>>(
        5,
        Transform{.scale = cubeTransform->get().scale,
                  .translation = cubeTransform->get().translation,
                  .rotation = cubeTransform->get().rotation},
        Transform{.scale = cubeTransform->get().scale,
                  .translation = cubeTransform->get().translation,
                  .rotation =
                      Quat::fromAxisAngle({0, 1, 0}, std::numbers::pi * 1.99)});
    tweenSystem->tween(cube, translateCube);
  }

  float lastTime = glfwGetTime();
  while (!window->shouldClose()) {
    float time = glfwGetTime();
    float dt = time - lastTime;
    lastTime = time;

    world->update(dt);
    window->swapBuffers();
    window->pollEvents();
  }

  delete window;
  return 0;
}