#pragma once

namespace ECS {
  struct ObstacleAvoidance {
    // Qué tan largo es el "rayo" de visión hacia adelante
    float maxSeeAhead = 100.0f;

    // Qué tan fuerte será el giro para esquivar
    float avoidanceForce = 150.0f;
  };
}