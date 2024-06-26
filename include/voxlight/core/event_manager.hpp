#pragma once

#include <array>
#include <cstdint>
#include <functional>
#include <unordered_map>
#include <vector>

#include "event.hpp"

template <typename T>
class EventManager {
 public:
  using EventType = T::EventType;
  using EventCallback = std::function<void(EventType eventType, T const&)>;

  void subscribe(EventType eventType, EventCallback listener) { observers[eventType].push_back(listener); }

  void publish(EventType eventType, T const& event) {
    if(observers.find(eventType) == observers.end()) {
      return;
    }

    for(auto& observer : observers[eventType]) {
      observer(eventType, event);
    }
  }

 private:
  std::unordered_map<EventType, std::vector<EventCallback>> observers;
};
