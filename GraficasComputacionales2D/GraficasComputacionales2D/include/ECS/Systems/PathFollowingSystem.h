/**
 * @file PathFollowingSystem.h
 * @brief Sistema para mantener a las entidades dentro de un circuito (Path) usando el algoritmo de Craig Reynolds.
 */

#pragma once
#include "Prerequisites.h"
#include "ECS/System.h"               // <-- NUEVO: Clase base del ECS
#include "ECS/Registry.h"             // <-- NUEVO: Para consultar componentes
#include "ECS/Components/Transform.h" // <-- NUEVO: Para saber dónde está el kart
#include "ECS/Components/Kinematic.h" // <-- NUEVO: Para saber a qué velocidad va
#include "ECS/Components/Path.h"
#include "ECS/Components/PathFollow.h"
#include "ECS/Components/Steering.h"
#include <cmath>
#include <algorithm>

namespace ECS {

  /**
   * @class PathFollowingSystem
   * @brief Modifica la fuerza de dirección (Steering) para seguir una ruta prediciendo la posición futura.
   */
   // <-- CORRECCIÓN: Herencia pública de System para que Registry.h no falle
  class PathFollowingSystem final : public System {
  private:
    // --- Utilidades Matemáticas SFML ---

    // @brief Producto Punto (Dot Product) de dos vectores.
    static float Dot(const sf::Vector2f& a, const sf::Vector2f& b) {
      return a.x * b.x + a.y * b.y;
    }

    // @brief Longitud al cuadrado de un vector (más rápido que usar std::sqrt).
    static float LengthSq(const sf::Vector2f& v) {
      return Dot(v, v);
    }

    // @brief Magnitud real de un vector.
    static float Length(const sf::Vector2f& v) {
      return std::sqrt(LengthSq(v));
    }

    // @brief Normaliza un vector a longitud 1.
    static sf::Vector2f Normalize(const sf::Vector2f& v) {
      float len = Length(v);
      return (len > 0.0f) ? (v / len) : sf::Vector2f(0.0f, 0.0f);
    }

    /**
     * @brief Proyección perpendicular (Punto Normal) del punto P sobre el segmento AB.
     */
    static sf::Vector2f GetNormalPoint(const sf::Vector2f& p, const sf::Vector2f& a, const sf::Vector2f& b) {
      sf::Vector2f ap = p - a;
      sf::Vector2f ab = b - a;

      float abLenSq = LengthSq(ab);
      if (abLenSq == 0.0f) return a; // Evita división por cero si A y B son el mismo punto

      float t = Dot(ap, ab) / abLenSq;
      t = std::max(0.0f, std::min(1.0f, t)); // Restringe el punto dentro del segmento

      return a + ab * t;
    }

  public:
    /**
     * @brief Calcula la fuerza Seek para seguir la pista, anticipando curvas.
     */
    static sf::Vector2f CalculateSteering(const sf::Vector2f& currentPos,
      const sf::Vector2f& velocity,
      float maxSpeed,
      const Path& path,
      const PathFollow& follow)
    {
      if (path.points.size() < 2) {
        return sf::Vector2f(0.0f, 0.0f);
      }

      // 1. Predicción
      sf::Vector2f vNorm = Normalize(velocity);
      sf::Vector2f futurePos = currentPos + (vNorm * follow.predictTime);

      sf::Vector2f targetNormalPoint(0.0f, 0.0f);
      sf::Vector2f targetDir(0.0f, 0.0f);
      float worldRecord = 999999.0f;

      // NUEVO: Guardaremos en qué segmento de la pista estamos
      size_t bestIndex = 0;

      // 2. Búsqueda del segmento más cercano
      for (size_t i = 0; i < path.points.size(); ++i) {
        sf::Vector2f a = path.points[i];
        sf::Vector2f b = path.points[(i + 1) % path.points.size()]; // Bucle cerrado

        sf::Vector2f normalPoint = GetNormalPoint(futurePos, a, b);
        float dist = Length(futurePos - normalPoint);

        if (dist < worldRecord) {
          worldRecord = dist;
          targetNormalPoint = normalPoint;
          targetDir = Normalize(b - a);
          bestIndex = i; // Guardamos el índice ganador
        }
      }

      // 3. Decisión y Corrección (Curvas Suaves)
      // Obtenemos el vértice final de nuestro segmento actual
      sf::Vector2f b = path.points[(bestIndex + 1) % path.points.size()];

      // ¿Cuánta distancia hay desde nuestro punto normal hasta el final del segmento?
      float distToEnd = Length(b - targetNormalPoint);

      sf::Vector2f target;

      // Si el "targetOffset" cabe en la línea actual, lo aplicamos normal
      if (follow.targetOffset <= distToEnd) {
        target = targetNormalPoint + (targetDir * follow.targetOffset);
      }
      // Si el offset se sale de la línea, "doblamos" la distancia sobrante al siguiente segmento
      else {
        // Vértice final del SIGUIENTE segmento
        sf::Vector2f nextB = path.points[(bestIndex + 2) % path.points.size()];
        sf::Vector2f nextDir = Normalize(nextB - b);

        float remainingOffset = follow.targetOffset - distToEnd;
        target = b + (nextDir * remainingOffset);
      }

      // Fuerza Seek constante hacia ese punto futuro continuo
      sf::Vector2f desiredVelocity = Normalize(target - currentPos) * maxSpeed;
      return desiredVelocity - velocity;
    }

    // <-- NUEVO: El bucle que el Registry llama cada frame para aplicar la lógica
    void OnUpdate(Registry& registry, float /*deltaTime*/) override {
      // Iteramos sobre todos los corredores de la pista
      registry.GetView<Transform, Kinematic, PathFollow>().Each(
        [&registry](EntityID entity, Transform& transform, Kinematic& kinematic, PathFollow& follow) {

          // Si la pista fue destruida por alguna razón, ignoramos
          if (!registry.IsAlive(follow.pathEntity) || !registry.HasComponent<Path>(follow.pathEntity)) {
            return;
          }

          // Extraemos la geometría de la pista a la que está asignado el corredor
          const Path& path = registry.GetComponent<Path>(follow.pathEntity);

          // Calculamos la fuerza matemática usando tu algoritmo
          sf::Vector2f steeringForce = CalculateSteering(
            transform.position,
            kinematic.velocity,
            kinematic.maxSpeed,
            path,
            follow
          );

          // IMPORTANTE: Sumamos la fuerza directamente a la aceleración
          // Así respetamos la arquitectura de tu motor, permitiendo que
          // otros sistemas (como Obstacle Avoidance) también sumen las suyas.
          
          kinematic.acceleration += steeringForce;
        }
      );
    }
  };

} // Namespace ECS