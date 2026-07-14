/**
 * @file Camera.h
 * @brief Componente Camera de DATOS PUROS para el Entity Component System.
 *
 * @details Convierte a una entidad en una cámara. Su POSICIÓN vive en el Transform
 *          de la misma entidad (el centro de la vista). Aquí sólo van los ajustes
 *          propios de la cámara.
 *
 *          El CameraSystem busca la cámara activa, interpola su Transform hacia
 *          el objetivo (si lo hay) y aplica la vista a la ventana.
 */

#pragma once
#include "ECS/Types.h"

 /**
  * @namespace ECS
  * @brief Espacio de nombres que agrupa las clases y estructuras del Entity Component System.
  */
namespace ECS {

  /**
   * @struct Camera
   * @brief Componente que define las propiedades de visión y seguimiento de una cámara.
   */
  struct Camera {
    /**
     * @brief Nivel de acercamiento.
     * @details Zoom > 1 acerca (ves menos mundo), < 1 aleja (ves más).
     */
    float zoom{ 1.f };

    /**
     * @brief Estado de la cámara.
     * @details Solo UNA cámara debería estar activa a la vez en el motor.
     */
    bool active{ true };

    /**
     * @brief Entidad objetivo a seguir.
     * @details Si se asigna NULL_ENTITY, la cámara actúa como cámara libre (no sigue a nadie).
     */
    EntityID followTarget{ NULL_ENTITY };

    /**
     * @brief Velocidad de seguimiento al objetivo.
     * @details Mayor valor significa que se pega más rápido al objetivo.
     *          Es independiente del framerate (ver CameraSystem), no es px/seg.
     */
    float followSpeed{ 5.f };
  };

} // Namespace ECS