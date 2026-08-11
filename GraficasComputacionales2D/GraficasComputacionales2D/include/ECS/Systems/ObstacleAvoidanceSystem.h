/**
 * @file ObstacleAvoidanceSystem.h
 * @brief Sistema de Inteligencia Artificial con cono de visión frontal para rebase.
 */

#pragma once
#include "ECS/System.h"
#include "ECS/Registry.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Kinematic.h"
#include "ECS/Components/ObstacleAvoidance.h"
#include "ECS/Components/Obstacle.h"
#include <cmath>
#include <algorithm>

namespace ECS {

  class ObstacleAvoidanceSystem final : public System {
  public:
    void OnUpdate(Registry& registry, float dt) override {
      registry.GetView<Transform, Kinematic, ObstacleAvoidance>().Each(
        [&](EntityID entity, Transform& transform, Kinematic& kinematic, ObstacleAvoidance& avoidance) {

          float speed = std::sqrt(kinematic.velocity.x * kinematic.velocity.x + kinematic.velocity.y * kinematic.velocity.y);
          if (speed < 0.1f) return;

          sf::Vector2f forward = kinematic.velocity / speed;
          sf::Vector2f ahead = transform.position + forward * avoidance.maxSeeAhead;
          sf::Vector2f ahead2 = transform.position + forward * (avoidance.maxSeeAhead * 0.5f);

          EntityID closestObstacle = NULL_ENTITY;
          float minDistance = 999999.f;
          sf::Vector2f obstaclePos;

          registry.GetView<Transform, Obstacle>().Each(
            [&](EntityID obsEntity, Transform& obsTrans, Obstacle& obs) {
              if (entity == obsEntity) return;

              bool isKart = registry.TryGetComponent<Kinematic>(obsEntity) != nullptr;

              if (isKart) {
                sf::Vector2f toObstacle = obsTrans.position - transform.position;
                float distSq = toObstacle.x * toObstacle.x + toObstacle.y * toObstacle.y;

                // Radios combinados más un margen
                float panicDist = (obs.radius * 2.5f);
                float maxDist = 80.f + obs.radius;

                if (distSq > 0.001f) {
                  float dist = std::sqrt(distSq);
                  sf::Vector2f dirToObs = toObstacle / dist;

                  // 1. BURBUJA DE PÁNICO (360 grados, muy corto alcance)
                  if (dist < panicDist) {
                    // Empuje radial hacia afuera para evitar que se encimen en curvas cerradas
                    sf::Vector2f panicPush = transform.position - obsTrans.position;
                    kinematic.acceleration += (panicPush / dist) * (avoidance.avoidanceForce * 1.5f);
                  }
                  // 2. CONO DE REBASE (Solo frontal, alcance medio)
                  else if (dist < maxDist) {
                    float dotForward = forward.x * dirToObs.x + forward.y * dirToObs.y;

                    if (dotForward > 0.85f) {
                      sf::Vector2f right = { -forward.y, forward.x };
                      float dotRight = dirToObs.x * right.x + dirToObs.y * right.y;
                      float sideMultiplier = (dotRight > 0) ? -1.0f : 1.0f;

                      float proximityFactor = 1.0f - (dist / maxDist);
                      sf::Vector2f overtakeForce = right * sideMultiplier * (avoidance.avoidanceForce * proximityFactor);

                      kinematic.acceleration += overtakeForce;
                    }
                  }
                }
              }
              else {
                // ==========================================
                // LÓGICA DE ANTENA (Solo obstáculos estáticos)
                // ==========================================
                float d1 = std::sqrt(std::pow(obsTrans.position.x - ahead.x, 2) + std::pow(obsTrans.position.y - ahead.y, 2));
                float d2 = std::sqrt(std::pow(obsTrans.position.x - ahead2.x, 2) + std::pow(obsTrans.position.y - ahead2.y, 2));
                float dPos = std::sqrt(std::pow(obsTrans.position.x - transform.position.x, 2) + std::pow(obsTrans.position.y - transform.position.y, 2));

                float minDistToObs = std::min({ d1, d2, dPos });

                if (minDistToObs < obs.radius + 15.f && minDistToObs < minDistance) {
                  minDistance = minDistToObs;
                  closestObstacle = obsEntity;
                  obstaclePos = obsTrans.position;
                }
              }
            });

          if (closestObstacle != NULL_ENTITY) {
            sf::Vector2f avoidanceForce = ahead - obstaclePos;
            float forceLength = std::sqrt(avoidanceForce.x * avoidanceForce.x + avoidanceForce.y * avoidanceForce.y);

            if (forceLength > 0) {
              avoidanceForce = (avoidanceForce / forceLength) * avoidance.avoidanceForce;
            }

            kinematic.acceleration += avoidanceForce;
          }
        });
    }
  };
} // Namespace ECS