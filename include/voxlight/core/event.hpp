#pragma once

#include <array>
#include <concepts>
#include <cstdint>
#include <variant>
#include <vector>

template <typename T, class... Types>
  requires std::is_enum_v<T>
class Event {
 public:
  using EventType = T;

  Event(auto eventData) : event(eventData) {}

  template <typename U>
  U get() const {
    return std::get<U>(event);
  }

 private:
  std::variant<Types...> event;
};
