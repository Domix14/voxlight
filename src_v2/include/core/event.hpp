#pragma once

#include <array>
#include <cstdint>
#include <vector>
#include <concepts>


template<typename T> requires std::is_enum_v<T>
struct Event {
    virtual ~Event() = default;

    using EventType = T;
};