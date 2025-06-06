#include "system/input/GLFWInputSystem.h"
#include "ecs/System.h"
#include <cstddef>
#include <format>
#include <GLFW/glfw3.h>
#include <iostream>
#include <memory>
#include <print>
#include <string_view>
#include <utility>

namespace {
auto getActionName(int action) -> std::string_view {
  return action == GLFW_PRESS ? "PRESSED" : action == GLFW_REPEAT ? "REPEATED" : "RELEASED";
}
} // namespace

class GLFWInputSystem::Impl {
private:
  static void keyCallbackWrapper(GLFWwindow *window, int key, int scancode, int action, int mods) {
    auto *self = static_cast<Impl *>(glfwGetWindowUserPointer(window));
    self->keyCallback(window, key, scancode, action, mods);
  }

  static void mouseButtonCallbackWrapper(GLFWwindow *window, int button, int action, int mods) {
    auto *self = static_cast<Impl *>(glfwGetWindowUserPointer(window));
    self->mouseButtonCallback(window, button, action, mods);
  }

  static void cursorPosCallbackWrapper(GLFWwindow *window, double xpos, double ypos) {
    auto *self = static_cast<Impl *>(glfwGetWindowUserPointer(window));
    self->cursorPosCallback(window, xpos, ypos);
  }

  static void scrollCallbackWrapper(GLFWwindow *window, double xoffset, double yoffset) {
    auto *self = static_cast<Impl *>(glfwGetWindowUserPointer(window));
    self->scrollCallback(window, xoffset, yoffset);
  }

  void keyCallback(GLFWwindow * /*window*/, int key, int scancode, int action, int mods) {
    const auto *keyName = glfwGetKeyName(key, scancode);
    if (keyName == nullptr) {
      keyName = std::format("token {} scancode {}", key, scancode).data();
    }
    auto actionName = getActionName(action);
    std::println("{}", std::format("GLFWInputSystem:: key {} {} with mods {}", keyName, actionName, mods));
  }

  void mouseButtonCallback(GLFWwindow * /*window*/, int button, int action, int mods) {
    auto actionName = getActionName(action);
    std::println("{}", std::format("GLFWInputSystem:: mouse button {} {} with mods {}", button, actionName, mods));
  }

  void cursorPosCallback(GLFWwindow * /*window*/, double xpos, double ypos) {
    std::println("{}", std::format("GLFWInputSystem:: Cursor position {{ {:.2f}; {:.2f} }}", xpos, ypos));
  }

  void scrollCallback(GLFWwindow * /*window*/, double xoffset, double yoffset) {
    std::println("{}", std::format("GLFWInputSystem:: Scroll offset {{ {:.2f}; {:.2f} }}", xoffset, yoffset));
  }

public:
  explicit Impl(GLFWwindow *glfwWindow) noexcept {
    if (glfwWindow == nullptr) {
      std::println(std::cerr, "GLFWWindow* is null in GLFWInputSystem::Impl()");
      return;
    }

    glfwMakeContextCurrent(glfwWindow);
    glfwSetWindowUserPointer(glfwWindow, this);
    glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // hide cursor for infinite cursor moves

    glfwSetKeyCallback(glfwWindow, Impl::keyCallbackWrapper);
    glfwSetMouseButtonCallback(glfwWindow, Impl::mouseButtonCallbackWrapper);
    glfwSetCursorPosCallback(glfwWindow, Impl::cursorPosCallbackWrapper);
    glfwSetScrollCallback(glfwWindow, Impl::scrollCallbackWrapper);
  }

  ~Impl() {
    auto *glfwWindow = glfwGetCurrentContext();
    if (glfwWindow != nullptr) {
      glfwSetWindowUserPointer(glfwWindow, nullptr);
    }
  }
};

GLFWInputSystem::GLFWInputSystem(SystemID id, int priority, GLFWwindow *glfwWindow) noexcept
    : System(id, priority)
    , pimpl(new Impl(glfwWindow)) {}

GLFWInputSystem::~GLFWInputSystem() = default;

void GLFWInputSystem::start() {
  std::println("GLFWInputSystem.start()");
}

void GLFWInputSystem::update(float /*dt*/) {
  glfwPollEvents();
}
