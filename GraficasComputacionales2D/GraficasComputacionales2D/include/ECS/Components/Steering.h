#pragma once

#include "ECS/Types.h"
#include <SFML/System/Vector2.hpp>

namespace ECS {

  // Enum para identificar qué comportamiento está activo
  enum class SteeringBehaviorType {
    None,
    Seek,
    Flee,
    Arrive
  };

  struct Steering {
    // Comportamiento actual por defecto
    SteeringBehaviorType currentBehavior = SteeringBehaviorType::None;

    // El punto en el mundo al que queremos ir (o del que queremos huir)
    sf::Vector2f target = { 0.f, 0.f };

    // Opcional: Si prefieres que siga a una entidad dinámicamente en lugar de 
    // un punto fijo, puedes guardar su ID (al igual que Camera.h)
    EntityID targetEntity = NULL_ENTITY;

    // Radio de frenado exclusivo para el comportamiento 'Arrive'
    // Dicta a qué distancia del objetivo la entidad debe empezar a desacelerar.
    float slowingRadius = 150.f;
  };

} // Namespace ECS