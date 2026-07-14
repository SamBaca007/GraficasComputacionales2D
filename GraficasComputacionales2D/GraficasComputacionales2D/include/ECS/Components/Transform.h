/**
 * @file Transform.h
 * @brief Definición del componente Transform para el Entity Component System (ECS).
 */

#pragma once
#include "Prerequisites.h"

 /**
  * @namespace ECS
  * @brief Espacio de nombres que agrupa las clases y estructuras del Entity Component System.
  */
namespace ECS {

  /**
   * @struct Transform
   * @brief Componente de DATOS PUROS (sin lógica). Describe la posición, rotación y escala
   *        de una entidad en el espacio 2D del motor.
   *
   * @details Como todo componente de este ECS, es un simple struct:
   *          - Se almacena en un ComponentPool<Transform>
   *          - No hereda de nada ni tiene métodos virtuales.
   *          - La lógica que lo usa vive en los System (p. ej. un futuro RenderSystem o MovementSystem).
   */
  struct Transform {

    /** @brief Posición en píxeles (coordenadas de mundo). */
    sf::Vector2f position{ 0.f,0.f };

    /** @brief Rotación en grados (SFML usa grados, sentido horario). */
    float rotation{ 0.f };

    /** @brief Escala por eje (1,1 = tamaño original). */
    sf::Vector2f scale{ 1.f, 1.f };

    /**
     * @brief Constructor por defecto.
     */
    Transform() = default;

    /**
     * @brief Constructor de conveniencia para inicializar el componente con valores específicos.
     *
     * @param pos Posición inicial en el mundo 2D.
     * @param rot Rotación inicial en grados (por defecto 0.f).
     * @param scl Escala inicial en los ejes X e Y (por defecto 1.f, 1.f).
     */
    explicit Transform(sf::Vector2f pos,
      float rot = 0.f,
      sf::Vector2f scl = { 1.f, 1.f }) noexcept
      : position(pos), rotation(rot), scale(scl) {
    }

    // Helpers (operan SOLO sobre los datos del componente)

    /**
     * @brief Desplaza la posición de forma relativa.
     *
     * @param delta Vector que representa la cantidad de desplazamiento a aplicar.
     */
    void Translate(sf::Vector2f delta) noexcept { position += delta; }

    /**
     * @brief Suma grados a la rotación actual.
     *
     * @param degrees Cantidad de grados a sumar a la rotación existente.
     */
    void Rotate(float degrees) noexcept { rotation += degrees; }
  };

} // Namespace ECS