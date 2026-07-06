#pragma once

#include <SFML/System/Vector2.hpp>

// ====================================================================
// ECS/Components/Kinematic.h
//
// Kinematic - Componente de DATOS PUROS para movimiento físico básica.
// Almacena los vectores de estado y los límites físicos necesarios 
// para que los Steering Behaviors calculen el movimiento orgánico.
// ====================================================================
namespace ECS {

  struct Kinematic {
    // Vectores de estado físico
    sf::Vector2f velocity = { 0.f, 0.f };
    sf::Vector2f acceleration = { 0.f, 0.f };

    // Límites de control (valores por defecto modificables en el Inspector)
    float maxSpeed = 200.f;  // Velocidad máxima en píxeles por segundo
    float maxForce = 150.f;  // Fuerza máxima de giro/dirección
  };
} // Namespace ECS