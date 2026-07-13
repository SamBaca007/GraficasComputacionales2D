#pragma once
#include <SFML/System/Vector2.hpp>
#include <cmath>

namespace ECS {
  struct Wander {
    float circleDistance = 120.0f; // Qué tan adelante de la entidad está el círculo
    float circleRadius = 60.0f;    // El radio del círculo imaginario
    float wanderAngle = 0.0f;      // El ángulo actual del objetivo (en radianes)
    float maxJitter = 0.4f;        // Qué tanto puede cambiar el ángulo por segundo
  };

  inline float length(const sf::Vector2f& v) {
    return std::sqrt(v.x * v.x + v.y * v.y);
  }

  inline sf::Vector2f normalize(const sf::Vector2f& v) {
    float len = length(v);
    if (len != 0.f) return v / len;
    return sf::Vector2f(0.f, 0.f);
  }
} // Namespace ECS