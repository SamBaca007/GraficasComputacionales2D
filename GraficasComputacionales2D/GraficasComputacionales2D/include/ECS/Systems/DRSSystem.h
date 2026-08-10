#pragma once
#include "ECS/System.h"
#include "ECS/Registry.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Kinematic.h"
#include "ECS/Components/PathFollow.h"
#include "ECS/Components/Render.h"
#include "ECS/Components/DRS.h"
#include "ECS/Components/Path.h" 

namespace ECS {
  class DRSSystem : public System {
  public:
    // OnStart coincide con la definición de System.h
    void OnStart(Registry& registry) override {
      // Inicialización opcional
    }

    // EL CAMBIO CLAVE: OnUpdate con los parámetros exactos dictados por System.h
    void OnUpdate(Registry& registry, float deltaTime) override {

      ECS::Path trackPath;
      registry.GetView<ECS::Path>().Each([&](ECS::EntityID, const ECS::Path& path) {
        trackPath = path;
        });

      if (trackPath.points.empty()) return;

      auto view = registry.GetView<ECS::Transform, ECS::Kinematic, ECS::PathFollow, ECS::DRS>();

      view.Each([&](ECS::EntityID entityA, ECS::Transform& transA, ECS::Kinematic& kinA, ECS::PathFollow& followA, ECS::DRS& drsA) {

        if (!drsA.initialized) {
          drsA.originalMaxSpeed = kinA.maxSpeed;
          drsA.originalAccel = kinA.accelerationRate;
          drsA.initialized = true;
        }

        bool shouldActivate = false;

        size_t nextWaypointIndex = (followA.currentSegment + 1) % trackPath.points.size();
        sf::Vector2f targetPos = trackPath.points[nextWaypointIndex];

        float dx = targetPos.x - transA.position.x;
        float dy = targetPos.y - transA.position.y;
        float distToNextWaypoint = std::sqrt(dx * dx + dy * dy);

        if (distToNextWaypoint > drsA.safeBrakingZone) {

          auto viewOthers = registry.GetView<ECS::Transform, ECS::PathFollow>();
          viewOthers.Each([&](ECS::EntityID entityB, ECS::Transform& transB, ECS::PathFollow& followB) {
            if (entityA == entityB) return;

            if (followB.currentSegment == followA.currentSegment ||
              followB.currentSegment == (followA.currentSegment + 1) % trackPath.points.size()) {

              float kdx = transB.position.x - transA.position.x;
              float kdy = transB.position.y - transA.position.y;
              float distBetweenKarts = std::sqrt(kdx * kdx + kdy * kdy);

              if (distBetweenKarts < drsA.detectionRange) {
                float b_dx = targetPos.x - transB.position.x;
                float b_dy = targetPos.y - transB.position.y;
                float distBToWaypoint = std::sqrt(b_dx * b_dx + b_dy * b_dy);

                if (distBToWaypoint < distToNextWaypoint) {
                  shouldActivate = true;
                }
              }
            }
            });
        }

        if (shouldActivate && !drsA.isActive) {
          drsA.isActive = true;
          kinA.maxSpeed = drsA.originalMaxSpeed + drsA.speedBoost;
          kinA.accelerationRate = drsA.originalAccel + drsA.accelBoost;
        }
        else if (!shouldActivate && drsA.isActive) {
          drsA.isActive = false;
          kinA.maxSpeed = drsA.originalMaxSpeed;
          kinA.accelerationRate = drsA.originalAccel;
        }
        });
    }
  };
}