/**
 * @file Pursuit.h
 * @brief Componente de configuración para el comportamiento de intercepción o persecución predictiva (Pursuit).
 *
 * @details A diferencia de un comportamiento "Seek" tradicional que simplemente se dirige
 * hacia la posición actual del objetivo, "Pursuit" proyecta la trayectoria y velocidad
 * del objetivo para interceptarlo en su posición futura. Este componente almacena
 * el identificador de la entidad dinámica a la cual se desea anticipar y dar caza.
 */

#pragma once
#include "ECS/Types.h"

 /**
  * @namespace ECS
  * @brief Espacio de nombres que agrupa las clases y estructuras del Entity Component System.
  */
namespace ECS {

  /**
   * @struct Pursuit
   * @brief Componente de datos puros que identifica al objetivo que se desea interceptar.
   */
  struct Pursuit {
    /**
     * @brief El identificador único de la entidad dinámica a la que queremos interceptar.
     * @details Debe ser una entidad válida y viva dentro del registro (que posea preferiblemente
     * los componentes Transform y Kinematic para poder predecir su movimiento).
     * Si se mantiene en NULL_ENTITY, el comportamiento de intercepción se desactivará o ignorará.
     */
     // La entidad a la que queremos interceptar
    EntityID targetEntity = NULL_ENTITY;
  };

} // Namespace ECS