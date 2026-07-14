/**
 * @file Steering.h
 * @brief Componente Steering de DATOS PUROS para la Inteligencia Artificial de movimiento.
 *
 * @details Actúa como el "volante" de la entidad. Define qué patrón de comportamiento
 *          (Steering Behavior) debe seguir la entidad y almacena los datos necesarios
 *          (como el objetivo a seguir o el radio de frenado) para que el SteeringSystem
 *          calcule las fuerzas direccionales a aplicar sobre el componente Kinematic.
 */

#pragma once

#include "ECS/Types.h"
#include <SFML/System/Vector2.hpp>

 /**
  * @namespace ECS
  * @brief Espacio de nombres que agrupa las clases y estructuras del Entity Component System.
  */
namespace ECS {

  /**
   * @enum SteeringBehaviorType
   * @brief Enumeración para identificar qué comportamiento de dirección está activo.
   */
   // Enum para identificar qué comportamiento está activo
  enum class SteeringBehaviorType {
    /** @brief Ningún comportamiento activo. La entidad mantiene su trayectoria o se detiene. */
    None,
    /** @brief Se dirige directamente en línea recta hacia la posición objetivo. */
    Seek,
    /** @brief Huye en dirección opuesta a la posición objetivo. */
    Flee,
    /** @brief Se dirige al objetivo, pero desacelera suavemente al acercarse. */
    Arrive,
    /** @brief Movimiento errático y fluido simulando deambular o patrullar sin rumbo fijo. */
    Wander,
    /** @brief Intercepta a un objetivo en movimiento prediciendo su posición futura. */
    Pursuit,
    /** @brief Esquiva objetos y evita colisiones proyectando un rayo visual hacia adelante. */
    ObstacleAvoidance
  };

  /**
   * @struct Steering
   * @brief Componente que contiene la configuración y los objetivos de navegación de una entidad.
   */
  struct Steering {
    /**
     * @brief Comportamiento actual de la entidad (por defecto, ninguno).
     */
     // Comportamiento actual por defecto
    SteeringBehaviorType currentBehavior = SteeringBehaviorType::None;

    /**
     * @brief El punto estático en el mundo al que queremos ir (o del que queremos huir).
     */
     // El punto en el mundo al que queremos ir (o del que queremos huir)
    sf::Vector2f target = { 0.f, 0.f };

    /**
     * @brief Entidad objetivo dinámica.
     * @details Opcional: Si prefieres que siga a una entidad dinámicamente en lugar de
     *          un punto fijo, puedes guardar su ID (al igual que Camera.h).
     *          NULL_ENTITY significa que se usará el punto fijo 'target'.
     */
     // Opcional: Si prefieres que siga a una entidad dinámicamente en lugar de 
     // un punto fijo, puedes guardar su ID (al igual que Camera.h)
    EntityID targetEntity = NULL_ENTITY;

    /**
     * @brief Radio de frenado exclusivo para el comportamiento 'Arrive'.
     * @details Dicta a qué distancia del objetivo la entidad debe empezar a desacelerar
     *          para detenerse exactamente en la marca.
     */
     // Radio de frenado exclusivo para el comportamiento 'Arrive'
     // Dicta a qué distancia del objetivo la entidad debe empezar a desacelerar.
    float slowingRadius = 150.f;
  };

} // Namespace ECS