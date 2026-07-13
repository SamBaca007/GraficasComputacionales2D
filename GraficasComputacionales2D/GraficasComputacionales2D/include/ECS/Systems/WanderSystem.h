#pragma once
#include "ECS/System.h"
#include "ECS/Registry.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Kinematic.h"
#include "ECS/Components/Wander.h"
#include <cmath>
#include <cstdlib>

namespace ECS {
  class WanderSystem final : public System {
  public:
    void OnUpdate(Registry& registry, float dt) override {
      registry.GetView<Transform, Kinematic, Wander>().Each(
        [&](EntityID entity, Transform& transform, Kinematic& kinematic, Wander& wander) {

          // Variar el ángulo aleatoriamente usando tu maxJitter
          wander.wanderAngle += ((std::rand() % 200) - 100) * 0.01f * wander.maxJitter;

          // Calcular el centro del círculo de deambulación usando tu circleDistance
          sf::Vector2f circleCenter = kinematic.velocity;
          float speed = std::sqrt(circleCenter.x * circleCenter.x + circleCenter.y * circleCenter.y);
          if (speed > 0) {
            circleCenter = (circleCenter / speed) * wander.circleDistance;
          }
          else {
            circleCenter = sf::Vector2f(wander.circleDistance, 0);
          }

          // Calcular el desplazamiento en el borde del círculo usando tu circleRadius
          sf::Vector2f displacement(
            std::cos(wander.wanderAngle) * wander.circleRadius,
            std::sin(wander.wanderAngle) * wander.circleRadius
          );

          // Sumamos la fuerza a la aceleración
          sf::Vector2f wanderForce = circleCenter + displacement;
          kinematic.acceleration += wanderForce;
        });
    }
  };
} // Namespace ECS