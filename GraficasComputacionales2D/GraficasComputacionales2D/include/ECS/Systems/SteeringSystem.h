/**
 * @file SteeringSystem.h
 * @brief Sistema encargado de calcular y aplicar las fuerzas de dirección (Steering Behaviors).
 *
 * @details Este sistema evalúa todas las entidades que posean los componentes Transform,
 * Kinematic y Steering. Dependiendo del comportamiento activo (Seek, Flee, Arrive),
 * calcula la fuerza direccional resultante y la acumula en la aceleración
 * del componente Kinematic para que el KinematicSystem procese el movimiento físico posteriormente.
 */

#pragma once
#include "ECS/System.h"
#include "ECS/Registry.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Kinematic.h"
#include "ECS/Components/Steering.h"
#include <cmath>

 /**
  * @namespace ECS
  * @brief Espacio de nombres que agrupa las clases y estructuras del Entity Component System.
  */
namespace ECS {

  /**
   * @class SteeringSystem
   * @brief Sistema de IA que procesa y calcula las fuerzas de navegación de las entidades.
   */
  class SteeringSystem final : public System {
  public:
    /**
     * @brief Constructor por defecto de SteeringSystem.
     */
    SteeringSystem() = default;

    /**
     * @brief Actualiza el sistema calculando las fuerzas de dirección para cada entidad compatible.
     * @details Resuelve el objetivo (sea un punto estático o una entidad dinámica), ejecuta
     * el algoritmo del comportamiento activo y acumula la fuerza en la aceleración
     * cinemática de la entidad.
     * * @param registry Referencia al registro principal del ECS que contiene las entidades y componentes.
     * @param deltaTime Tiempo transcurrido desde el último frame (en segundos).
     */
    void OnUpdate(Registry& registry, float deltaTime) override {
      registry.GetView<Transform, Kinematic, Steering>().Each(
        [&registry](EntityID entity, Transform& transform,
          Kinematic& kinematic, Steering& steering) {

            // 1. Determinar la posición final del objetivo
            sf::Vector2f targetPos = steering.target;

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
              break;
            }

            // 3. ACUMULAR LA FUERZA
            // Nota el "+=" en lugar de "=". Esto permite que Obstacle Avoidance 
            // u otros sistemas sumen sus propias fuerzas sin sobrescribirse.
            kinematic.acceleration += steeringForce;

            // ¡Y listo! KinematicSystem se encargará de mover la figura después.
        }
      );
    }

  private:
    /**
     * @brief Trunca la magnitud de un vector para que no exceda un valor máximo.
     * * @param vector El vector bidimensional que se desea limitar.
     * @param max La magnitud (longitud) máxima permitida para el vector.
     * @return sf::Vector2f El vector truncado si excedía el máximo, o el vector original intacto.
     */
    static sf::Vector2f Truncate(sf::Vector2f vector, float max) {
      float lengthSq = (vector.x * vector.x) + (vector.y * vector.y);
      if (lengthSq > max * max) {
        float length = std::sqrt(lengthSq);
        return (vector / length) * max;
      }
      return vector;
    }

    /**
     * @brief Normaliza un vector (lo convierte en un vector unitario de magnitud 1).
     * * @param vector Referencia al vector que será modificado in-situ.
     * @return float La magnitud original del vector antes de ser normalizado.
     */
    static float Normalize(sf::Vector2f& vector) {
      float length = std::sqrt((vector.x * vector.x) + (vector.y * vector.y));
      if (length > 0) {
        vector /= length;
      }
      return length;
    }

    /**
     * @brief Calcula la fuerza de dirección para el comportamiento "Seek" (Buscar).
     * @details Genera una fuerza que impulsa a la entidad directamente hacia la posición objetivo a máxima velocidad.
     * * @param position La posición actual de la entidad.
     * @param target La posición hacia la que se desea mover.
     * @param kinematic El componente cinemático para obtener los límites de velocidad y fuerza.
     * @return sf::Vector2f El vector de fuerza de dirección resultante truncado a maxForce.
     */
    static sf::Vector2f CalculateSeek(const sf::Vector2f& position,
      const sf::Vector2f& target, const Kinematic& kinematic) {
      sf::Vector2f desiredVelocity = target - position;
      Normalize(desiredVelocity);
      desiredVelocity *= kinematic.maxSpeed;

      sf::Vector2f steering = desiredVelocity - kinematic.velocity;
      return Truncate(steering, kinematic.maxForce);
    }

    /**
     * @brief Calcula la fuerza de dirección para el comportamiento "Flee" (Huir).
     * @details Genera una fuerza que impulsa a la entidad en dirección exactamente opuesta al objetivo.
     * * @param position La posición actual de la entidad.
     * @param target La posición de la amenaza o punto del que se desea escapar.
     * @param kinematic El componente cinemático para obtener los límites de velocidad y fuerza.
     * @return sf::Vector2f El vector de fuerza de dirección resultante truncado a maxForce.
     */
    static sf::Vector2f CalculateFlee(const sf::Vector2f& position,
      const sf::Vector2f& target, const Kinematic& kinematic) {
      sf::Vector2f desiredVelocity = position - target;
      Normalize(desiredVelocity);
      desiredVelocity *= kinematic.maxSpeed;

      sf::Vector2f steering = desiredVelocity - kinematic.velocity;
      return Truncate(steering, kinematic.maxForce);
    }

    /**
     * @brief Calcula la fuerza de dirección para el comportamiento "Arrive" (Llegar).
     * @details Similar a Seek, pero cuando la entidad entra dentro del radio de frenado (slowingRadius),
     * su velocidad deseada disminuye proporcionalmente a la distancia restante, permitiendo
     * una parada suave exactamente en el objetivo.
     * * @param position La posición actual de la entidad.
     * @param target La posición de destino donde la entidad debe detenerse.
     * @param kinematic El componente cinemático para obtener los límites de velocidad y fuerza.
     * @param slowingRadius Radio alrededor del objetivo donde comienza la desaceleración.
     * @return sf::Vector2f El vector de fuerza de dirección resultante,
     * o una fuerza de frenado opuesta a la velocidad si ya está en el punto.
     */
    static sf::Vector2f CalculateArrive(const sf::Vector2f& position,
      const sf::Vector2f& target, const Kinematic& kinematic, float slowingRadius) {
      sf::Vector2f desiredVelocity = target - position;
      float distance = Normalize(desiredVelocity);

      if (distance > 0) {
        float speed = kinematic.maxSpeed;
        if (distance < slowingRadius) {
          speed = kinematic.maxSpeed * (distance / slowingRadius);
        }
        desiredVelocity *= speed;

        sf::Vector2f steering = desiredVelocity - kinematic.velocity;
        return Truncate(steering, kinematic.maxForce);
      }
      return -kinematic.velocity;
    }
  };

} // Namespace ECS