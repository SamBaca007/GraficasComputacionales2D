/**
 * @file ObstacleAvoidanceSystem.h
 * @brief Sistema de Inteligencia Artificial para la detección y evasión activa de obstáculos.
 *
 * @details Evaluando las entidades que poseen Transform, Kinematic y ObstacleAvoidance, este
 * sistema calcula un "rayo de visión" vectorial hacia la dirección de movimiento actual.
 * Luego, itera sobre las entidades que posean el componente Obstacle en el mundo; si detecta
 * una colisión inminente en su trayectoria proyectada, genera una fuerza lateral repulsiva para
 * esquivar el peligro y la acumula en el componente Kinematic sin sobrescribir otras fuerzas
 * de navegación que estén activas al mismo tiempo.
 */

#pragma once
#include "ECS/System.h"
#include "ECS/Registry.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Kinematic.h"
#include "ECS/Components/ObstacleAvoidance.h"
#include "ECS/Components/Obstacle.h"
#include <cmath>

 /**
  * @namespace ECS
  * @brief Espacio de nombres que agrupa las clases y structures del Entity Component System.
  */
namespace ECS {

  /**
   * @class ObstacleAvoidanceSystem
   * @brief Sistema encargado de prevenir colisiones mediante el desvío dinámico de trayectorias.
   */
  class ObstacleAvoidanceSystem final : public System {
  public:
    /**
     * @brief Actualiza el sistema comprobando obstrucciones y aplicando fuerzas evasivas.
     * @details Si la entidad está en movimiento (> 0.1f), proyecta un punto temporal frente
     * a ella (`ahead`) en función del vector velocidad normalizado multiplicado por `maxSeeAhead`.
     * Posterior a ello, busca la entidad con componente Obstacle más cercana cuyo radio colisione
     * con dicho punto. Si hay colisión, calcula una fuerza perpendicular o de alejamiento escalada
     * por `avoidanceForce` y la suma a la aceleración del componente Kinematic.
     * * @param registry Referencia al registro del ECS donde residen todas las entidades y componentes.
     * @param dt Tiempo transcurrido desde el último frame (en segundos).
     */
    void OnUpdate(Registry& registry, float dt) override {
      registry.GetView<Transform, Kinematic, ObstacleAvoidance>().Each(
        [&](EntityID entity, Transform& transform, Kinematic& kinematic, ObstacleAvoidance& avoidance) {

          float speed = std::sqrt(kinematic.velocity.x * kinematic.velocity.x + kinematic.velocity.y
            * kinematic.velocity.y);
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