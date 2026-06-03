#pragma once
#include "ECS/Types.h"
#include "ECS/ComponentPool.h"
#include "ECS/View.h"
#include "ECS/System.h"

namespace ECS {
  class Registry {

  private:
    // Entidades
    std::vector<EntityID> m_entities;
    std::vector<EntityVersion> m_versions;
    std::vector<EntityIndex> m_freeList;

    // Componentes
    std::unordered_map<ComponentTypeID, std::unique_ptr<IComponentPool>> m_componentPools;

    // Sistemas
    std::vector<std::unique_ptr<System>> m_systems;
  };
}