/**
 * @file ObstacleAvoidance.h
 * @brief Componente de configuración de DATOS PUROS para la evasión de obstáculos y prevención de colisiones.
 *
 * @details Almacena los parámetros necesarios para que el sistema de navegación proyecte un
 * rayo o "antena" sensorial frente a la entidad (raycasting o box-probing). Si un obstáculo
 * interfiere con esta proyección, se generará una fuerza lateral de empuje para desviar la
 * trayectoria de la entidad y esquivar una colisión inminente sin perder fluidez en el movimiento.
 */

#pragma once

 /**
  * @namespace ECS
  * @brief Espacio de nombres que agrupa las clases y estructuras del Entity Component System.
  */
namespace ECS {

  /**
   * @struct ObstacleAvoidance
   * @brief Componente que define la capacidad sensorial y la intensidad de evasión de una entidad.
   */
  struct ObstacleAvoidance {
    /**
     * @brief Longitud máxima del rayo de visión proyectado hacia adelante.
     * @details Determina con cuánta anticipación o distancia la entidad puede detectar obstáculos
     * en su trayectoria actual. Un valor mayor permite reaccionar antes, mientras que un valor
     * menor requiere reacciones más cercanas y drásticas.
     */
     // Qué tan largo es el "rayo" de visión hacia adelante
    float maxSeeAhead = 100.0f;

    /**
     * @brief Magnitud de la fuerza de giro lateral aplicada al detectar un obstáculo.
     * @details Define qué tan violento o rápido será el cambio de dirección para evitar la colisión.
     * Debe ser lo suficientemente alto como para vencer la velocidad actual y desviar la trayectoria a tiempo.
     */
     // Qué tan fuerte será el giro para esquivar
    float avoidanceForce = 150.0f;
  };

} // Namespace ECS