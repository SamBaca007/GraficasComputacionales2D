/**
 * @file ObstacleAvoidanceSystem.h
 * @brief Sistema de Inteligencia Artificial para la detección y evasión activa de obstáculos.
 */

#pragma once
#include "ECS/System.h"
#include "ECS/Registry.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Kinematic.h"
#include "ECS/Components/ObstacleAvoidance.h"
#include "ECS/Components/Obstacle.h"
#include <cmath>
#include <algorithm> // Necesario para std::min

namespace ECS {

  class ObstacleAvoidanceSystem final : public System {
  public:
    void OnUpdate(Registry& registry, float dt) override {
      registry.GetView<Transform, Kinematic, ObstacleAvoidance>().Each(
        [&](EntityID entity, Transform& transform, Kinematic& kinematic, ObstacleAvoidance& avoidance) {

          float speed = std::sqrt(kinematic.velocity.x * kinematic.velocity.x + kinematic.velocity.y * kinematic.velocity.y);
          if (speed < 0.1f) return; // Si casi no se mueve, no esquiva

          sf::Vector2f forward = kinematic.velocity / speed;

          // Proyectar DOS puntos de visión para mayor precisión a altas velocidades
          sf::Vector2f ahead = transform.position + forward * avoidance.maxSeeAhead;
          sf::Vector2f ahead2 = transform.position + forward * (avoidance.maxSeeAhead * 0.5f);

          EntityID closestObstacle = NULL_ENTITY;
          float minDistance = 999999.f;
          sf::Vector2f obstaclePos;

          // Revisar todos los obstáculos
          registry.GetView<Transform, Obstacle>().Each(
            [&](EntityID obsEntity, Transform& obsTrans, Obstacle& obs) {
              if (entity == obsEntity) return;

              // Calcular la distancia desde el centro del obstáculo a nuestros puntos de detección
              float d1 = std::sqrt(std::pow(obsTrans.position.x - ahead.x, 2) + std::pow(obsTrans.position.y - ahead.y, 2));
              float d2 = std::sqrt(std::pow(obsTrans.position.x - ahead2.x, 2) + std::pow(obsTrans.position.y - ahead2.y, 2));
              float dPos = std::sqrt(std::pow(obsTrans.position.x - transform.position.x, 2) + std::pow(obsTrans.position.y - transform.position.y, 2));

              // Tomamos la distancia más corta de los tres puntos evaluados
              float minDistToObs = std::min({ d1, d2, dPos });

              // Si el rayo choca con el radio de colisión
              if (minDistToObs < obs.radius + 15.f && minDistToObs < minDistance) {
                minDistance = minDistToObs;
                closestObstacle = obsEntity;
                obstaclePos = obsTrans.position;
              }
            });

          // Si detectamos un obstáculo, aplicamos fuerza de evasión
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