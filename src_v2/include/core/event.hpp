#pragma once

#include <array>
#include <concepts>
#include <cstdint>
#include <vector>

template <typename T>
  requires std::is_enum_v<T>
struct Event {
  virtual ~Event() = default;

  using EventType = T;
};
