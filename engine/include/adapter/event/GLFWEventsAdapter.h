#ifndef ADAPTER_EVENT_GLFWEVENTSADAPTER_H
#define ADAPTER_EVENT_GLFWEVENTSADAPTER_H

#include "system/event/EventSystem.hpp"
#include <memory>

class GLFWEventsAdapter {
private:
  class Impl;
  std::unique_ptr<Impl> pimpl;

public:
  GLFWEventsAdapter();
  GLFWEventsAdapter(const GLFWEventsAdapter &) = delete;
  GLFWEventsAdapter(GLFWEventsAdapter &&) = delete;

  ~GLFWEventsAdapter();

  auto operator=(const GLFWEventsAdapter &) -> GLFWEventsAdapter & = delete;
  auto operator=(GLFWEventsAdapter &&) -> GLFWEventsAdapter & = delete;

  void attach(std::shared_ptr<EventSystem> eventSystem);
  void detach(std::shared_ptr<EventSystem> eventSystem);
};

#endif
