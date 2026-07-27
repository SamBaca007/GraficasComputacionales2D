/**
 * @file Path.h
 * @brief Componente de configuración para la ruta del circuito (Mario Kart).
 *
 * @details Este componente define el esqueleto matemático de la pista,
 * almacenando una lista de puntos que representan el centro del asfalto
 * y un radio que dicta el límite hacia el césped o las paredes.
 */

#pragma once
#include "Prerequisites.h"

 // Si Vector2D está en otro archivo que no está incluido en Prerequisites.h, 
 // recuerda incluirlo aquí, por ejemplo: #include "Core/Vector2D.h"

 /**
  * @namespace ECS
  * @brief Espacio de nombres que agrupa las clases y estructuras del Entity Component System.
  */
namespace ECS {

  /**
   * @struct Path
   * @brief Componente de datos puros que almacena la geometría y límites de una pista.
   */
  struct Path {
    /**
     * @brief Puntos en el espacio 2D que forman la línea central del circuito.
     */
    std::vector<sf::Vector2f> points;

    /**
     * @brief El "grosor" de la pista a partir del centro.
     * @details Define qué tan lejos puede alejarse el kart del centro
     * antes de salir del asfalto.
     */
    float radius = 50.0f; // Valor por defecto seguro
  };

} // Namespace ECS