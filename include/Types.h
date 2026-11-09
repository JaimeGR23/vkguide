#pragma once

#include <cstdint>
#include <unordered_map>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <string>
#include <memory>

namespace VkGuide
{
    using int8 = std::int8_t;
    using int16 = std::int16_t;
    using int32 = std::int32_t;
    using int64 = std::int64_t;

    using uint8 = std::uint8_t;
    using uint16 = std::uint16_t;
    using uint32 = std::uint32_t;
    using uint64 = std::uint64_t;

    using float32 = float;
    using float64 = double;

    using string = std::string;
    using string_view = std::string_view;

    template<typename T>
    using vector = std::vector<T>;

    using vec2 = glm::vec2;
    using vec3 = glm::vec3;
    using vec4 = glm::vec4;

    using ivec2 = glm::ivec2;
    using ivec3 = glm::ivec3;
    using ivec4 = glm::ivec4;

    using uvec2 = glm::uvec2;
    using uvec3 = glm::uvec3;
    using uvec4 = glm::uvec4;

    using mat2 = glm::mat2;
    using mat3 = glm::mat3;
    using mat4 = glm::mat4;

    using quat = glm::quat;

    template<typename T>
    using unique_ptr = std::unique_ptr<T>;

    template<typename K, typename V>
    using unordered_map = std::unordered_map<K, V>;

    constexpr float32 PI = 3.14159265358979323846f;
    constexpr float32 TWO_PI = 2.0f * PI;
    constexpr float32 HALF_PI = 0.5f * PI;
    constexpr float32 QUARTER_PI = 0.25f * PI;

    constexpr float32 DEG2RAD = PI / 180.0f;
    constexpr float32 RAD2DEG = 180.0f / PI;

    constexpr float32 EPSILON = 1e-6f;
}

using namespace VkGuide;
