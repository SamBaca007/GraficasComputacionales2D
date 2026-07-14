/**
 * @file Obstacle.h
 * @brief Componente de DATOS PUROS para marcar e identificar entidades como obstáculos evitables en el mundo.
 *
 * @details Actúa como una etiqueta (tag) espacial que permite a los sistemas de inteligencia
 * artificial reconocer qué entidades en el entorno representan peligros físicos o barreras.
 * Define el volumen o área de colisión circular que el sistema ObstacleAvoidance utilizará
 * para proyectar intersecciones y calcular las fuerzas de desvío necesarias para evitar colisiones.
 */

#pragma once

 /**
  * @namespace ECS
  * @brief Espacio de nombres que agrupa las clases y estructuras del Entity Component System.
  */
namespace ECS {

  /**
   * @struct Obstacle
   * @brief Componente que define a una entidad como una barrera física y especifica su tamaño de colisión.
   */
  struct Obstacle {
    /**
     * @brief Radio del área de colisión circular del obstáculo.
     * @details Determina el espacio físico que ocupa el obstáculo a partir de su posición central (Transform).
     * El sistema de evasión utiliza este radio para evaluar si un rayo de visión pasará demasiado
     * cerca del centro de la entidad y, de ser así, accionar una fuerza de repulsión o desvío.
     */
     // El radio de colisión del obstáculo
    float radius = 40.0f;
  };

} // Namespace ECS