#pragma once
#include "ECS/System.h"
#include "ECS/Registry.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Kinematic.h"
#include "ECS/Components/Steering.h"
#include <cmath>

namespace ECS {

  class SteeringSystem final : public System {
  public:
    SteeringSystem() = default;

    void OnUpdate(Registry& registry, float deltaTime) override {
      registry.GetView<Transform, Kinematic, Steering>().Each(
        [&registry](EntityID entity, Transform& transform,
          Kinematic& kinematic, Steering& steering) {

            // 1. Determinar la posición final del objetivo
            sf::Vector2f targetPos = steering.target;

            if (steering.targetEntity != NULL_ENTITY &&
              registry.IsAlive(steering.targetEntity)) {
              if (auto* targetTransform = registry.TryGetComponent<Transform>
                (steering.targetEntity)) {
                targetPos = targetTransform->position;
              }
            }

            sf::Vector2f steeringForce{ 0.f, 0.f };

            // 2. Calcular la fuerza de dirección según el comportamiento activo
            switch (steering.currentBehavior) {
            case SteeringBehaviorType::Seek:
              steeringForce = CalculateSeek(transform.position, targetPos, kinematic);
              break;
            case SteeringBehaviorType::Flee:
              steeringForce = CalculateFlee(transform.position, targetPos, kinematic);
              break;
            case SteeringBehaviorType::Arrive:
              steeringForce = CalculateArrive(transform.position, targetPos, kinematic,
                steering.slowingRadius);
              break;
            case SteeringBehaviorType::None:
            default:
              break;
            }

            // 3. ACUMULAR LA FUERZA
            // Nota el "+=" en lugar de "=". Esto permite que Obstacle Avoidance 
            // u otros sistemas sumen sus propias fuerzas sin sobrescribirse.
            kinematic.acceleration += steeringForce;

            // ¡Y listo! KinematicSystem se encargará de mover la figura después.
        }
      );
    }

  private:
    static sf::Vector2f Truncate(sf::Vector2f vector, float max) {
      float lengthSq = (vector.x * vector.x) + (vector.y * vector.y);
      if (lengthSq > max * max) {
        float length = std::sqrt(lengthSq);
        return (vector / length) * max;
      }
      return vector;
    }

    static float Normalize(sf::Vector2f& vector) {
      float length = std::sqrt((vector.x * vector.x) + (vector.y * vector.y));
      if (length > 0) {
        vector /= length;
      }
      return length;
    }

    static sf::Vector2f CalculateSeek(const sf::Vector2f& position,
      const sf::Vector2f& target, const Kinematic& kinematic) {
      sf::Vector2f desiredVelocity = target - position;
      Normalize(desiredVelocity);
      desiredVelocity *= kinematic.maxSpeed;

      sf::Vector2f steering = desiredVelocity - kinematic.velocity;
      return Truncate(steering, kinematic.maxForce);
    }

    static sf::Vector2f CalculateFlee(const sf::Vector2f& position,
      const sf::Vector2f& target, const Kinematic& kinematic) {
      sf::Vector2f desiredVelocity = position - target;
      Normalize(desiredVelocity);
      desiredVelocity *= kinematic.maxSpeed;

      sf::Vector2f steering = desiredVelocity - kinematic.velocity;
      return Truncate(steering, kinematic.maxForce);
    }

    static sf::Vector2f CalculateArrive(const sf::Vector2f& position,
      const sf::Vector2f& target, const Kinematic& kinematic, float slowingRadius) {
      sf::Vector2f desiredVelocity = target - position;
      float distance = Normalize(desiredVelocity);

      if (distance > 0) {
        float speed = kinematic.maxSpeed;
        if (distance < slowingRadius) {
          speed = kinematic.maxSpeed * (distance / slowingRadius);
        }
        desiredVelocity *= speed;

        sf::Vector2f steering = desiredVelocity - kinematic.velocity;
        return Truncate(steering, kinematic.maxForce);
      }
      return -kinematic.velocity;
    }
  };

} // Namespace ECS