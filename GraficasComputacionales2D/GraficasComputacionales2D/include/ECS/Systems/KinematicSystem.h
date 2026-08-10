#pragma once
#include "Prerequisites.h"
#include "ECS/System.h"
#include "ECS/Registry.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Kinematic.h"
#include "ECS/Components/Wander.h" // Para usar ECS::length y ECS::normalize
#include <cmath>

namespace ECS {
  class KinematicSystem : public ECS::System {
  public:
    void OnUpdate(ECS::Registry& registry, float dt) override {
      auto view = registry.GetView<ECS::Transform, ECS::Kinematic>();

      view.Each([&](ECS::EntityID entity, ECS::Transform& transform,
        ECS::Kinematic& kinematic) {

          float currentSpeed = ECS::length(kinematic.velocity);
          float accelMag = ECS::length(kinematic.acceleration);

          // 1. PERDIDA DE VELOCIDAD POR DERRAPE / CURVA CERRADA
          if (currentSpeed > 10.0f && accelMag > 0.1f) {
            sf::Vector2f dirVel = kinematic.velocity / currentSpeed;
            sf::Vector2f dirAcc = kinematic.acceleration / accelMag;

            // Producto punto: 1.0 (recta), 0.0 (curva a 90°), negativo (frenando)
            float alignment = (dirVel.x * dirAcc.x) + (dirVel.y * dirAcc.y);

            // Si está exigiendo un giro lateral (alignment < 0.85), pierde velocidad
            if (alignment < 0.85f) {
              float turnPenalty = (1.0f - alignment) * 160.0f * dt;
              currentSpeed = (currentSpeed - turnPenalty > 0.0f) ? (currentSpeed - turnPenalty) : 0.0f;
            }
          }

          // 2. INTEGRACIÓN DE LA DIRECCIÓN (Steering modifica hacia dónde apuntamos)
          kinematic.velocity += kinematic.acceleration * dt;
          float targetSpeed = ECS::length(kinematic.velocity);

          // 3. RAMPA DE ACELERACIÓN DEL MOTOR (Inercia Realista)
          // Define cuántos px/s² puede ganar el motor por segundo.
          // Con 85.0f, pasar de 50 px/s a 200 px/s tomará ~1.76 segundos.
          float engineAcceleration = kinematic.accelerationRate;

          if (targetSpeed > currentSpeed) {
            currentSpeed += engineAcceleration * dt;
            if (currentSpeed > targetSpeed) {
              currentSpeed = targetSpeed;
            }
          }
          else {
            currentSpeed = targetSpeed;
          }

          // --- FRICCIÓN CUANDO NO HAY ACELERACIÓN ---
          if (accelMag == 0.0f) {
            currentSpeed *= 0.98f;
            if (currentSpeed < 5.0f) currentSpeed = 0.0f;
          }

          // 4. LIMITAR A VELOCIDAD MÁXIMA PERMITIDA
          if (currentSpeed > kinematic.maxSpeed) {
            currentSpeed = kinematic.maxSpeed;
          }

          // Re-ensamblar la velocidad con su magnitud real
          if (currentSpeed > 0.1f) {
            kinematic.velocity = ECS::normalize(kinematic.velocity) * currentSpeed;
          }
          else {
            kinematic.velocity = sf::Vector2f(0.f, 0.f);
          }

          // 5. Integración de Posición
          transform.position += kinematic.velocity * dt;

          // 6. Orientar la figura hacia la velocidad
          if (currentSpeed > 0.1f) {
            transform.rotation = std::atan2(kinematic.velocity.y, kinematic.velocity.x)
              * (180.f / 3.14159265f);
          }

          // 7. Reiniciar la aceleración para el siguiente frame
          kinematic.acceleration = sf::Vector2f(0.f, 0.f);
        });
    }
  };
} // Namespace ECS