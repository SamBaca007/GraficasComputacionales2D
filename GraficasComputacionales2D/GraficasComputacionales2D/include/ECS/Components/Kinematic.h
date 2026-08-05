/**
 * @file Kinematic.h
 * @brief Componente Kinematic de DATOS PUROS para el movimiento físico básico.
 *
 * @details Almacena los vectores de estado y los límites físicos necesarios
 *          para que los Steering Behaviors calculen el movimiento orgánico de la entidad.
 */

#pragma once

#include <SFML/System/Vector2.hpp>

 /**
  * @namespace ECS
  * @brief Espacio de nombres que agrupa las clases y estructuras del Entity Component System.
  */
namespace ECS {

  /**
   * @struct Kinematic
   * @brief Componente que contiene la información física y los límites de movimiento de una entidad.
   */
  struct Kinematic {
    // Vectores de estado físico

    /** @brief Vector de velocidad actual de la entidad en el espacio 2D. */
    sf::Vector2f velocity = { 0.f, 0.f };

    /** @brief Vector de aceleración actual aplicada a la entidad en el frame. */
    sf::Vector2f acceleration = { 0.f, 0.f };

    // Límites de control (valores por defecto modificables en el Inspector)

    /** @brief Límite de velocidad máxima permitida en píxeles por segundo. */
    float maxSpeed = 200.f;  // Velocidad máxima en píxeles por segundo

    /** @brief Límite de fuerza máxima permitida para alterar la dirección (giro/dirección). */
    float maxForce = 150.f;  // Fuerza máxima de giro/dirección

    // --- NUEVO: Tasa de aceleración individual del motor ---
    float accelerationRate = 85.0f;
  };
} // Namespace ECS