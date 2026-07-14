/**
 * @file WanderSystem.h
 * @brief Sistema encargado de procesar y aplicar el comportamiento de deambulación (Wander).
 *
 * @details Este sistema evalúa todas las entidades que posean los componentes Transform,
 * Kinematic y Wander. Utiliza el algoritmo del círculo de Craig Reynolds para simular
 * una patrulla o movimiento errático natural, proyectando un círculo imaginario frente
 * a la entidad y variando un ángulo aleatoriamente en cada frame para calcular la
 * fuerza direccional resultante, la cual se acumula en la aceleración del componente Kinematic.
 */

#pragma once
#include "ECS/System.h"
#include "ECS/Registry.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Kinematic.h"
#include "ECS/Components/Wander.h"
#include <cmath>
#include <cstdlib>

 /**
  * @namespace ECS
  * @brief Espacio de nombres que agrupa las clases y estructuras del Entity Component System.
  */
namespace ECS {

  /**
   * @class WanderSystem
   * @brief Sistema de IA que calcula la fuerza de deambulación errática y fluida para las entidades.
   */
  class WanderSystem final : public System {
  public:
    /**
     * @brief Actualiza el sistema calculando y aplicando la fuerza de deambulación para cada entidad compatible.
     * @details Por cada iteración, modifica el ángulo actual mediante un valor aleatorio escalado
     * por el `maxJitter`. Luego, calcula la posición del centro del círculo imaginario en base a la
     * dirección actual y suma el vector de desplazamiento en el borde del círculo (`circleRadius`).
     * Finalmente, acumula la fuerza obtenida en la aceleración del componente Kinematic.
     * * @param registry Referencia al registro principal del ECS que contiene las entidades y sus componentes.
     * @param dt Tiempo transcurrido desde el último frame (en segundos).
     */
    void OnUpdate(Registry& registry, float dt) override {
      registry.GetView<Transform, Kinematic, Wander>().Each(
        [&](EntityID entity, Transform& transform, Kinematic& kinematic, Wander& wander) {

          // Variar el ángulo aleatoriamente usando tu maxJitter
          wander.wanderAngle += ((std::rand() % 200) - 100) * 0.01f * wander.maxJitter;

          // Calcular el centro del círculo de deambulación usando tu circleDistance
          sf::Vector2f circleCenter = kinematic.velocity;
          float speed = std::sqrt(circleCenter.x * circleCenter.x + circleCenter.y * circleCenter.y);
          if (speed > 0) {
            circleCenter = (circleCenter / speed) * wander.circleDistance;
          }
          else {
            circleCenter = sf::Vector2f(wander.circleDistance, 0);
          }

          // Calcular el desplazamiento en el borde del círculo usando tu circleRadius
          sf::Vector2f displacement(
            std::cos(wander.wanderAngle) * wander.circleRadius,
            std::sin(wander.wanderAngle) * wander.circleRadius
          );

          // Sumamos la fuerza a la aceleración
          sf::Vector2f wanderForce = circleCenter + displacement;
          kinematic.acceleration += wanderForce;
        });
    }
  };
} // Namespace ECS