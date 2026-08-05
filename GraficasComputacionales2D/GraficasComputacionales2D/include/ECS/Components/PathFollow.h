/**
 * @file PathFollow.h
 * @brief Componente de configuración para el comportamiento de seguimiento de ruta.
 *
 * @details Provee los parámetros necesarios para que un agente autónomo
 * pueda predecir su posición futura y mantenerse dentro de los límites
 * de un circuito (Path). Si el agente se sale del radio del Path,
 * utilizará estos datos para calcular la ruta más corta de regreso.
 */

#pragma once
#include "Prerequisites.h"
#include "ECS/Types.h"

namespace ECS {

  /**
   * @struct PathFollow
   * @brief Componente que enlaza a una entidad dinámica con una ruta a seguir.
   */
  struct PathFollow {
    /**
     * @brief El identificador de la entidad que contiene el componente Path.
     */
    EntityID pathEntity = NULL_ENTITY;

    /**
     * @brief Distancia hacia adelante (en el vector de velocidad) que el kart predice.
     * @details A mayor velocidad, este valor debería idealmente escalar un poco,
     * pero un valor estático funciona perfecto para empezar.
     */
    float predictTime = 25.0f;

    /**
     * @brief Distancia de proyección sobre la pista.
     * @details Cuando el kart se sale, busca el punto más cercano en la pista
     * y le suma este valor para apuntar ligeramente hacia "adelante" en la ruta,
     * logrando que el regreso sea fluido y no un giro robótico de 90 grados.
     */
    float targetOffset = 30.0f;

    /**
     * @brief Índice del segmento actual de la pista que el kart está recorriendo.
     * @details Se actualiza automáticamente por el PathFollowingSystem.
     */
    size_t currentSegment = 0;

    /**
     * @brief Vuelta actual en la que se encuentra el corredor.
     */
    int currentLap = 0;
  };

} // Namespace ECS