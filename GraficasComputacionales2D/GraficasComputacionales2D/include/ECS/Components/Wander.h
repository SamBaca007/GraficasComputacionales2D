/**
 * @file Wander.h
 * @brief Componente de configuración y funciones matemáticas para el comportamiento de deambulación (Wander).
 *
 * @details Este componente almacena los parámetros necesarios para simular un movimiento errático
 * y orgánico (algoritmo del círculo de deambulación de Craig Reynolds). En lugar de cambiar
 * de dirección al azar bruscamente, proyecta un círculo imaginario frente a la entidad y
 * desplaza un punto objetivo a lo largo de su circunferencia, logrando giros suaves y naturales.
 */

#pragma once
#include <SFML/System/Vector2.hpp>
#include <cmath>

 /**
  * @namespace ECS
  * @brief Espacio de nombres que agrupa las clases y estructuras del Entity Component System.
  */
namespace ECS {

  /**
   * @struct Wander
   * @brief Componente de datos puros que define los parámetros del algoritmo de deambulación.
   */
  struct Wander {
    /**
     * @brief Distancia desde el centro de la entidad hacia adelante donde se proyecta el círculo imaginario.
     */
    float circleDistance = 120.0f; // Qué tan adelante de la entidad está el círculo

    /**
     * @brief El radio del círculo imaginario de deambulación.
     * @details Un radio mayor permite giros más amplios y drásticos.
     */
    float circleRadius = 60.0f;    // El radio del círculo imaginario

    /**
     * @brief El ángulo actual (en radianes) sobre la circunferencia del círculo donde se ubica el objetivo temporal.
     */
    float wanderAngle = 0.0f;      // El ángulo actual del objetivo (en radianes)

    /**
     * @brief Variación aleatoria máxima que puede aplicarse al ángulo en cada actualización.
     * @details Controla qué tanto "nerviosismo" o cambio de dirección errático tiene el movimiento por segundo.
     */
    float maxJitter = 0.4f;        // Qué tanto puede cambiar el ángulo por segundo
  };

  /**
   * @brief Calcula la magnitud (longitud) de un vector bidimensional.
   * * @param v Vector constante del cual se desea obtener la longitud.
   * @return float La longitud o norma del vector utilizando el teorema de Pitágoras.
   */
  inline float length(const sf::Vector2f& v) {
    return std::sqrt(v.x * v.x + v.y * v.y);
  }

  /**
   * @brief Devuelve la versión normalizada de un vector (vector unitario con magnitud 1 y misma dirección).
   * * @param v Vector constante que se desea normalizar.
   * @return sf::Vector2f El vector normalizado, o un vector nulo (0, 0)
   * si la magnitud original es cero para evitar división por cero.
   */
  inline sf::Vector2f normalize(const sf::Vector2f& v) {
    float len = length(v);
    if (len != 0.f) return v / len;
    return sf::Vector2f(0.f, 0.f);
  }

} // Namespace ECS