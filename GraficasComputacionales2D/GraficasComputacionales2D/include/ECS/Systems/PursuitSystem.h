#pragma once
#include "ECS/System.h"
#include "ECS/Registry.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Kinematic.h"
#include "ECS/Components/Pursuit.h"
#include <cmath>

namespace ECS {
  class PursuitSystem final : public System {
  public:
    void OnUpdate(Registry& registry, float dt) override {
      registry.GetView<Transform, Kinematic, Pursuit>().Each(
        [&](EntityID entity, Transform& transform,
          Kinematic& kinematic, Pursuit& pursuit) {

          if (pursuit.targetEntity != NULL_ENTITY &&
            registry.IsAlive(pursuit.targetEntity)) {
            auto* targetTrans
              = registry.TryGetComponent<Transform>(pursuit.targetEntity);
            auto* targetKin
              = registry.TryGetComponent<Kinematic>(pursuit.targetEntity);

            if (targetTrans && targetKin) {
              // Calcular distancia al objetivo
              sf::Vector2f toPursuer = targetTrans->position - transform.position;
              float distance = std::sqrt(toPursuer.x * toPursuer.x
                + toPursuer.y * toPursuer.y);

              // Predecir posición futura basada en la velocidad del objetivo
              float lookAheadTime = distance / kinematic.maxSpeed;
              sf::Vector2f futurePosition = targetTrans->position
                + (targetKin->velocity * lookAheadTime);

              // Hacer "Seek" hacia esa posición futura
              sf::Vector2f desiredVelocity = futurePosition - transform.position;
              float length = std::sqrt(desiredVelocity.x * desiredVelocity.x
                + desiredVelocity.y * desiredVelocity.y);
              if (length > 0) desiredVelocity = (desiredVelocity / length)
                * kinematic.maxSpeed;

              sf::Vector2f steeringForce = desiredVelocity - kinematic.velocity;

              // Solo sumamos la fuerza a la aceleración
              kinematic.acceleration += steeringForce;
            }
          }
        });
    }
  };
} // Namespace ECS