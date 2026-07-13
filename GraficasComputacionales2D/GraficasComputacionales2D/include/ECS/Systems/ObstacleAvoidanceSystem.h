#pragma once
#include "ECS/System.h"
#include "ECS/Registry.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Kinematic.h"
#include "ECS/Components/ObstacleAvoidance.h"
#include "ECS/Components/Obstacle.h"
#include <cmath>

namespace ECS {
  class ObstacleAvoidanceSystem final : public System {
  public:
    void OnUpdate(Registry& registry, float dt) override {
      registry.GetView<Transform, Kinematic, ObstacleAvoidance>().Each(
        [&](EntityID entity, Transform& transform, Kinematic& kinematic, ObstacleAvoidance& avoidance) {

          float speed = std::sqrt(kinematic.velocity.x * kinematic.velocity.x + kinematic.velocity.y * kinematic.velocity.y);
          if (speed < 0.1f) return; // Si casi no se mueve, no esquiva

          // Proyectar un "rayo de visión" usando tu maxSeeAhead
          sf::Vector2f forward = kinematic.velocity / speed;
          sf::Vector2f ahead = transform.position + forward * avoidance.maxSeeAhead;

          EntityID closestObstacle = NULL_ENTITY;
          float minDistance = 999999.f;
          sf::Vector2f obstaclePos;

          // Revisar todos los obstáculos
          registry.GetView<Transform, Obstacle>().Each(
            [&](EntityID obsEntity, Transform& obsTrans, Obstacle& obs) {
              if (entity == obsEntity) return;

              float dx = obsTrans.position.x - ahead.x;
              float dy = obsTrans.position.y - ahead.y;
              float dist = std::sqrt(dx * dx + dy * dy);

              // Si el obstáculo está dentro de nuestra visión y área de colisión
              if (dist < obs.radius + 30.f && dist < minDistance) {
                minDistance = dist;
                closestObstacle = obsEntity;
                obstaclePos = obsTrans.position;
              }
            });

          if (closestObstacle != NULL_ENTITY) {
            sf::Vector2f avoidanceForce = ahead - obstaclePos;
            float forceLength = std::sqrt(avoidanceForce.x * avoidanceForce.x + avoidanceForce.y * avoidanceForce.y);
            if (forceLength > 0) {
              // Aplicamos tu avoidanceForce para esquivar
              avoidanceForce = (avoidanceForce / forceLength) * avoidance.avoidanceForce;
            }

            kinematic.acceleration += avoidanceForce;
          }
        });
    }
  };
} // Namespace ECS