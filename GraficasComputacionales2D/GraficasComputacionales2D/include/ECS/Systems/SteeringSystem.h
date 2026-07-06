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
      // Obtenemos todas las entidades que tengan Transform, Kinematic
      // y Steering simultáneamente
      registry.GetView<Transform, Kinematic, Steering>().Each(
        [&registry, deltaTime](EntityID entity, Transform& transform,
          Kinematic& kinematic, Steering& steering) {

          // 1. Determinar la posición final del objetivo
          sf::Vector2f targetPos = steering.target;

          // Si le dijimos que siga a OTRA entidad, buscamos la posición real
          // de esa entidad
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
            // Si no hay comportamiento, no hay fuerza adicional.
            // Aquí podrías agregar fricción en el futuro si deseas que
            // se detenga suavemente.
            break;
          }

          // 3. Aplicar física de Integración de Euler
          // Fuerza = Masa * Aceleración (asumiendo masa = 1, Fuerza = Aceleración)
          kinematic.acceleration = steeringForce;

          // Actualizar velocidad basada en la aceleración y el tiempo
          kinematic.velocity += kinematic.acceleration * deltaTime;

          // Truncar la velocidad para no superar el maxSpeed del componente Kinematic
          float speedSq = (kinematic.velocity.x * kinematic.velocity.x) +
            (kinematic.velocity.y * kinematic.velocity.y);
          if (speedSq > (kinematic.maxSpeed * kinematic.maxSpeed)) {
            float speed = std::sqrt(speedSq);
            kinematic.velocity = (kinematic.velocity / speed) * kinematic.maxSpeed;
          }

          // Actualizar la posición final en el Transform
          transform.position += kinematic.velocity * deltaTime;

          // Extra visual (Opcional): Hacer que la entidad "mire" hacia donde se mueve
          if (speedSq > 0.1f) {
            transform.rotation = std::atan2(kinematic.velocity.y, kinematic.velocity.x)
              * 180.f / 3.14159f;
          }

          // Reiniciar aceleración al final del frame para que no se acumule infinitamente
          kinematic.acceleration = { 0.f, 0.f };
        }
      );
    }

  private:
    // Helper: Trunca un vector para que su magnitud no exceda un máximo
    static sf::Vector2f Truncate(sf::Vector2f vector, float max) {
      float lengthSq = (vector.x * vector.x) + (vector.y * vector.y);
      if (lengthSq > max * max) {
        float length = std::sqrt(lengthSq);
        return (vector / length) * max;
      }
      return vector;
    }

    // Helper: Normaliza un vector y devuelve su magnitud original
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
      sf::Vector2f desiredVelocity = position - target; // Inverso al Seek
      Normalize(desiredVelocity);
      desiredVelocity *= kinematic.maxSpeed;

      sf::Vector2f steering = desiredVelocity - kinematic.velocity;
      return Truncate(steering, kinematic.maxForce);
    }

    static sf::Vector2f CalculateArrive(const sf::Vector2f& position,
      const sf::Vector2f& target, const Kinematic& kinematic, float slowingRadius) {
      sf::Vector2f desiredVelocity = target - position;
      // Guarda la distancia real antes de normalizar
      float distance = Normalize(desiredVelocity); 

      if (distance > 0) {
        float speed = kinematic.maxSpeed;
        // Frenado: Si está dentro del círculo de Arrive, disminuye la velocidad
        if (distance < slowingRadius) {
          speed = kinematic.maxSpeed * (distance / slowingRadius);
        }
        desiredVelocity *= speed;

        sf::Vector2f steering = desiredVelocity - kinematic.velocity;
        return Truncate(steering, kinematic.maxForce);
      }
      // Si ya está exactamente en el objetivo, devuelve una fuerza opuesta 
      // a la velocidad para frenar en seco
      return -kinematic.velocity;
    }
  };

} // Namespace ECS