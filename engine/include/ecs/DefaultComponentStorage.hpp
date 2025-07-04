#pragma once

#include "ComponentStorage.h"
#include <optional>
#include <ranges>
#include <unordered_map>
#include <vector>

template <typename R, typename T>
concept EntityComponentRange = std::ranges::input_range<R> && requires(R range) {
  { *std::ranges::begin(range) } -> std::convertible_to<std::pair<Entity, std::reference_wrapper<const T>>>;
};

template <typename T> class DefaultComponentStorage : public ComponentStorage {
private:
  std::vector<T> components;
  std::unordered_map<Entity, size_t> entity2index;
  std::vector<Entity> index2entity;

public:
  void add(Entity entity, const T &component) noexcept {
    size_t index = components.size();
    entity2index[entity] = index;
    index2entity.push_back(entity);
    components.push_back(component);
  }

  void replace(Entity entity, const T &component) noexcept {
    remove(entity);
    add(entity, component);
  }

  std::optional<std::reference_wrapper<T>> get(Entity entity) noexcept {
    auto it = entity2index.find(entity);
    if (it == entity2index.end()) {
      return std::nullopt;
    }
    return components[it->second];
  }

  void remove(Entity entity) noexcept override {
    auto it = entity2index.find(entity);
    if (it == entity2index.end()) {
      return;
    }

    size_t index = it->second;
    size_t lastIndex = components.size() - 1;

    if (index != lastIndex) {
      components[index] = std::move(components.back());
      auto lastEntity = index2entity[lastIndex];
      entity2index[lastEntity] = index;
      index2entity[index] = lastEntity;
    }

    components.pop_back();
    index2entity.pop_back();
    entity2index.erase(entity);
  }

  EntityComponentRange<T> auto all() const {
    return std::views::iota(0u, components.size()) |
           std::views::transform(
               [this](size_t index)
               {
                 return std::pair{index2entity[index], std::cref(components[index])};
               }
           );
  }

  void copyComponent(Entity source, Entity target) noexcept override {
    auto comp = get(source);
    if (comp.has_value()) {
      add(target, comp.value());
    }
  }
};
