/**
 * @file PursuitSystem.h
 * @brief Sistema encargado de calcular y aplicar la fuerza de persecución predictiva (Pursuit).
 *
 * @details Este sistema evalúa a las entidades que poseen los componentes Transform,
 * Kinematic y Pursuit. A diferencia de un seguimiento directo, calcula el tiempo de
 * anticipación basado en la distancia actual y la velocidad máxima del perseguidor para
 * predecir la posición futura del objetivo en movimiento. Finalmente, aplica una fuerza
 * de tipo "Seek" hacia esa coordenada futura y la acumula en la aceleración del componente Kinematic.
 */

#pragma once
#include "ECS/System.h"
#include "ECS/Registry.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Kinematic.h"
#include "ECS/Components/Pursuit.h"
#include <cmath>

 /**
  * @namespace ECS
  * @brief Espacio de nombres que agrupa las clases y estructuras del Entity Component System.
  */
namespace ECS {

  /**
   * @class PursuitSystem
   * @brief Sistema de IA que procesa la intercepción inteligente de objetivos móviles.
   */
  class PursuitSystem final : public System {
  public:
    /**
     * @brief Actualiza el sistema calculando la fuerza de intercepción para cada entidad compatible.
     * @details Para cada entidad, verifica que el objetivo asignado exista y tenga los componentes
     * necesarios. Luego, calcula el tiempo de anticipación (`lookAheadTime`) dividiendo la distancia
     * entre la velocidad máxima del perseguidor, proyecta la posición futura del objetivo sumando su
     * velocidad escalada por dicho tiempo, y calcula una fuerza direccional hacia ese punto que
     * finalmente se suma a la aceleración del perseguidor.
     * * @param registry Referencia al registro principal del ECS que gestiona entidades y componentes.
     * @param dt Tiempo transcurrido desde el último frame en segundos (no utilizado directamente aquí,
     * pero requerido por la interfaz System).
     */
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