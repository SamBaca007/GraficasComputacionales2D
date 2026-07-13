#pragma once
#include "Prerequisites.h"
#include "ECS/System.h"
#include "ECS/Registry.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Kinematic.h"
#include "ECS/Components/Wander.h" // Para usar tu función ECS::length y ECS::normalize

class KinematicSystem : public ECS::System {
public:
  void OnUpdate(ECS::Registry& registry, float dt) override {
    // Solo pedimos entidades que tengan Transform y Kinematic
    auto view = registry.GetView<ECS::Transform, ECS::Kinematic>();

    view.Each([&](ECS::EntityID entity, ECS::Transform& transform,
      ECS::Kinematic& kinematic) {

      // ==========================================
      // LÍMITES SUAVES (CONTAINMENT)
      // ==========================================
      // Ajusta estos límites dependiendo de qué tan grande quieres que sea tu "mundo"
      const float limitX = 1000.0f;
      const float limitY = 1000.0f;
      // Qué tan agresivo es el empujón de regreso (si van muy rápido, súbelo a 300 o 400)
      const float returnForce = 200.0f;

      if (transform.position.x < -limitX) kinematic.acceleration.x += returnForce;
      if (transform.position.x > limitX)  kinematic.acceleration.x -= returnForce;
      if (transform.position.y < -limitY) kinematic.acceleration.y += returnForce;
      if (transform.position.y > limitY)  kinematic.acceleration.y -= returnForce;
      // ==========================================

      // 1. Integración de Euler: Velocidad suma la Aceleración
      kinematic.velocity += kinematic.acceleration * dt;

      // --- FRICCIÓN PARA EVITAR LA DERIVA INFINITA ---
      // Si la aceleración es cero (ningún comportamiento la empuja), aplicamos fricción
      if (ECS::length(kinematic.acceleration) == 0.0f) {
        kinematic.velocity *= 0.98f; // Reduce la velocidad gradualmente
        if (ECS::length(kinematic.velocity) < 5.0f) {
          kinematic.velocity = { 0.f, 0.f }; // Frenado total
        }
      }
      // ------------------------------------------------------

      // 2. Limitar la velocidad para que no exceda la máxima permitida
      float speed = ECS::length(kinematic.velocity);
      if (speed > kinematic.maxSpeed) {
        kinematic.velocity = ECS::normalize(kinematic.velocity) * kinematic.maxSpeed;
      }

      // 3. Posición suma la Velocidad
      transform.position += kinematic.velocity * dt;

      // Extra visual: Rotar la figura hacia donde se está moviendo
      if (speed > 0.1f) {
        transform.rotation = std::atan2(kinematic.velocity.y, kinematic.velocity.x)
          * (180.f / 3.14159265f);
      }

      // 4. EL SECRETO: Reiniciar la aceleración a cero para el siguiente frame
      kinematic.acceleration = sf::Vector2f(0.f, 0.f);
      });
  }
};