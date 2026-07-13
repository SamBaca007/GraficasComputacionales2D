#pragma once
#include "ECS/Types.h"

namespace ECS {
  struct Pursuit {
    // La entidad a la que queremos interceptar
    EntityID targetEntity = NULL_ENTITY;
  };
} // Namespace ECS